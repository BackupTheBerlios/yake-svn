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
If you are interested in another license model contact the Yake Team via
E-Mail: team@yake.org.
For more information see the LICENSE file in the root directory of the
source code distribution.
------------------------------------------------------------------------------------
*/
#include <yake/rx/yakePCH.h>
#include <yake/rx/yakeRx.h>

namespace yake {
namespace rx {

	RXO_DEFINE_BASE( RxClass );

	String RxClass::getName() const
	{
		return mName;
	}
	void RxClass::addSuper(RxClass* pClass)
	{
		YAKE_ASSERT( pClass );
		mParents.push_back( pClass );
	}
	bool RxClass::isDerivedFrom(RxClass* pClass)
	{
		YAKE_ASSERT( pClass );
		for (ClassList::const_iterator it = mParents.begin(); it != mParents.end(); ++it)
		{
			RxClass* pSuper = *it;
			YAKE_ASSERT( pSuper );
			if (pSuper == pClass)
				return true;
			if (pSuper->isDerivedFrom( pClass ))
				return true;
		}
		return false;
	}
	RxClass::RxClass(const String & name, FN_CREATERXOBJECT fnCreate) : 
					mName(name), mFnCreate(fnCreate)
	{
		YAKE_ASSERT( mFnCreate );
		static sIds = 0;
		mId = sIds++;
	}
	RxClass::~RxClass()
	{}
	RxObject* RxClass::createObject()
	{
		YAKE_ASSERT( mFnCreate );
		if (mFnCreate)
		{
			RxObject* pObject = mFnCreate();
			if (!pObject)
				return 0;
			AttrPtrList attr = getAttributes(true, true, true);
			::yake::base::templates::ConstDequeIterator<AttrPtrList> it(attr.begin(), attr.end());
			while (it.hasMoreElements())
			{
				Attribute* pAttr = it.getNext();
				if (!pAttr)
					continue;
				Attribute* pClone = pAttr->clone();
				YAKE_ASSERT( pClone );
				if (!pClone)
					continue;

				MemPtrBase* pMemPtrBase = dynamic_cast<MemPtrBase*>(pClone->_getMemPtrBase());
				if (pMemPtrBase)
					pMemPtrBase->__badHack_setObject(pObject);

				pObject->addDynamicAttribute( pClone );
			}
			return pObject;
		}
		else
			return 0;
	}
	RxClassId RxClass::getId() const
	{ return mId; }
	bool RxClass::operator == (const RxClass & rhs)
	{ return mId == rhs.getId(); }
}
}