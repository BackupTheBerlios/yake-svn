INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies \
	-I$(top_srcdir)/dependencies/ttl -I$(top_srcdir)/dependencies/luabind $(all_includes)
METASOURCES = AUTO






lib_LTLIBRARIES = libyakescriptingLua.la
libyakescriptingLua_la_SOURCES = ScriptingSystemLua.cpp yakePCH.cpp
libyakescriptingLua_la_LIBADD = $(top_builddir)/src/yake/base/libyakeBase.la \
	-lluabind -llualib -llua
