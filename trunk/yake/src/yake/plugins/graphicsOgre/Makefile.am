INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO
AM_CXXFLAGS = -rdynamic -DGCC_3_1 -DEXT_HASH
lib_LTLIBRARIES = libyakeGraphicsSystemOgre.la
libyakeGraphicsSystemOgre_la_SOURCES =  graphicsOgreCamera.cpp graphicsOgreCore.cpp graphicsOgreEntity.cpp graphicsOgreGeometryAccess.cpp graphicsOgreLight.cpp graphicsOgreNode.cpp graphicsOgreSkeleton.cpp graphicsOgreViewport.cpp yakeGraphicsSystem.cpp yakeGraphicsWorld.cpp yakePCH.cpp
libyakeGraphicsSystemOgre_la_LIBADD = $(top_builddir)/src/yake/graphics/libyakeGraphics.la -lOgreMain
