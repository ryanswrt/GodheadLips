Dialogspec{name="fengzhi", unique = true,
	--Every thing the player says is a choice. It can branch conversations
	--Notice, the Choice isn't closed immediately, like {choice,blah} but continues with a comma
	--This means, everything until the choice is closed with a }, is one dialog branch
	--Theoretically, you can also use "say",playername but that is worse design
	{"choice", "Oh. Hello, sir! I didn't expect to find anyone out here."},
	  --Say is used to make NPC's talk. Notice, you can use anything for the name
	  --So you can have multi NPC dialog
	  {"say", "Fengzhi", "Oh I see. Straight into it are we? Well, I don't believe you've introduced yourself."},
	  {"choice", "I'm <playername>"},
	  {"say", "Fengzhi", "Greetings, wanderer. Might I assume you're lost?"},
	  {"choice", "Well, um.. stranded I guess. Not entirely sure how I arrived. Everything's ...hazy."},
	  {"say", "Fengzhi", "You guess? What do you know, then?"},
	  {"choice", "You're a philosopher?"},
	  {"say", "Fengzhi", "Do you know that I am?"},
	  {"choice", "..Yes, quite."},
	  {"say", "Fengzhi", "So here is what I know. You are what you know, moved by what you don't know. Once this has been realized there can be no doubt as to identity or direction, and we can never be lost. We are always what we know that we are, and we know that our destination is unknown. You are not lost."},
	  --Now here we branch with 2 choices
	    {"choice", "Are you sure? I think you lost me.",
	      {"say", "Fengzhi", "The moment you know where you are headed that destination becomes part of your identity. You are playername heading somewhere."},
	      {"choice","Then, I'm <blank> headed for... for.. Hyalang was it?"}
	      --Choice tree runs out, and continues to the parent branch
	    },	    
	    {"choice", "Ah. So... <thoughtful gesture>",
	      {"say", "Fengzhi", "Where are you headed, then, stranger?"},
	      {"choice","I was... trying to reach... Hyalang..."}
	      --Choice tree runs out, and continues to the parent branch
	      },
	    {"say", "Fengzhi", "Pleased to meet you, <blank>, traveling to Hyalang. What is your purpose?"},
	      {"choice", "Ow, my head... Yes, I just wanted to get away from the violence at home.",
		{"say", "Fengzhi","Have you succeeded?"},
		{"choice", "It doesn't seem like it. But is there fighting here too?",
		  {"say", "Fengzhi","The world is constantly at war with itself and Death's triumph over life is the ultimate key to life itself."},
		  {"say", "Fengzhi","Without death, there could never be life. I know this.."},
		}		
	      },
	      {"choice", "What is yours?",
		{"say", "Fengzhi","My purpose is to defy purpose, and find meaning in that which never meant anything."},
		{"choice", "Odd, then, you would seem to assume *I* am concerned with a purpose.",
		  {"say", "Fengzhi"," You told me you were lost, implying a desire for direction. We have sorted that out. You are not lost, and this conversation serves no purpose. Be on your way."},
		  --The same as /end in the dialog
		  {"exit"}},
		{"choice","How is that going?",
		  {"say", "Fengzhi"," Not well, I'm afraid. I'm rather lost, you see."},
		  {"choice"," Option 2.2.1: Does that mean you doubt your identity or your direction?",
		    {"say", "Fengzhi"," There is no doubt. I know that I am unaware. I am void, a vessel for knowledge that is yet to be revealed. As such, my identity is my direction."},
		    {"choice"," Option 2.2.1.1: Then how are you lost? Do you not know the way to any town or village here?",
		      {"say", "Fengzhi"," Follow the coast to the right and you will be in Port Hyalang. I am not lost in space, stranger, but in essence."},
		      {"exit"}
		    },
		    {"choice"," Option 2.2.1.2: Did you chose to come to these old ruins to become unlost, or are you even concerned with being something other than lost?",
		      {"say", "Fengzhi"," I chose to be here, right now. I am a vessel for knowledge that is yet to be revealed. I am the memory of countless deaths before me."},
		      {"say", "Fengzhi","I am alive here, right now."},{"exit"}
		    }		    
		  },
		  {"choice"," Option 2.2.2: Great. Well, that makes two of us. Do you know what place this is?",
		    {"say", "Fengzhi"," This is the ruined port of Hyalang. Great men lived here once. Their deaths left this place empty, ready to be seeded with new life. Death will claim them, too, and life's triumph will be absolute."},
		    {"exit"}
		  }
		},
		{"choice"," Option 2.3: You'd like my uncle, then. :)",
		  {"say", "Fengzhi"," I salute his life and celebrate his inevitable death."},
		  --This sets a flag, that can be checked in dialogs in order to see if topics are known
		  {"flag","Celebrate Death"},
		  {"choice"," Option 2.3.1: Why do you celebrate death?",
		    {"say", "Fengzhi"," If there was no death, there could be no life. Death's inevitability is the very force behind life. Death is the Creator. We must celebrate it."},
		    {"exit"}
		  },
		  {"choice"," Option 2.3.2: Are we having a party?!",
		  {"say", "Fengzhi"," Seems to me the party is already over. You don't look so good."},
		  {"exit"}
		}
	      }
	      }
	  }
