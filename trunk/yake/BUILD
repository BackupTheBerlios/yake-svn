
This document contains build instructions for different platforms:

CONTENTS
--------

0. Prologue
1. BUILD INSTRUCTIONS FOR YAKE ON PC/WIN
2. BUILD INSTRUCTIONS FOR YAKE 0.1b2 ON PC/LINUX

0. Prologue
-----------

The YAKE Documentation and User Wiki may contain more detailed instructions or
additional information.

You can find it here: http://www.yake.org/wiki .


1. BUILD INSTRUCTIONS FOR YAKE ON PC/WIN
----------------------------------------

Last modified: 16/9/2004 by Stephan Kaiser (psy)

Please refer to the manual for detailed instructions.
In essence, opening the workspaces for Yake and Yapp
respectively and doing a complete 'Build' is what you
want to do.

You can find the workspaces here:

(yake_root_dir)/scripts/yake/yake.sln

and

(yake_root_dir)/scripts/yapp/yapp.sln



2. BUILD INSTRUCTIONS FOR YAKE 0.1b2 ON PC/LINUX
------------------------------------------------

Last modified: 22/9/2004 by Nikita Buida (mj)

This instruction applies to CVS version of Yake.

Assuming that you are in yake root directory.

#. Copy all files except CVS to yake root dir from ./scripts/linux.
#. Set up ./dependencies dir.
Its structure should look like this:
/dependencies
	|
	/boost/boost
	|
	/lua
	|
	/luabind
	|
	/ode
	|
	/ogrenew
	|
	/tinyxml
	|
	/ttl

All that dirs are in Yake dependencies package.
Note: you DON'T really need that ./dependencies dir in Linux. With latest fixes you can just 
install all that libraries on your Linux system as usual. You should edit OgreString.h in 
dependencies dir as it is described in Yake forums to avoid string redefinition ( 
just comment out Ogre's definitions of additional operators ).

#. Execute autogen.sh script - it will create various missing scripts needed for automake & friends.

Now there are two ways of building Yake: automated-command-line and IDE-wise ;)
If you want command line build then do as usual:
	./configure
	make
	make install

NOTE:

On the current state of development not all components of CVS version can be built 
without errors as it is unstable. So for the first time I'd recomend to use KDevelop
to rid of minor bugs and then to build however you like.

Else, KDevelop'ers could do:
#. Run KDevelop and open project file from yake root dir.
#. Run automake and friends from KDevelop Build menu.
#. Run configure from Build menu. 

#. You can start building libraries now.
You may just build the whole project. But if you want more detailed knowledge of Yake
start off with base and its components. Then move to plugins. And finally samples.

# Install all built libraries (with root rights). They are all installed in /usr/local/lib by default

# In Project settings change run options. There are absolute paths to the program to run. You should change that
according to your configuration.

# Build GraphicsDemo. From now on you're able to run the most (IMHO) impressive Yake demo.
You will need to set up working dir for demo, e.g. create config files and have Ogre's media.


NOTE:

To 22/09/2004 I managed to build only described libraries and demos. More to come in 
the next release of this doc.

