INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO



lib_LTLIBRARIES = libyakeNativeLinux.la
libyakeNativeLinux_la_SOURCES =  yakeApplication.cpp yakeCriticalSection.cpp yakeDebug.cpp yakeLibrary.cpp yakeTime.cpp

AM_CXXFLAGS = -rdynamic
libyakeNativeLinux_la_LIBADD = -ldl
