INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/luabind \
	-I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES = libyappmodelLuaBindings.la
libyappmodelLuaBindings_la_SOURCES = yakeLuaBinder.cpp
libyappmodelLuaBindings_la_LIBADD =\
	$(top_builddir)/src/yake/scripting/libyakeScripting.la\
	$(top_builddir)/src/yake/plugins/scriptingLua/libyakescriptingLua.la\
	-lluabind
