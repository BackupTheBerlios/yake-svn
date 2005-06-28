INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/OGRE \
	-I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
AM_CXXFLAGS = -DEXT_HASH -DGCC_3_1
lib_LTLIBRARIES = libyakeinputOgre.la
libyakeinputOgre_la_SOURCES = InputSystemOgre.cpp yakePCH.cpp
libyakeinputOgre_la_LIBADD = $(top_builddir)/src/yake/base/libyakeBase.la\
	-lOgreMain
