/**------------------------------------------------------------------------------------
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
*/
#ifndef YAKE_BASE_TEMPLATES_ASSOCIATOR_H
#define YAKE_BASE_TEMPLATES_ASSOCIATOR_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASEPREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

/** A template class for managing names which are associated with object pointers.
*/
template< typename T >
class ObjectPtrNameAssociator // todo: how`s this? => "ObjectPtrNameAssociator : public Manager< String, T >"
{
public:
	/** Associates an object with a name.
	*/
	virtual void associate( const String& rName, T* pObject )
	{
		YAKE_ASSERT( pObject ).debug("non-0 value expected! request ignored.");
		if (!pObject)
			return;

		YAKE_ASSERT( rName.length() > 0 )( rName )( rName.length() ).debug("name is empty. request ignored.");
		if (rName.length() == 0)
			return;
		mObjectNames[ rName ] = pObject;
	}
	/** Gets the object associated to a name.
	*/
	virtual T* get( const String& rName )
	{
		typename ObjectNameMap::const_iterator itFind = mObjectNames.find( rName );
		if (itFind != mObjectNames.end())
			return itFind->second;
		return 0;
	}

	virtual void removeAll()
	{
		mObjectNames.clear();
	};

	virtual void removeAndDestroyAll()
	{
		for (ObjectNameMap::const_iterator it = mObjectNames.begin(); it != mObjectNames.end(); ++it)
		{
			if (it->second)
				delete it->second;
		}
		mObjectNames.clear();
	};

protected:
	typedef std::map< String, T* > ObjectNameMap;
	ObjectNameMap	mObjectNames;
};

} // yake

#endif // YAKE_BASE_TEMPLATES_ASSOCIATOR_H
