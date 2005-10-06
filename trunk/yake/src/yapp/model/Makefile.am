INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies \
	-I$(top_srcdir)/dependencies/ttl $(all_includes)

lib_LTLIBRARIES = libyappModel.la
libyappModel_la_SOURCES = yakeComplexModel.cpp yakeGraphical.cpp \
	yakePhysical.cpp yakeActor.cpp yakeModelLink.cpp yakeModelMovableLink.cpp
libyappModel_la_LIBADD = $(top_builddir)/src/yapp/loader/libyappLoader.la
