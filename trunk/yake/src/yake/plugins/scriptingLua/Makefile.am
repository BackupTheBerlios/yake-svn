INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/boost -I$(top_srcdir)/dependencies/luabind -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO






lib_LTLIBRARIES = libyakeScriptingSystemLua.la
libyakeScriptingSystemLua_la_LIBADD = $(top_builddir)/src/yake/base/libyakeBase.la -llualib -llua
libyakeScriptingSystemLua_la_SOURCES =  plugin.cpp ScriptingSystemLua.cpp yakePCH.cpp
