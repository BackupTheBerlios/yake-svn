INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
SUBDIRS = event
lib_LTLIBRARIES = libyappBase.la
libyappBase_la_SOURCES = yapp.cpp yappPCH.cpp
libyappBase_la_LIBADD = $(top_builddir)/src/yapp/base/event/libyappEvents.la\
	$(top_builddir)/src/yake/base/libyakeBase.la
AM_CXXFLAGS = -rdynamic
