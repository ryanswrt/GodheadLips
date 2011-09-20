import os
import string
import Options
import Utils

APPNAME='godheadlips'
VERSION='0.0.1'

top = '.'
out = '.build'

CORE_DIRS = 'ai algorithm archive binding callback client engine extension font generator image main math model network particle paths physics render render/render21 render/render32 script server sound string system thread video voxel widget'
EXTS_DIRS = 'ai animation camera config-file database file graphics heightmap lobby model-editing network noise object-physics object-render password physics reload render skeleton sound speech thread tiles tiles-physics tiles-render vision watchdog widgets'

def options(ctx):
	ctx.tool_options('compiler_cc')
	ctx.tool_options('compiler_cxx')
	ctx.add_option('--relpath', action='store', default=True, help='relative data and library paths [default: true]')
	ctx.add_option('--bindir', action='store', default=None, help='override executable directory [default: PREFIX/bin]')
	ctx.add_option('--libdir', action='store', default=None, help='override library directory [default: PREFIX/lib]')
	ctx.add_option('--datadir', action='store', default=None, help='override data directory [default: PREFIX/share]')
	ctx.add_option('--sound', action='store', default=True, help='compile with sound support [default: true]')
	ctx.add_option('--optimize', action='store', default=False, help='compile with heavy optimizations [default: false]')
	ctx.add_option('--luajit', action='store', default=True, help='compile with LuaJIT if possible [default: true]')
	ctx.add_option('--memdebug', action='store', default=False, help='compile with expensive memory debugging [default: false]')

def configure(ctx):

	# Options
	if Options.options.relpath != "false" and (Options.options.bindir or Options.options.libdir or Options.options.datadir):
		print("\nThe directory overrides are not used by a relative path build")
		print("To enable a traditional build, configure with --relpath=false\n")
		exit(1)
	ctx.env.RELPATH = Options.options.relpath != "false"
	ctx.env.SOUND = Options.options.sound != "false"
	ctx.env.MEMDEBUG = Options.options.memdebug == "true"
	if Options.options.optimize == "true":
		optimize = '-O3'
	else:
		optimize = '-O0'

	# Flags
	ctx.check_tool('compiler_cc')
	ctx.check_tool('compiler_cxx')
	ctx.env.INCLUDES_CORE = ['.', 'src']
	ctx.env.INCLUDES_EXTENSION = ['.', 'src']
	ctx.env.INCLUDES_TEST = []
	cflags = ctx.env.CFLAGS
	cflags.extend(['-g', '-Wall', optimize, '-DHAVE_CONFIG_H'])
	ldflags = ctx.env.LINKFLAGS
	ldflags.extend(['-g'])
	ctx.env.CFLAGS_CORE = cflags
	ctx.env.CFLAGS_EXTENSION = cflags
	ctx.env.CFLAGS_TEST = cflags
	ctx.env.CXXFLAGS_CORE = cflags
	ctx.env.CXXFLAGS_EXTENSION = cflags
	ctx.env.CXXFLAGS_TEST = cflags
	ctx.env.LIBPATH_CORE = []
	ctx.env.LIBPATH_EXTENSION = []
	ctx.env.LIBPATH_TEST = []
	ctx.env.LINKFLAGS_CORE = ldflags
	ctx.env.LINKFLAGS_EXTENSION = ldflags
	ctx.env.LINKFLAGS_TEST = ldflags

	# Base dependencies
	ctx.check(header_name='dlfcn.h', define_name='HAVE_DLFCN_H', mandatory=False)
	ctx.check(header_name='fcntl.h', define_name='HAVE_FCNTL_H', mandatory=False)
	ctx.check(header_name='inotifytools/inotify.h', define_name='HAVE_INOTIFYTOOLS_INOTIFY_H', mandatory=False)
	ctx.check(header_name='inttypes.h', define_name='HAVE_INTTYPES_H', mandatory=False)
	ctx.check(header_name='poll.h', define_name='HAVE_POLL_H', mandatory=False)
	ctx.check(header_name='stdint.h', define_name='HAVE_STDINT_H', mandatory=False)
	ctx.check(header_name='sys/inotify.h', define_name='HAVE_SYS_INOTIFY_H', mandatory=False)
	ctx.check(header_name='sys/mman.h', define_name='HAVE_SYS_MMAN_H', mandatory=False)
	ctx.check(header_name='sys/stat.h', define_name='HAVE_SYS_STAT_H')
	ctx.check(header_name='sys/time.h', define_name='HAVE_SYS_TIME_H')
	ctx.check(header_name='sys/wait.h', define_name='HAVE_SYS_WAIT_H', mandatory=False)
	ctx.check(header_name='time.h', define_name='HAVE_TIME_H')
	ctx.check(header_name='unistd.h', define_name='HAVE_UNISTD_H')
	ctx.check(header_name='windows.h', define_name='HAVE_WINDOWS_H', mandatory=False)
	ctx.check(header_name='iconv.h', define_name='HAVE_ICONV_H')
	ctx.check_cc(msg='Checking for function fork', header_name='unistd.h', function_name='fork', define_name='HAVE_FORK', mandatory=False)
	ctx.check_cc(msg='Checking for function usleep', header_name='unistd.h', function_name='usleep', define_name='HAVE_USLEEP', mandatory=False)
	ctx.check_cc(lib='dl', uselib_store='CORE', mandatory=False)
	ctx.check_cc(lib='m', uselib_store='CORE', mandatory=False)
	ctx.check_cc(lib='pthread', uselib_store='THREAD', mandatory=False)
	ctx.check_cc(lib='iconv', uselib_store='CORE', mandatory=False) # BSD

	# zlib
	if not ctx.check_cc(lib='z', mandatory=False, uselib_store='ZLIB'):
		ctx.check_cc(lib='zdll', mandatory=True, uselib_store='ZLIB')
	ctx.check_cc(header_name='zlib.h', mandatory=True, uselib_store='ZLIB')

	# SQLite
	if not ctx.check_cfg(package='sqlite3', atleast_version='3.6.0', args='--cflags --libs', uselib_store='SQLITE', mandatory=False):
		ctx.check_cc(header_name='sqlite3.h', mandatory=True, uselib='CORE TEST', uselib_store='SQLITE')
		ctx.check_cc(lib='sqlite3', mandatory=True, uselib='CORE TEST', uselib_store='SQLITE')

	# Bullet
	if not ctx.check_cfg(package='bullet', atleast_version='2.77', args='--cflags --libs', mandatory=False):
		if ctx.check_cxx(lib='linearmath', uselib='CORE TEST', mandatory=False):
			ctx.env.LINKFLAGS_BULLET = ['-llinearmath', '-lbulletcollision', '-lbulletdynamics']
		elif ctx.check_cxx(lib='LinearMath', uselib='CORE TEST', mandatory=True):
			ctx.env.LINKFLAGS_BULLET = ['-lLinearMath', '-lBulletCollision', '-lBulletDynamics']
		ctx.check_cxx(header_name='btBulletCollisionCommon.h', mandatory=True, uselib='CORE TEST BULLET', uselib_store='BULLET')

	# ENet
	if not ctx.check_cfg(package='enet', atleast_version='1.2.0', args='--cflags --libs', mandatory=False) and \
	   not ctx.check_cfg(package='libenet', atleast_version='1.2.0', args='--cflags --libs', uselib_store='ENET', mandatory=False):
		ctx.check_cc(header_name='enet/enet.h', mandatory=True, uselib='CORE TEST', uselib_store='ENET')
		ctx.check_cc(lib='enet', mandatory=True, uselib='CORE TEST', uselib_store='ENET')

	# Lua/LuaJIT
	# Ubuntu Lucid ships a broken 32-bit LuaJIT on 64-bit systems so we need some extra
	# checks to know if it really works or if we should fall back to standard Lua.
	if Options.options.luajit == "true" and\
	   ctx.check_cfg(package='luajit', atleast_version='2.0.0', args='--cflags --libs', uselib_store='LUAJIT', mandatory=False) and\
	   ctx.check(msg="Checking for luajit architecture", fragment='int main() { return 0; }\n', mandatory=False, uselib='CORE TEST LUAJIT'):
		ctx.check_cfg(package='luajit', args='--cflags --libs', uselib_store='LUA', mandatory=False)
	elif not ctx.check_cfg(package='lua5.1', atleast_version='5.1', args='--cflags --libs', uselib_store='LUA', mandatory=False):
		if not ctx.check_cfg(package='lua', atleast_version='5.1', args='--cflags --libs', mandatory=False):
			ctx.check_cc(header_name='lua.h', mandatory=True, uselib='CORE TEST', uselib_store='LUA')
			ctx.check_cc(header_name='lauxlib.h', mandatory=True, uselib='CORE TEST', uselib_store='LUA')
			if not ctx.check_cc(lib='lua5.1', uselib='CORE TEST', uselib_store='LUA', mandatory=False):
				ctx.check_cc(lib='lua', mandatory=True, uselib='CORE TEST', uselib_store='LUA')

	# SDL
	if not ctx.check_cfg(package='sdl', atleast_version='1.2.0', args='--cflags --libs', mandatory=False):
		ctx.check_cxx(header_name='SDL.h', mandatory=True, uselib='CORE TEST', uselib_store='SDL')
		ctx.check_cxx(lib='SDL', mandatory=True, uselib='CORE TEST', uselib_store='SDL')

	# SDL_ttf
	ctx.check_cc(lib='SDL_ttf', mandatory=True, uselib='CORE TEST SDL', uselib_store='SDL_TTF')
	ctx.check_cc(msg = "Checking for header SDL_ttf.h", mandatory=True, uselib='CORE TEST SDL', fragment="#include <SDL_ttf.h>\nint main(int argc, char** argv) { return 0; }\n")

	# GLEW
	if not ctx.check_cfg(package='glew', atleast_version='1.5.5', args='--cflags --libs', mandatory=False):
		ctx.check_cc(header_name='GL/glew.h', mandatory=True, uselib='CORE TEST', uselib_store='GLEW')
		if not ctx.check_cc(lib='GLEW', mandatory=False, uselib='CORE TEST', uselib_store='GLEW'):
			ctx.check_cc(lib='GLEW32', mandatory=True, uselib='CORE TEST', uselib_store='GLEW')

	# GL
	if not ctx.check_cc(lib='GL', mandatory=False, uselib='CORE TEST', uselib_store='GLEW'):
		ctx.check_cc(lib='OpenGL32', mandatory=True, uselib='CORE TEST', uselib_store='GLEW')

	if ctx.env.SOUND:
		# AL
		if not ctx.check_cfg(package='openal', atleast_version='0.0.8', args='--cflags --libs', uselib_store="AL", mandatory=False):
			ctx.check_cc(header_name='AL/al.h', mandatory=True, uselib='CORE TEST', uselib_store='AL')
			if not ctx.check_cc(lib='openal', mandatory=False, uselib='CORE TEST', uselib_store='AL'):
				ctx.check_cc(lib='OpenAL32', mandatory=True, uselib='CORE TEST', uselib_store='AL')
		# OGG
		if not ctx.check_cfg(package='ogg', atleast_version='1.1.0', args='--cflags --libs', mandatory=False):
			ctx.check_cc(header_name='ogg.h', mandatory=True, uselib='CORE TEST', uselib_store='OGG')
			ctx.check_cc(lib='ogg', mandatory=True, uselib='CORE TEST', uselib_store='OGG')
		# VORBIS
		if not ctx.check_cfg(package='vorbisfile', atleast_version='1.2.0', args='--cflags --libs', uselib_store='VORBIS', mandatory=False):
			ctx.check_cc(header_name='vorbisfile.h', mandatory=True, uselib='CORE TEST', uselib_store='VORBIS')
			ctx.check_cc(lib='vorbisfile', mandatory=True, uselib='CORE TEST', uselib_store='VORBIS')
		# FLAC
		if not ctx.check_cfg(package='flac', atleast_version='1.2.0', args='--cflags --libs', mandatory=False):
			ctx.check_cc(header_name='stream_decoder.h', mandatory=True, uselib='CORE TEST', uselib_store='FLAC')
			ctx.check_cc(lib='FLAC', mandatory=True, uselib='CORE TEST', uselib_store='FLAC')

	# CURL
	if not ctx.check_cfg(package='libcurl', args='--cflags --libs', uselib_store="CURL", mandatory=False):
		if not ctx.check_cfg(package='curl', args='--cflags --libs', uselib_store="CURL", mandatory=False):
			found_curl = False
			if ctx.check_cc(lib='curl', mandatory=False, uselib='CORE TEST', uselib_store='CURL'):
				if ctx.check_cc(header_name='curl/curl.h', lib='curl', mandatory=False, uselib='CORE TEST', uselib_store='CURL'):
					found_curl = True
			if found_curl:
				ctx.env.CURL = True
				ctx.define('HAVE_CURL', 1)
	else:
		ctx.env.CURL = True

	# Paths and defines
	ctx.define('LI_ENABLE_ERROR', 1)
	if ctx.env.MEMDEBUG:
		ctx.define('LI_ENABLE_MEMDEBUG', 1)
	if not ctx.env.SOUND:
		ctx.define('LI_DISABLE_SOUND', 1)
	if ctx.env.RELPATH:
		ctx.define('LI_RELATIVE_PATHS', 1)
		ctx.env.RPATH_CORE = ['$ORIGIN/lib']
		ctx.env.PREFIX = ctx.path.abspath()
		ctx.env.BINDIR = ctx.env.PREFIX
		ctx.env.EXTSDIR = os.path.join(ctx.env.PREFIX, 'lib', 'extensions')
		ctx.env.DATADIR = os.path.join(ctx.env.PREFIX, 'data')
		ctx.env.PROGDIR = os.path.join(ctx.env.PREFIX, 'bin')
		ctx.env.TOOLDIR = os.path.join(ctx.env.PREFIX, 'tool')
	else:
		bindir = Options.options.bindir
		if not bindir:
			bindir = os.path.join(ctx.env.PREFIX, 'bin')
		libdir = Options.options.libdir
		if not libdir:
			libdir = os.path.join(ctx.env.PREFIX, 'lib')
		datadir = Options.options.datadir
		if not datadir:
			datadir = os.path.join(ctx.env.PREFIX, 'share')
		ctx.env.BINDIR = bindir
		ctx.env.EXTSDIR = os.path.join(libdir, APPNAME, 'extensions', '0.1')
		ctx.env.DATADIR = os.path.join(datadir, APPNAME)
		ctx.env.PROGDIR = bindir
		ctx.env.TOOLDIR = os.path.join(datadir, APPNAME, 'tool')
		ctx.env.APPDIR = os.path.join(datadir, 'applications')
		ctx.env.ICONDIR = os.path.join(datadir, 'pixmaps')
		ctx.define('LIEXTSDIR', ctx.env.EXTSDIR)
		ctx.define('LIDATADIR', os.path.join(datadir, APPNAME))
		ctx.define('LIPROGDIR', ctx.env.PROGDIR)
		ctx.define('LITOOLDIR', ctx.env.TOOLDIR)
	ctx.write_config_header('config.h')

	# Messages
	if ctx.env.RELPATH:
		ctx.define('LI_RELATIVE_PATHS', 1)
		print("\nConfigured with:")
		print("\trelative paths")
	else:
		print("\nConfigured with:")
		print("\tbindir: " + bindir)
		print("\tlibdir: " + libdir)
		print("\tdatadir: " + datadir)
	if ctx.env.SOUND:
		print("\tsound support")
	if ctx.env.CURL:
		print("\tmaster server connectivity")
	if ctx.env.MEMDEBUG:
		print("\tmemory debugging")
	print("\nBuild command: ./waf")
	print("Install command: ./waf install\n")

def build(ctx):
	ctx.add_group("build")
	ctx.add_group("install")
	ctx.set_group("build")
	objs = ''
	libs = 'CORE LUA SQLITE BULLET ENET SDL SDL_TTF GLEW THREAD AL VORBIS OGG FLAC CURL ZLIB'

	# Core objects.
	for dir in CORE_DIRS.split(' '):
		srcs = ctx.path.ant_glob('src/lipsofsuna/%s/*.c' % dir)
		if srcs:
			objs += dir + '_objs '
			ctx.new_task_gen(
				features = 'c',
				source = srcs,
				target = dir + '_objs',
				use = libs)
		srcs = ctx.path.ant_glob('src/lipsofsuna/%s/*.cpp' % dir)
		if srcs:
			objs += dir + '_cxx_objs '
			ctx.new_task_gen(
				features = 'cxx',
				source = srcs,
				target = dir + '_cxx_objs',
				use = libs)

	# Extension objects.
	for dir in EXTS_DIRS.split(' '):
		objs += dir + '_ext_objs '
		ctx.new_task_gen(
			features = 'c',
			source = ctx.path.ant_glob('src/lipsofsuna/extension/%s/*.c' % dir),
			target = dir + '_ext_objs',
			use = 'EXTENSION LUA SQLITE ENET SDL SDL_TTF GLEW THREAD AL VORBIS OGG FLAC CURL ZLIB')

	# Target executable.
	ctx.new_task_gen(
		features = 'c cxx cprogram',
		target = 'godheadlips',
		install_path = '${BINDIR}',
		add_objects = objs,
		use = libs)

	# LOD tool.
	ctx.new_task_gen(
		features = 'c',
		source = ctx.path.ant_glob('src/lodtool/*.c'),
		target = 'lodtool_objs',
		use = 'CORE THREAD ZLIB')
	ctx.new_task_gen(
		features = 'c cprogram',
		target = 'lipsofsuna-lodtool',
		install_path = '${BINDIR}',
		add_objects = 'algorithm_objs archive_objs model_objs system_objs lodtool_objs',
		use = 'CORE THREAD ZLIB')

	# Installation. Since relpath uses the same directory layout as the source tree, the executable
	# will find the data files from their current locations and we can avoid installing them.
	ctx.set_group("install")
	if not ctx.env.RELPATH:
		ctx.install_files(ctx.env.TOOLDIR, ['tool/lipsofsuna_export.py'])
		start_dir = ctx.path.find_dir('data')
		ctx.install_files('${DATADIR}', start_dir.ant_glob('**/*.*'), cwd=start_dir, relative_trick=True)
		ctx.install_files('${APPDIR}', ['misc/godheadlips.desktop'])
		ctx.install_files('${ICONDIR}', ['misc/lipsofsuna.png', 'misc/lipsofsuna.svg'])

def dist(ctx):
	import Logs
	import tarfile
	dirs = ['src/**/*.*', 'data/**/*.*', 'tool/*', 'misc/*', 'docs/*', 'AUTHORS', 'COPYING', 'NEWS', 'README', 'waf', 'wscript']
	excl = ['**/.*', '**/import', 'docs/html']
	base = APPNAME + '-' + VERSION
	name = base + '.tar.gz'
	Logs.pprint('GREEN', "Creating `%s'" % name)
	tar = tarfile.open(name, 'w:gz')
	for f in ctx.path.ant_glob(dirs, excl=excl):
		tinfo = tar.gettarinfo(name = f.abspath(), arcname = base + '/' + f.path_from(ctx.path))
		tinfo.uid = 0
		tinfo.gid = 0
		tinfo.uname = 'root'
		tinfo.gname = 'root'
		fu = None
		try:
			fu = open(f.abspath())
			tar.addfile(tinfo, fileobj = fu)
		finally:
			fu.close()
	tar.close()
	Logs.pprint('GREEN', 'Done')
	exit()

def html(ctx):
	import Logs
	import re
	import shutil
	# Initialize directories.
	luadir = os.path.join(ctx.path.abspath(), 'docs', 'html', 'lua')
	shutil.rmtree(luadir, True)
	os.makedirs(luadir)
	# Compile the Doxygen documentation.
	ctx.exec_command('doxygen docs/Doxyfile')
	Logs.pprint('GREEN', "Built Doxygen documentation")
	# Compile the script documentation.
	ctx.exec_command('luadoc -d docs/html/lua data/system data/lipsofsuna/scripts/common')
	Logs.pprint('GREEN', "Built Lua documentation")
