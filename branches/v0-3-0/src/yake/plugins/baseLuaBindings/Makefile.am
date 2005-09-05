INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies \
	-I$(top_srcdir)/dependencies/ttl -I$(top_srcdir)/dependencies/luabind $(all_includes)

lib_LTLIBRARIES = libyakebaseLuaBindings.la
libyakebaseLuaBindings_la_SOURCES = yakeLuaBinder.cpp
libyakebaseLuaBindings_la_LIBADD = \
	$(top_builddir)/src/yake/scripting/libyakeScripting.la $(top_builddir)/src/yake/plugins/scriptingLua/libyakescriptingLua.la \
	-lluabind
