INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/luabind \
	-I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO



lib_LTLIBRARIES = libyakescriptingBindingsLua.la
libyakescriptingBindingsLua_la_SOURCES = pch.cpp yakeBinder.cpp
libyakescriptingBindingsLua_la_LIBADD = \
	$(top_builddir)/src/yake/base/libyakeBase.la -lluabind -llualib -llua
