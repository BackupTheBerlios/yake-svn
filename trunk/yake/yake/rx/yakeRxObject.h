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
#ifndef YAKE_RX_OBJECT_H
#define YAKE_RX_OBJECT_H

#include <yake/rx/yakeRxPrerequisites.h>

namespace yake {
namespace rx {

	/**
		@todo Hierarchy, Inheritance, ...
	*/

	class RxObject;
	class RxClass;

#define RXO_DECLARE_COMMON( RXNAME ) \
		private: \
			static ::yake::rx::RxClass msStaticRxClass; \
			typedef RXNAME ThisClass; \
			static rx::AttrProtoPtrList ms_attrProto; \
		protected: \
			static void rxInitObjectAttributes( RXNAME& o ); \
		public: \
			::yake::rx::AttrProtoPtrList getAllStaticAttrProto(); \
			typedef RXNAME ThisClass; \
			static ::yake::rx::RxClass* rxGetClass(); \
			virtual ::yake::rx::RxClass* isA() const; \
			static ThisClass* safeCast(RxObject* pObject);

#define RXO_DECLARE_FACTORY( RXNAME ) \
			static RXNAME* create();

#define	RXO_DECLARE_BASE( RXNAME ) \
		RXO_DECLARE_COMMON( RXNAME ) \
		RXO_DECLARE_FACTORY( RXNAME )

#define	RXO_DECLARE( RXNAME ) \
		RXO_DECLARE_COMMON( RXNAME ) \
		RXO_DECLARE_FACTORY( RXNAME ) \
		static void rxInit();

#define	RXO_DECLARE_NOFACTORY( RXNAME ) \
		RXO_DECLARE_COMMON( RXNAME ) \
		static void rxInit();

#define RXO_DEFINE_COMMON( RXNAME ) \
		::yake::rx::AttrProtoPtrList RXNAME::ms_attrProto; \
		::yake::rx::RxClass RXNAME::msStaticRxClass(#RXNAME, (::yake::rx::FN_CREATERXOBJECT)RXNAME::create); \
		::yake::rx::RxClass* RXNAME::isA() const \
		{ \
			return rxGetClass(); \
		} \
		::yake::rx::RxClass* RXNAME::rxGetClass() \
		{ \
			return &msStaticRxClass; \
		} \
		RXNAME* RXNAME::safeCast(RxObject* pObject) \
		{ \
			YAKE_ASSERT( pObject ); \
			if (&msStaticRxClass == pObject->isA()) \
				return static_cast<RXNAME*>(pObject); \
			else \
			{ \
				if (pObject->isA()->isDerivedFrom(&msStaticRxClass)) \
					return static_cast<RXNAME*>(pObject); \
				else \
					return 0; \
			} \
		} 

#define	RXO_DEFINE_FACTORY( RXNAME ) \
		RXNAME* RXNAME::create() \
		{ \
			RXNAME* pNew = new RXNAME(); \
			if (!pNew) \
				return 0; \
			RXNAME::rxInitObjectAttributes(*pNew); \
			return pNew; \
		}

#define	RXO_DEFINE_BASE( RXNAME ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		RXO_DEFINE_FACTORY( RXNAME ) \
		void RXNAME::rxInitObjectAttributes( RXNAME& o ) \
		{ \
			::yake::base::templates::ConstDequeIterator<rx::AttrProtoPtrList> it( RXNAME::ms_attrProto.begin(), RXNAME::ms_attrProto.end() ); \
			while (it.hasMoreElements()) \
				o.addDynamicAttribute(it.getNext()->createConcreteMemPtr(o)); \
		} \
		::yake::rx::AttrProtoPtrList RXNAME::getAllStaticAttrProto() \
		{ \
			::yake::rx::AttrProtoPtrList empty; \
			return empty; \
		}

#define	RXO_DEFINE( RXNAME, RXSUPER ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		RXO_DEFINE_FACTORY( RXNAME ) \
		void RXNAME::rxInit() \
		{ \
			RXNAME::rxInitAttributePrototypes(); \
			msStaticRxClass.addSuper( RXSUPER::rxGetClass() ); \
		} \
		void RXNAME::rxInitObjectAttributes( RXNAME& o ) \
		{ \
			RXSUPER::rxInitObjectAttributes( o ); \
			::yake::base::templates::ConstDequeIterator<rx::AttrProtoPtrList> it( RXNAME::ms_attrProto.begin(), RXNAME::ms_attrProto.end() ); \
			while (it.hasMoreElements()) \
				o.addDynamicAttribute(it.getNext()->createConcreteMemPtr(o)); \
		} \
		::yake::rx::AttrProtoPtrList RXNAME::getAllStaticAttrProto() \
		{ \
			return ms_attrProto; \
		}

#define	RXO_DEFINE_NOFACTORY( RXNAME, RXSUPER ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		void RXNAME::rxInit() \
		{ \
			RXSUPER::rxInitAttributePrototypes(); \
			RXNAME::rxInitAttributePrototypes(); \
			msStaticRxClass.addSuper( RXSUPER::rxGetClass() ); \
		} \
		::yake::rx::AttrProtoPtrList RXNAME::getAllStaticAttrProto() \
		{ \
			::yake::rx::AttrProtoPtrList list = ms_attrProto; \
			list += RXSUPER0::getAllStaticAttrProto(); \
			return ms_attrProto; \
		}

#define	RXO_DEFINE2( RXNAME, RXSUPER0, RXSUPER1 ) \
		RXO_DEFINE_COMMON( RXNAME ) \
		void RXNAME::rxInit() \
		{ \
			RXSUPER0::rxInitAttributePrototypes(); \
			RXSUPER1::rxInitAttributePrototypes(); \
			RXNAME::rxInitAttributePrototypes(); \
			msStaticRxClass.addSuper( RXSUPER0::rxGetClass() ); \
			msStaticRxClass.addSuper( RXSUPER1::rxGetClass() ); \
		} \
		::yake::rx::AttrProtoPtrList RXNAME::getAllStaticAttrProto() const \
		{ \
			::yake::rx::AttrProtoPtrList list = ms_attrProto; \
			list += RXSUPER0::getAllStaticAttrProto(); \
			list += RXSUPER1::getAllStaticAttrProto(); \
			return ms_attrProto; \
		}


	class YAKE_RX_API RxObject :
		public AttributeHolder<AttributeHolderQueryAttributes,AttributeHolderDynamicInsert>
	{
	public:
		virtual ~RxObject() {}
		RXO_DECLARE_BASE( RxObject );
	};

	typedef YAKE_RX_API ::yake::rx::RxObject*(*FN_CREATERXOBJECT)();

}
}

#endif