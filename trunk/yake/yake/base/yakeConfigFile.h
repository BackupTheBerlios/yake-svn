/*
   ------------------------------------------------------------------------------------
   This file is part of YAKE
   Copyright © 2004 The YAKE Team
   For the latest information visit http://www.yake.org 
   ------------------------------------------------------------------------------------
   This program is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License along with
   this program; if not, write to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA, or go to
   http://www.gnu.org/copyleft/lesser.txt.
   ------------------------------------------------------------------------------------
   If you are interested in another license model contact the Yake Team via
   E-Mail: team@yake.org.
   For more information see the LICENSE file in the root directory of the
   source code distribution.
   ------------------------------------------------------------------------------------
*/
#ifndef YAKE_BASE_CONFIGFILE_H
#define YAKE_BASE_CONFIGFILE_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include <yake/base/yakePrerequisites.h>
#endif

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake
{
namespace base
{

/** Represents node in a hierarchical configuration file.
* \todo Modify it to use 'weak ptr'.
*/
class YAKE_BASE_API ConfigNode
{
// Types
public:
	typedef std::map< String, String > KeyValueMap;
	typedef std::map< String, ConfigNode* > ConfigNodeList;

// Class
private:
	ConfigNode( const ConfigNode& );
public:
	ConfigNode() : mName( "" ) {}

// Methods
public:
	/** C'tor, specifying node name.
		\param name the name of this configuration node.
		\return 
	*/
	ConfigNode( const String& rName );
	~ConfigNode();

	/** Get this node's name.
		\return this configuration node's name.
	*/
	String getName() const;

	/** Return the value associated to the key <key>.
		\param key The key for which the value shall be returned.
		\return the string containing the value associated with <key>.
	*/
	String getValue( const String& rKey );
		
	/** Adds a key-value pair to the configuration node.
		One key can be associated with one value.
		\param key the key string.
		\param value the associated value string.
	*/
	void addKeyValuePair( const String& rKey, const String& rValue );

	/** Get a child by its name.
		\param name the child node's name.
		\return a pointer to the child node or 0 if there's no child with the given name.
	*/
	ConfigNode* getNodeByName( const String& rName );
		
	/** Adds a configuration node to the node's list of child nodes.
		Ownership transfers to this node.
		\param node a pointer to free configuration node.
	*/
	void addNode( ConfigNode* rNode );

// Data
protected:
	KeyValueMap		mKeyValueMap;
	ConfigNodeList	mChildren;
	String			mName;
};

/** Represents a hierarchical configuration file.
* \todo Modify it to use 'weak ptr'.
*/
class YAKE_BASE_API ConfigFile
{
// Class
private:
	ConfigFile( const ConfigFile& );
public:
	ConfigFile( const String& rFile = "" );
	~ConfigFile();

// Methods
public:
	/** Load a configuration file. If <file> is empty
		then the filename specified at construction time
		will be used. If it was empty, too, no configuration
		file will be loaded.
		\param file configuration file name
		\return true on success, false on error.
	*/
	bool load( const String& rFile = "" );

	/** Get a pointer to the root node of the configuration node tree.
		There can be only one root.
		Ownership is _not_ transferred.
		\return pointer to the configuration tree's root node.
	*/
	RefPtr<ConfigNode> getRootNode();

// Data
protected:
	String				filename_;
	RefPtr<ConfigNode>	mRootNode;
};

} // base
} // yake

#endif // YAKE_BASE_CONFIGFILE_H

