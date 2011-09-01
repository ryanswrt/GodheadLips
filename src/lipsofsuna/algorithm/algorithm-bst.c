/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/* Code based on research papers "Scapegoat Trees" by Galperin and Rives
   and "Tree Rebalancing in Optimal Time and Space" by Stout and Warren. */

/**
 * \addtogroup LIAlg Algorithm
 * @{
 * \addtogroup LIAlgBst Bst
 * @{
 */

#include <math.h>
#include <lipsofsuna/system.h>
#include "algorithm-bst.h"

#define LIALG_SCAPEGOAT_ALPHA 0.55

static inline int
private_halpha (int n);

static inline void
private_each (LIAlgBstNode*   node,
              LIAlgBstForeach func);

static inline void
private_free (LIAlgBst*     self,
              LIAlgBstNode* node);

static inline LIAlgBstNode*
private_index (LIAlgBstNode* node,
               int*          counter);

static inline int
private_size (LIAlgBstNode* node);

static inline LIAlgBstNode*
private_rebalance (LIAlgBstNode* root,
                   int           size);

static inline void
private_makelist (LIAlgBstNode* root);

static inline void
private_maketree (LIAlgBstNode* root,
                  int           size);

static inline void
private_compress (LIAlgBstNode* root,
                  int           count);

/****************************************************************************/

/**
 * \brief Creates a new self-balancing binary search tree.
 *
 * \param cmp Comparison function.
 * \param malloc Malloc function.
 * \param free Free function.
 * \return New binary search tree or NULL.
 */
LIAlgBst*
lialg_bst_new (LIAlgBstCompare cmp,
               LIAlgBstMalloc  malloc,
               LIAlgBstFree    free)
{
	LIAlgBst* self;

	self = lisys_malloc (sizeof (LIAlgBst));
	if (self == NULL)
		return NULL;
	self->size = 0;
	self->max_size = 0;
	self->root = NULL;
	self->cmp = cmp;
	self->free = free;
	self->malloc = malloc;

	return self;
}

/**
 * \brief Frees the binary tree.
 *
 * \param self Binary search tree.
 */
void
lialg_bst_free (LIAlgBst* self)
{
	private_free (self, self->root);
	self->free (self);
}

/**
 * \brief Clears the nodes of the binary tree.
 *
 * \param self Binary search tree.
 */
void
lialg_bst_clear (LIAlgBst* self)
{
	private_free (self, self->root);
	self->size = 0;
	self->max_size = 0;
	self->root = NULL;
}

/**
 * \brief Finds a node from the binary tree.
 *
 * \param self Binary search tree.
 * \param data Searched data.
 * \return Node or NULL.
 */
LIAlgBstNode*
lialg_bst_find (LIAlgBst*   self,
                const void* data)
{
	int cmp;
	LIAlgBstNode* node;

	/* Normal binary tree search. */
	node = self->root;
	while (node != NULL)
	{
		cmp = self->cmp (data, node->data);
		if (cmp == 0)
			return node;
		if (cmp < 0)
			node = node->left;
		else
			node = node->right;
	}

	return NULL;
}

/**
 * \brief Finds a node by an index by iterating the elements in order.
 *
 * This is a slow operation that doesn't benefit from the tree structure at
 * all. It's provided nevertheless for your convenience; just keep the caveat
 * in mind.
 *
 * \param self Binary search tree.
 * \param index Index of the searched data.
 * \return Node or NULL.
 */
LIAlgBstNode*
lialg_bst_find_by_index (LIAlgBst* self,
                         int       index)
{
	return private_index (self->root, &index);
}

/**
 * \brief Inserts an existing, unlinked node to the binary tree.
 *
 * The advantage of this function is that it allocates no memory, and thus
 * cannot fail. This make it useful for cases when you need to ensure that
 * all the insertions succeed since you can do all the allocations beforehand
 * instead of trying to undo the changes when an error occurs.
 *
 * \param self Binary search tree.
 * \param node Node to link to the tree.
 * \return Nonzero on success.
 */
void
lialg_bst_link (LIAlgBst*     self,
                LIAlgBstNode* node)
{
	int i;
	int size;
	int depth;
	LIAlgBstNode* tmp;
	LIAlgBstNode* ptr;
	LIAlgBstNode* parent;

	node->left = NULL;
	node->right = NULL;

	/* Root insertion. */
	if (self->root == NULL)
	{
		self->root = node;
		node->parent = NULL;
		self->size = 1;
		self->max_size = 1;
		return;
	}

	/* Normal insertion. */
	parent = self->root;
	for (depth = 1 ; ; depth++)
	{
		if (self->cmp (node->data, parent->data) <= 0)
		{
			if (parent->left == NULL)
			{
				parent->left = node;
				node->parent = parent;
				break;
			}
			else
				parent = parent->left;
		}
		else
		{
			if (parent->right == NULL)
			{
				parent->right = node;
				node->parent = parent;
				break;
			}
			else
				parent = parent->right;
		}
	}

	/* Update the size of the tree. */
	self->size++;
	if (self->size > self->max_size)
		self->max_size = self->size;

	/* Check if rebalancing is needed. */
	if (depth - 1 <= private_halpha (self->size))
		return;
	ptr = node;

	/* Find and rebalance a scapegoat. */
	for (size = i = 1 ; i < depth ; i++)
	{
		/* Calculate the size. */
		if (ptr == parent->left)
			size += private_size (parent->right) + 1;
		else
			size += private_size (parent->left) + 1;
		ptr = parent;
		parent = parent->parent;

		/* Find the scapegoat. */
		if (i <= private_halpha (size))
			continue;

		/* Rebalance the scapegoat. */
		tmp = private_rebalance (ptr, size);
		if (parent != NULL)
		{
			if (ptr == parent->left)
				parent->left = tmp;
			else
				parent->right = tmp;
			tmp->parent = parent;
		}
		else
		{
			self->root = tmp;
			tmp->parent = NULL;
		}
		return;
	}

	//lisys_assert (0);
}

/**
 * \brief Inserts a new node to the binary tree.
 *
 * \param self Binary search tree.
 * \param data Data to store to the inserted node.
 * \return Nonzero on success.
 */
LIAlgBstNode*
lialg_bst_insert (LIAlgBst* self,
                  void*     data)
{
	LIAlgBstNode* node;

	/* Create a new node. */
	node = self->malloc (sizeof (LIAlgBstNode));
	if (node == NULL)
		return NULL;
	node->data = data;

	/* Link to the tree. */
	lialg_bst_link (self, node);
	return node;
}

/**
 * \brief Removes and frees a node from the binary tree.
 *
 * Unlinks the node from the tree, then deleting it.
 * Any user data stored to the node is not freed.
 *
 * \param self Binary search tree.
 * \param node Node to remove.
 */
void
lialg_bst_remove (LIAlgBst*     self,
                  LIAlgBstNode* node)
{
	lialg_bst_unlink (self, node);
	self->free (node);
}

/**
 * \brief Unlinks the node from the binary tree.
 *
 * Unlinks the node from the tree without freeing it.
 * The node can be reused by any tree with #lialg_bst_link.
 *
 * \param self A binary search tree.
 * \param node The node to unlink.
 */
void
lialg_bst_unlink (LIAlgBst*     self,
                  LIAlgBstNode* node)
{
	LIAlgBstNode* tmp;

	/* Normal binary search tree deletion. */
	if (node->left == NULL)
	{
		/* Replace with the child on the right. */
		tmp = node->right;
		if (node->parent != NULL)
		{
			if (node->parent->left == node)
				node->parent->left = tmp;
			else
				node->parent->right = tmp;
		}
		else
			self->root = tmp;
		if (tmp != NULL)
			tmp->parent = node->parent;
	}
	else if (node->right == NULL)
	{
		/* Replace with the child on the left. */
		tmp = node->left;
		if (node->parent != NULL)
		{
			if (node->parent->left == node)
				node->parent->left = tmp;
			else
				node->parent->right = tmp;
		}
		else
			self->root = tmp;
		tmp->parent = node->parent;
	}
	else
	{
		/* Replace with the in order successor. */
		/* We could use data swapping if we didn't want
		   to preserve the roles of undeleted nodes. */
		if (node->right->left != NULL)
		{
			/*   N
			 *  / \           T
			 * Nl  Nr        / \
			 *    / \       Nl  Nr    Nr and Tp may be the same node
			 *   Tp    ==>     / \    but it doesn't matter since their
			 *  / \           Tp      right children remain unchanged.
			 * T             / \
			 *  \           Tr
			 *   Tr
			 */
			tmp = node->right;
			while (tmp->left != NULL)
				tmp = tmp->left;
			tmp->parent->left = tmp->right;
			if (tmp->right != NULL)
				tmp->right->parent = tmp->parent;
			tmp->left = node->left;
			tmp->left->parent = tmp;
			tmp->right = node->right;
			tmp->right->parent = tmp;
		}
		else
		{
			/*   N
			 *  / \            T
			 * Nl  T    ==>   / \
			 *      \        Nl Tr
			 *      Tr
			 */
			tmp = node->right;
			tmp->left = node->left;
			tmp->left->parent = tmp;
		}

		/* Np        Np
		 *  \   ==>   \
		 *   N         T
		 */
		tmp->parent = node->parent;
		if (node->parent != NULL)
		{
			if (node->parent->left == node)
				node->parent->left = tmp;
			else
				node->parent->right = tmp;
		}
		else
			self->root = tmp;
	}

	/* Check if rebalancing is needed. */
	if (--self->size > (self->max_size >> 1))
		return;

	/* Rebalance the whole tree. */
	self->root = private_rebalance (self->root, self->size);
	if (self->root != NULL)
		self->root->parent = NULL;
	self->max_size = self->size;
}

/**
 * \brief Calls the provided function for each node until it returns zero.
 *
 * The values are fed to the function in sorted order. The first value is the
 * one found to be the smallest by the comparison function of the tree.
 *
 * \param self Binary search tree.
 * \param func Function to call.
 */
void
lialg_bst_foreach (LIAlgBst*       self,
                   LIAlgBstForeach func)
{
	/* Recursive inorder traversal. */
	private_each (self->root, func);
}

/**
 * \brief Matches nodes using a user provided function.
 *
 * The passed function is called with the first argument being the user data
 * passed and the second one being the data of the tested node. If the return
 * value was less than zero, greater than zero, or zero, respectively, the next
 * tested node will be to the right, to the left, or a match was been found.
 *
 * \param self Binary search tree.
 * \param func Function to do the matching.
 * \param data Data passed to the function.
 * \return Matching node or NULL if there was no match.
 */
LIAlgBstNode*
lialg_bst_match (LIAlgBst*     self,
                 LIAlgBstMatch func,
                 const void*   data)
{
	int ret;
	LIAlgBstNode* node;

	node = self->root;
	while (node != NULL)
	{
		ret = func (data, node->data);
		if (ret < 0)
			node = node->left;
		else if (ret > 0)
			node = node->right;
		else
			return node;
	}

	return NULL;
}

/****************************************************************************/

static inline int
private_halpha (int n)
{
	/* TODO: This could probably be optimized. */
	return (int) floor (log (n) / log (1 / LIALG_SCAPEGOAT_ALPHA));
}

static inline void
private_each (LIAlgBstNode*   node,
              LIAlgBstForeach func)
{
	LIAlgBstNode* tmp;

	if (node != NULL)
	{
		tmp = node->right;
		private_each (node->left, func);
		func (node->data);
		private_each (tmp, func);
	}
}

static inline void
private_free (LIAlgBst*     self,
              LIAlgBstNode* node)
{
	if (node == NULL)
		return;
	private_free (self, node->left);
	private_free (self, node->right);
	self->free (node);
}

static inline LIAlgBstNode*
private_index (LIAlgBstNode* node,
               int*          counter)
{
	void* tmp;

	if (node == NULL)
		return NULL;

	/* Rewind to the beginning. */
	tmp = private_index (node->left, counter);
	if (tmp != NULL)
		return tmp;

	/* Decrement the counter. */
	if (*counter == 0)
		return node;
	(*counter)--;

	/* Iterate towards the end. */
	return private_index (node->right, counter);
}

static inline int
private_size (LIAlgBstNode* node)
{
	if (node != NULL)
		return private_size (node->left) + private_size (node->right) + 1;
	return 0;
}

static inline LIAlgBstNode*
private_rebalance (LIAlgBstNode* root,
                   int           size)
{
	LIAlgBstNode tmp;

	tmp.right = root;
	private_makelist (&tmp);
	private_maketree (&tmp, size);
	return tmp.right;
}

static inline void
private_makelist (LIAlgBstNode* root)
{
	LIAlgBstNode* tmp;
	LIAlgBstNode* prev;
	LIAlgBstNode* curr;

	/* Tree to ordered list. */
	prev = root;
	curr = root->right;
	while (curr != NULL)
	{
		if (curr->left != NULL)
		{
			/* Rotate right. */
			tmp = curr->left;
			curr->left = tmp->right;
			tmp->right = curr;
			prev->right = tmp;
			curr = tmp;
		}
		else
		{
			/* Move right. */
			prev = curr;
			curr = curr->right;
		}
	}
}

static inline void
private_maketree (LIAlgBstNode* root,
                  int           size)
{
	int count;
	LIAlgBstNode* node;

	/* Ordered list to tree. */
	count = 1;
	while (count <= size)
		count = (count << 1) | 1;
	count >>= 1;
	private_compress (root, size - count);
	while (count > 1)
		private_compress (root, count >>= 1);
	for (node = root ; node->right != NULL ; node = node->right)
		node->right->parent = node;
}

static inline void
private_compress (LIAlgBstNode* root,
                  int           count)
{
	LIAlgBstNode* child;
	LIAlgBstNode* pivot;
	LIAlgBstNode* scanner;

	scanner = root;
	while (count--)
	{
		child = scanner->right;
		pivot = child->right;
		scanner->right = pivot;
		pivot->parent = scanner;
		child->right = pivot->left;
		if (child->right != NULL)
			child->right->parent = child;
		pivot->left = child;
		child->parent = pivot;
		scanner = pivot;
	}
}

/** @} */
/** @} */
