INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES = libyakeInputSystemOgre.la
libyakeInputSystemOgre_la_SOURCES =  inputPlugin.cpp InputSystemOgre.cpp yakePCH.cpp
libyakeInputSystemOgre_la_LIBADD = $(top_builddir)/src/yake/input/libyakeInput.la -lOgreMain
AM_CXXFLAGS = -DGCC_3_1 -DEXT_HASH
