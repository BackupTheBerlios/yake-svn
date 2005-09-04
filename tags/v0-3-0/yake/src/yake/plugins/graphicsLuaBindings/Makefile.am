INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies \
	-I$(top_srcdir)/dependencies/luabind -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES = libyakegraphicsBindingsLua.la
libyakegraphicsBindingsLua_la_LIBADD =\
	$(top_builddir)/src/yake/scripting/libyakeScripting.la
libyakegraphicsBindingsLua_la_SOURCES = yakeLuaBinder.cpp
