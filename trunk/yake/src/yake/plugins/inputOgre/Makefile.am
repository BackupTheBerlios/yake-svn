INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl \
	-I/usr/local/include/OGRE $(all_includes)
METASOURCES = AUTO
AM_CXXFLAGS = -DGCC_3_1 -DEXT_HASH
lib_LTLIBRARIES = libyakeinputOgre.la
libyakeinputOgre_la_SOURCES = inputPlugin.cpp InputSystemOgre.cpp yakePCH.cpp
