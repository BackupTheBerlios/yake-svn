INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies \
	-I$(top_srcdir)/dependencies/OGRE -I$(top_srcdir)/dependencies/luabind -I$(top_srcdir)/dependencies/ttl \
	-I/usr/include/CEGUI -I/usr/local/include/CEGUI $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = consoleDemo
consoleDemo_SOURCES = yakeConsoleDemo.cpp
consoleDemo_LDADD = \
	$(top_builddir)/src/yapp/plugins/ceguiOgreRendererAdapter/libyakeCEGUIOgreAdapter.la $(top_builddir)/src/yapp/base/libyappBase.la \
	$(top_builddir)/src/yake/scripting/libyakeScripting.la $(top_builddir)/src/yake/physics/libyakePhysics.la \
	$(top_builddir)/src/yake/input/libyakeInput.la $(top_builddir)/src/yake/graphics/libyakeGraphics.la \
	$(top_builddir)/src/yake/data/libyakeData.la $(top_builddir)/src/yake/audio/libyakeAudio.la -llua -llualib -lluabind \
	-lCEGUIOgreRenderer
