INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies -I$(top_srcdir)/dependencies/ode/include -I$(top_srcdir)/dependencies/ttl $(all_includes)
METASOURCES = AUTO



lib_LTLIBRARIES = libyakePhysicsSystemODE.la
libyakePhysicsSystemODE_la_SOURCES =  OdeBallJoint.cpp OdeBody.cpp OdeCollisionGeometry.cpp OdeComplexObject.cpp OdeFixedJoint.cpp OdeHinge2Joint.cpp OdeHingeJoint.cpp OdeJointBase.cpp OdeWorld.cpp PhysicsSystemODE.cpp plugin.cpp yakePCH.cpp
libyakePhysicsSystemODE_la_LIBADD = $(top_builddir)/src/yake/physics/libyakePhysics.la -lode
