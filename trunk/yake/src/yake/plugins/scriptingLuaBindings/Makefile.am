INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/luabind -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO




lib_LTLIBRARIES = libyakeScriptingBindingsLua.la
libyakeScriptingBindingsLua_la_LIBADD = $(top_builddir)/src/yake/base/libyakeBase.la -lluabind -llualib -llua
libyakeScriptingBindingsLua_la_SOURCES =  pch.cpp plugin.cpp yakeBinder.cpp

