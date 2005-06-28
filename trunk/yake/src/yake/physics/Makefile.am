INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES =  libyakePhysics.la

libyakePhysics_la_SOURCES = yakePCH.cpp yakePhysicsAffectors.cpp \
	yakePhysicsSystem.cpp

libyakePhysics_la_LIBADD = $(top_builddir)/src/yake/base/libyakeBase.la
