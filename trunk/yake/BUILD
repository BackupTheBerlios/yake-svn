
This file is part of Yake. Copyright (c) by the Yake Team and the contributors.

This document contains build instructions for different platforms:

CONTENTS
--------

0. Prologue
1. BUILD INSTRUCTIONS FOR YAKE ON PC/WINDOWS
2. BUILD INSTRUCTIONS FOR YAKE ON PC/LINUX

0. Prologue
-----------

The YAKE Documentation and User Wiki may contain more detailed instructions or
additional information.

You can find it here: http://www.yake.org/wiki .


1. BUILD INSTRUCTIONS FOR YAKE ON PC/WINDOWS
--------------------------------------------

Last modified: 21/06/2006 by Stephan Kaiser (psy)

Please refer to the manual for detailed instructions.

1.1 Building with Microsoft Visual C++ 2003/2005 (7.1/8.0)
----------------------------------------------------------

In essence, opening the workspace for Yake and starting
a complete 'Build' is what you want to do.

You can find the workspaces for MSVC 2003 (7.1) and MSVC 2005 (8.0)
respectively here:

/yake/scripts/msvc71/yake.sln
/yake/scripts/msvc8/yake.sln

For stable releases of Yake for both compilers prebuilt
dependencies packages are available.

If you don't have or don't want to use the dependencies package
then you have to compile these yourself as needed, of course.

See the file /yake/dependencies/DEPENDENCIES for more information.

1.2 Building with Code::Blocks
------------------------------

The Code Blocks project files can be found in this directory:

/yake/scripts/cb/


2. BUILD INSTRUCTIONS FOR YAKE ON PC/LINUX
------------------------------------------

Last modified: 30/9/2005 by Stephan Kaiser (psy)
Last modified: 22/9/2004 by Nikita Buida (mj)

This instruction applies to SVN (Subversion) version of Yake.

If you encounter problems and need the very latest instructions, please
visit www.yake.org and search the forums and the wiki. You will most likely
find the solution there.

Assuming that you are in yake root directory.

#. Copy all files except SVN related ones to yake root dir from ./scripts/linux.
#. Set up ./dependencies dir.
Its structure should look like this:
/dependencies
	|
	|
	/luabind
	|
	/OGRE
	|
	/tinyxml
	|
	/ttl

All that dirs are in Yake dependencies package.
Note: you DON'T really need that ./dependencies dir in Linux. With latest fixes you can just 
install all that libraries on your Linux system as usual. You should edit OgreString.h in 
dependencies dir as it is described in Yake forums to avoid string redefinition ( 
just comment out Ogre's definitions of additional operators ).

Next, you have 2 options for building Yake.
Way 1. SCons [RECOMMENDED]
Install SCons build system. Just 'emerge scons' if you are on Gentoo like me, or go to www.scons.org or search for
package for your distro.

Then go to yake root dir and run:
# scons

That will build everything. After that as root:
# scons install

That installs libraries. 
Update dynamic loader cache (as root):
# ldconfig

Now you will find all demos in common/bin directory ;)

ATTENTION! This is my preferred way to build Yake. It is best supported by me from now on ( 10/2005 ).

Way 2. KDevelop/automake
#. Execute autogen.sh script - it will create various missing scripts needed for automake & friends.

Now there are two ways of building Yake: automated-command-line and IDE-wise ;)
If you want command line build then do as usual:
	./configure
	make
	make install

Else, KDevelop'ers could do:
#. Run KDevelop and open project file from yake root dir.
#. Run automake and friends from KDevelop Build menu.
#. Run configure from Build menu. 

#. You can start building libraries now.
You can just build the whole project. 

# Install all built libraries (with root rights). They are all installed in /usr/local/lib by default

# In Project settings change run options. There are absolute paths to the program to run. You should change that
according to your configuration.

# From now on you're able to run the demos.
You will need to set up working dir for demo, e.g. create config files and have Ogre's media.


