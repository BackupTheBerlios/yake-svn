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
#ifndef YAKE_RX_PROP_H
#define YAKE_RX_PROP_H

#include <yake/rx/yakeRxPrerequisites.h>

namespace yake {
	using namespace base;
namespace rx {

	class YAKE_RX_API MemPtrBase
	{
	public:
		virtual ~MemPtrBase() {}
		virtual void setValue(MemPtrBase* pBase) = 0;
		virtual void setValueString(const String & value) = 0;
		virtual String getValueString() const = 0;
		virtual MemPtrBase* clone() = 0;
		virtual void __badHack_setObject(void*) = 0;
	private:
	};

	template<typename ValueT>
		ValueT rxConvertFromString(const String & value);

	template<>
		YAKE_RX_API float rxConvertFromString(const String & value);

	template<typename FromT>
		YAKE_RX_API String rxConvertToString(const FromT & t);

	template<>
		YAKE_RX_API String rxConvertToString<float>(const float & t);

	template<>
		YAKE_RX_API String rxConvertToString<int>(const int & t);

	template<class ClassType>
	class MemPtrMid : public MemPtrBase
	{
	public:
		void setObject( ClassType & o )
		{
			mpO = &o;
		}
		virtual void __badHack_setObject(void*pO)
		{
			RxObject* pRxO = reinterpret_cast<ClassType*>(pO);
			mpO = static_cast<ClassType*>(pRxO);
		}
	protected:
		ClassType*				mpO;
	};

	template<class ClassType, class ValueType>
	class MemPtr : public MemPtrMid<ClassType>
	{
	public:
		MemPtr(ValueType ClassType::* p)
		{
			mP = p;
			mpO = 0;
		}
		MemPtr(ClassType& o, ValueType ClassType::* p)
		{
			mP = p;
			mpO = &o;
		}
		MemPtr<ClassType,ValueType>& operator = (const MemPtr<ClassType,ValueType> & rhs)
		{
			mP = rhs.mP;
			mpO = rhs.mpO;
		}
		virtual MemPtrBase* clone()
		{
			if (mpO)
				return new MemPtr<ClassType, ValueType>( *mpO, mP );
			else
				return new MemPtr<ClassType, ValueType>( mP );
		}
		virtual void setValue(MemPtrBase* pBase)
		{
			MemPtr<ClassType,ValueType>* p = dynamic_cast<MemPtr<ClassType,ValueType>*>(pBase);
			if (p)
			{
				mP = p->mP;
				mpO = p->mpO;
			}
		}
		void setValue(const ValueType & v)
		{
			if (mpO)
				(mpO)->*mP = v;
		}
		virtual void setValueString(const String & value)
		{
			if (mpO)
				(mpO)->*mP = rxConvertFromString<ValueType>(value);
		}
		virtual String getValueString() const
		{
			if (mpO)
				return rxConvertToString(((mpO)->*mP));
			else
				return "mpO == 0";
		}
	private:
		ValueType ClassType::*	mP;
	};

	/** Possible attribute value types. */
	enum ValueType {
		VT_STRING,
		VT_INT32,
		VT_UINT32,
		VT_BOOL,
		VT_REAL,
		VT_VECTOR3,
		VT_QUATERNION,
		VT_UNKNOWN
	};

	/** Base class for RX values.
		@Remarks
			I did numerous attempts at a very flexible properties/attribute system
			totally integrated into C++ thereby allowing for direct assignment
			of correct types (like a 'real' attribute accepts operator =(const real)).
			Now I though, stop trying, get the basic system done & working and see
			what you can do after that. That's why this property/attribute system
			is String based for now (ugh!).
	*/
	class YAKE_RX_API ValueBase// : public RxObject
	{
	public:
		virtual ~ValueBase() {}
		virtual void setValueString( const String & value ) = 0;
		virtual String getValueString() = 0;
		virtual ValueBase& operator = ( const String & rhs ) = 0;
		//RXO_DECLARE_NOFACTORY( ValueBase );
	};

	/** Represents a boolean attribute value.
		@Remarks
			The value can either be stored only in this object or it can be
			'wrapped' when pointing towards a piece of memory containing the
			actual value.
		@todo
			add copy c'tor, operator =, etc.
	*/
	class YAKE_RX_API ValueBool : public ValueBase
	{
	public:
		ValueBool(bool value);
		ValueBool(bool* pWrappedValue = 0);
		//RXO_DECLARE( ValueBool );
		virtual void setValueString( const String & value );
		virtual String getValueString();
		ValueBase& operator = ( const String & rhs );
	private:
		bool	mValue;
		bool*	mWrapped;
	};

	class YAKE_RX_API ValueReal : public ValueBase
	{
	public:
		ValueReal(real value);
		ValueReal(real* pWrappedValue = 0);
		virtual void setValueString( const String & value );
		virtual String getValueString();
		ValueBase& operator = ( const String & rhs );
	private:
		real	mValue;
		real*	mWrapped;
	};

	class Attribute;
	/** Represents an attribute prototype.
	*/
	class YAKE_RX_API AttributeProto
	{
	public:
		AttributeProto( const String & name, ValueType type, MemPtrBase* pMemPtr = 0 );
		~AttributeProto();
		String getName() const;
		ValueType getType() const;
		String getTypeAsString() const;

		String getValueAsString() const;
		void setValueString(const String & value);

		Attribute* createConcrete( void* pValuePtr );

		template<class T>
			Attribute* createConcreteMemPtr( T& o )
		{
			MemPtrMid<T>* pClone = static_cast<MemPtrMid<T>*>(mpMemPtr->clone()); // slightly unsafe...
			pClone->setObject( o );
			return new Attribute(mName, mType, pClone);
		}
	private:
		String		mName;
		ValueType	mType;
		String		mValue;
		MemPtrBase*	mpMemPtr;
	};

	typedef YAKE_RX_API templates::Deque<AttributeProto*> AttrProtoPtrList;

	/** Represents a single attribute of an rx object or class.
	*/
	class YAKE_RX_API Attribute
	{
	protected:
		Attribute( const Attribute & other );
	public:
		Attribute( const String & name, ValueType type, void* pValuePtr = 0 );
		Attribute( const String & name, ValueType type, MemPtrBase * pMemPtr );
		~Attribute();
		String getName() const;
		ValueType getType() const;
		String getTypeAsString() const;

		String getValueAsString() const;
		void setValueString(const String & value);

		Attribute* clone();
		MemPtrBase* _getMemPtrBase() const;
	private:
		Attribute& operator = (const Attribute & rhs);
	private:
		String		mName;
		ValueType	mType;
		ValueBase*	mpValue;
		MemPtrBase*	mpMemPtr;
	};

	typedef YAKE_RX_API templates::Deque<Attribute*> AttrPtrList;

	inline std::ostream& operator << (std::ostream & s, const AttrPtrList & rhs)
	{
		s << "attributes (count= " << rhs.size() << ")" << std::endl;
		templates::ConstDequeIterator<AttrPtrList> it( rhs.begin(), rhs.end() );
		while (it.hasMoreElements())
		{
			Attribute* pAttr = it.getNext();
			YAKE_ASSERT( pAttr );
			s << "  " << pAttr->getName();
			s << " (" << pAttr->getTypeAsString() << ")";
			s << " = " << pAttr->getValueAsString();
			s << std::endl;
		}
		return s;
	}

	/** Interface for querying rx attributes */
	class YAKE_RX_API IAttributeQuery
	{
	public:
		virtual ~IAttributeQuery() {}
		virtual AttrPtrList getAttributes( bool bStatic = true, bool bDynamic = true, bool bIncludeSuperClasses = false ) const = 0;
		virtual Attribute* getAttribute( const String & name ) const = 0;
	};
	/** Interface for inserting new rx attributes */
	class YAKE_RX_API IDynamicAttributeInsert
	{
	public:
		virtual ~IDynamicAttributeInsert() {}
		virtual void addDynamicAttribute( Attribute* pProp ) = 0;
	};
	/** Interface for erasing rx attributes */
	class YAKE_RX_API IDynamicAttributeErase
	{
	public:
		virtual ~IDynamicAttributeErase() {}
		virtual void eraseDynamicAttribute( Attribute* pProp ) = 0;
	};

	/** Base class for attribute holder policies  */
	class YAKE_RX_API AttributeHolderPolicyBase
	{
	public:
		virtual ~AttributeHolderPolicyBase() {}
	protected:
		AttrPtrList	mAttributes;
	};

	/** Attribute holder policy class that does not provide a query interface. */
	class YAKE_RX_API AttributeHolderNoQueryAttributes
	{
	public:
	};
	/** Attribute holder policy class that does provide a query interface. */
	class YAKE_RX_API AttributeHolderQueryAttributes : public IAttributeQuery, virtual public AttributeHolderPolicyBase
	{
	public:
		virtual AttrPtrList getAttributes( bool bStatic = true, bool bDynamic = true, bool bIncludeSuperClasses = false ) const;
		virtual Attribute* getAttribute( const String & name ) const;
	};

	/** Attribute holder policy class that does not provide a dynamic attribute insertion interface. */
	class YAKE_RX_API AttributeHolderNoDynamicInsert
	{
	public:
	};
	/** Attribute holder policy class that does provide a dynamic attribute insertion interface. */
	class YAKE_RX_API AttributeHolderDynamicInsert : public IDynamicAttributeInsert, virtual public AttributeHolderPolicyBase
	{
	public:
		virtual void addDynamicAttribute( Attribute* pProp );
	};

	/** Generic attribute holder class. Implementation is configured using policies. */
	template<	typename AttributeQueryPolicy = AttributeHolderQueryAttributes,
				typename AttributeDynamicInsertPolicy = AttributeHolderNoDynamicInsert >
	class AttributeHolder : public AttributeQueryPolicy, public AttributeDynamicInsertPolicy
	{
	public:
	protected:
	};

}
}
#endif