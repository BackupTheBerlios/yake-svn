INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES =  libyakeInput.la
libyakeInput_la_SOURCES = yakeActionMap.cpp yakeInput.cpp \
	yakeInputEventGenerator.cpp yakePCH.cpp yakeInputSystem.cpp

libyakeInput_la_LIBADD = $(top_builddir)/src/yake/base/libyakeBase.la \
	-lboost_signals
