INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/dependencies/ttl $(all_includes)



SUBDIRS = native math templates


AM_CXXFLAGS = -rdynamic

METASOURCES = AUTO
lib_LTLIBRARIES = libyakeBase.la
<<<<<<< Makefile.am
libyakeBase_la_SOURCES = yakeDataChunk.cpp yakeDebugLogAppender.cpp \
		yakeDebugOutputStream.cpp yakeException.cpp yakeInputStream.cpp yakeLock.cpp yakeLog.cpp \
		yakeMimeTypeManager.cpp yakeOutputStream.cpp yakePCH.cpp yakeSerializable.cpp yakeState.cpp \
		yakeString.cpp yakeUniqueId.cpp yakeUniqueName.cpp yakeMovable.cpp \
	yakeCriticalSection.cpp yakeLibrary.cpp
=======
<<<<<<< Makefile.am
libyakeBase_la_SOURCES = yakeDataChunk.cpp yakeDebugLogAppender.cpp \
		yakeDebugOutputStream.cpp yakeException.cpp yakeInputStream.cpp yakeLock.cpp yakeLog.cpp \
		yakeMimeTypeManager.cpp yakeOutputStream.cpp yakePCH.cpp yakeSerializable.cpp yakeState.cpp \
		yakeString.cpp yakeUniqueId.cpp yakeUniqueName.cpp yakeMovable.cpp \
	yakeCriticalSection.cpp yakeLibrary.cpp
=======
libyakeBase_la_SOURCES = yakeCriticalSection.cpp yakeDataChunk.cpp \
	yakeDebugLogAppender.cpp yakeDebugOutputStream.cpp yakeException.cpp yakeInputStream.cpp \
	yakeLibrary.cpp yakeLock.cpp yakeLog.cpp yakeMimeTypeManager.cpp yakeOutputStream.cpp \
	yakePCH.cpp yakeSerializable.cpp yakeState.cpp yakeString.cpp yakeUniqueId.cpp \
	yakeUniqueName.cpp yakeMovable.cpp
>>>>>>> 1.2
>>>>>>> 1.3
libyakeBase_la_LIBADD = $(top_builddir)/src/yake/base/native/Linux/libyakeNativeLinux.la $(top_builddir)/src/yake/base/templates/libyakeTemplates.la $(top_builddir)/src/yake/base/math/libyakeMath.la
