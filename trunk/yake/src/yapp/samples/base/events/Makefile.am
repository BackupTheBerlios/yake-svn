INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = EventsDemo
EventsDemo_SOURCES = yakeDemo.cpp yakePCH.cpp
EventsDemo_LDADD = $(top_builddir)/src/yapp/base/libyappBase.la\
	$(top_builddir)/src/yake/base/libyakeBase.la
