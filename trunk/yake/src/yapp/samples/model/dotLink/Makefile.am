INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies \
	-I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = dotLinkDemo
dotLinkDemo_SOURCES = yakeDotLinkDemo.cpp
dotLinkDemo_LDADD = $(top_builddir)/src/yapp/loader/libyappLoader.la \
	$(top_builddir)/src/yapp/model/libyappModel.la $(top_builddir)/src/yapp/base/libyappBase.la \
	$(top_builddir)/src/yake/physics/libyakePhysics.la $(top_builddir)/src/yake/input/libyakeInput.la \
	$(top_builddir)/src/yake/graphics/libyakeGraphics.la $(top_builddir)/src/yake/data/libyakeData.la \
	$(top_builddir)/src/yake/base/libyakeBase.la $(top_builddir)/src/yake/audio/libyakeAudio.la
