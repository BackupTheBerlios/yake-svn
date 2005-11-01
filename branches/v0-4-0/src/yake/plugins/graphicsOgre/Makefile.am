INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies \
	-I$(top_srcdir)/dependencies/OGRE -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES = libyakegraphicsOgre.la
libyakegraphicsOgre_la_LIBADD = -lOgreMain
AM_CXXFLAGS = -DEXT_HASH -DGCC_3_1
libyakegraphicsOgre_la_SOURCES = graphicsOgreCamera.cpp graphicsOgreCore.cpp\
	graphicsOgreEntity.cpp graphicsOgreGeometryAccess.cpp\
	graphicsOgreLight.cpp graphicsOgreNode.cpp\
	graphicsOgreParticleSystem.cpp graphicsOgreSkeleton.cpp\
	graphicsOgreViewport.cpp yakeGraphicsSystem.cpp yakeGraphicsWorld.cpp\
	yakePCH.cpp
