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

	template<>
		float rxConvertFromString(const String & value)
	{
		return (float)atof( value.c_str() );
	}

	template<>
		String rxConvertToString<float>(const float & t)
	{
		::std::stringstream ss;
		ss << "(float)" << t;
		return String(ss.str().c_str());
	}

	template<>
		String rxConvertToString<int>(const int & t)
	{
		::std::stringstream ss;
		ss << "(int)" << t;
		return String(ss.str().c_str());
	}

	//RXO_DEFINE_NOFACTORY( ValueBase, RxObject );

	//RXO_DEFINE( ValueBool, ValueBase );
	ValueBool::ValueBool(bool value) : mWrapped(0), mValue(value)
	{
		mWrapped = &mValue;
	}
	ValueBool::ValueBool(bool* pWrappedValue) : mWrapped(pWrappedValue), mValue(false)
	{
		if (mWrapped == 0)
			mWrapped = &mValue;
	}
	void ValueBool::setValueString(const String & value)
	{
		if (value == "yes" || value == "1" || value == "true")
		{
			*mWrapped = true;
		}
		else
			*mWrapped = false;
	}
	String ValueBool::getValueString()
	{
		return (*mWrapped ? "true" : "false");
	}
	ValueBase& ValueBool::operator = ( const String & rhs )
	{
		setValueString( rhs );
		return static_cast<ValueBase&>(*this);
	}

	//-----------------------------------------------------

	ValueReal::ValueReal(real value) : mWrapped(0), mValue(value)
	{
		mWrapped = &mValue;
	}
	ValueReal::ValueReal(real* pWrappedValue) : mWrapped(pWrappedValue), mValue(false)
	{
		if (mWrapped == 0)
			mWrapped = &mValue;
	}
	void ValueReal::setValueString(const String & value)
	{
		*mWrapped = StringUtil::toReal( value );
	}
	String ValueReal::getValueString()
	{
		StrStream s;
		s << *mWrapped;
		return s.str();
	}
	ValueBase& ValueReal::operator = ( const String & rhs )
	{
		setValueString( rhs );
		return static_cast<ValueBase&>(*this);
	}

	//-----------------------------------------------------

	AttributeProto::AttributeProto( const String & name, ValueType type, MemPtrBase* pMemPtr ) :
		mName( name ),
		mType( type ),
		mpMemPtr( pMemPtr )
	{
	}
	AttributeProto::~AttributeProto()
	{
	}
	Attribute* AttributeProto::createConcrete( void* pValuePtr )
	{
		return new Attribute(mName, mType, pValuePtr);
	}
	String AttributeProto::getName() const
	{
		return mName;
	}
	ValueType AttributeProto::getType() const
	{
		return mType;
	}
	String AttributeProto::getTypeAsString() const
	{
		switch (mType)
		{
		case VT_BOOL:
			return "bool";
		case VT_STRING:
			return "string";
		case VT_INT32:
			return "int32";
		case VT_UINT32:
			return "uint32";
		case VT_REAL:
			return "real";
		case VT_UNKNOWN:
			return "-unknown-";
		default:
			{
				StrStream ss;
				ss << "-unknown(" << mType << ")";
				return ss.str();
			}
		};
	}
	String AttributeProto::getValueAsString() const
	{
		return mValue;
	}
	void AttributeProto::setValueString(const String & value)
	{
		mValue = value;
	}

	//-----------------------------------------------------

	Attribute::Attribute( const String & name, ValueType type, void* pValuePtr ) :
		mName( name ),
		mType( type ),
		mpValue( 0 ),
		mpMemPtr( 0 )
	{
		if (type == VT_BOOL)
			mpValue = new ValueBool( ((pValuePtr) ? (bool*)pValuePtr : false) );
		YAKE_ASSERT( mpValue );
	}
	Attribute::Attribute( const String & name, ValueType type, MemPtrBase * pMemPtr ) :
		mName( name ),
		mType( type ),
		mpValue( 0 ),
		mpMemPtr( pMemPtr )
	{
	}
	Attribute::~Attribute()
	{
		YAKE_SAFE_DELETE( mpValue );
	}
	Attribute* Attribute::clone()
	{
		return new Attribute( *this );
	}
	Attribute::Attribute( const Attribute & other ) :
		mName( other.mName ),
		mType( other.mType ),
		mpValue( other.mpValue ),
		mpMemPtr( other.mpMemPtr ? other.mpMemPtr->clone() : 0 )
	{
	}
	/*
	Attribute& Attribute::operator = (const Attribute & rhs)
	{
		mName = rhs.mName;
		mType = rhs.mType;
		mpValue = rhs.mpValue;
		mpMemPtr = 0;
		if (rhs.mpMemPtr)
			mpMemPtr = rhs.mpMemPtr->clone();
		return *this;
	}
	*/
	MemPtrBase* Attribute::_getMemPtrBase() const
	{
		return mpMemPtr;
	}
	String Attribute::getName() const
	{
		return mName;
	}
	ValueType Attribute::getType() const
	{
		return mType;
	}
	String Attribute::getTypeAsString() const
	{
		switch (mType)
		{
		case VT_BOOL:
			return "bool";
		case VT_STRING:
			return "string";
		case VT_INT32:
			return "int32";
		case VT_UINT32:
			return "uint32";
		case VT_REAL:
			return "real";
		case VT_UNKNOWN:
			return "-unknown-";
		default:
			{
				StrStream ss;
				ss << "-unknown(" << mType << ")";
				return ss.str();
			}
		};
	}
	String Attribute::getValueAsString() const
	{
		if (!mpMemPtr)
		{
			YAKE_ASSERT( mpValue );
			return mpValue->getValueString();
		}
		else
		{
			return mpMemPtr->getValueString();
		}
	}
	void Attribute::setValueString(const String & value)
	{
		if (!mpMemPtr)
		{
			YAKE_ASSERT( mpValue );
			mpValue->setValueString( value );
		}
		else
		{
			mpMemPtr->setValueString( value );
		}
	}


	//-----------------------------------------------------

	AttrPtrList AttributeHolderQueryAttributes::getAttributes( bool bStatic, bool bDynamic, bool bIncludeSuperClasses ) const
	{
		return mAttributes;
	}
	Attribute* AttributeHolderQueryAttributes::getAttribute( const String & name ) const
	{
		return 0;
	}
	void AttributeHolderDynamicInsert::addDynamicAttribute( Attribute* pProp )
	{
		YAKE_ASSERT( pProp );
		mAttributes.push_back( pProp );
	}

}
}