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
#ifndef YAKE_BASE_PARAMHOLDER_H
#define YAKE_BASE_PARAMHOLDER_H

#include "yakePrerequisites.h"
#include <yake/base/templates/yakeVariant.h>

namespace yake {
namespace base {

	/** ParamHolder holds any amount of parameters. A parameter is a key-value pair where
		the key is a String and the value is stored in a Variant.
		The amount of values such a variant can hold is limited to a few common types.
		Pointers are wrapped within a ParamHolder::Pointer object because several
		templates fail when trying to use the variant with (void*) as a type or similar types.
	*/
	class YAKE_BASE_API ParamHolder
	{
	public:
		ParamHolder();
		~ParamHolder();
		enum ParamType {
			kNone=0,
			kUint32=1,
			kReal=2,
			kString=3,
			kPointer=4
		};
		typedef uint8 ParamTypeNone;

		/** Wrapper class for (void*) pointers.
		*/
		class Pointer {
		public:
			Pointer( void* p ) : p_(p)
			{}
			void* get() const
			{ return p_; }
			operator void*()
			{ return p_; }
			Pointer& operator= (const Pointer & rhs)
			{ p_ = rhs.p_; }
			Pointer& operator= (void* p)
			{ p_ = p; }
		private:
			void*	p_;
		};
		/** Parameter value */
		typedef Variant<ParamTypeNone,uint32,real,String,Pointer> Param;

		/** A vector of strings */
		typedef templates::Vector<String> StringVector;

		/** Returns all keys stored in the parameter holder object. */
		StringVector getKeys() const;

		/** Returns the parameter value for a given key (id). */
		Param get( const String & id ) const;

		/** Returns the parameter value for a given key (id). */
		Param operator [] ( const String & id ) const;

		/** Sets the value of a given parameter identified by a key (id) to an uint32. */
		bool set( const String & id, const uint32 value );

		/** Sets the value of a given parameter identified by a key (id) to a String. */
		bool set( const String & id, const String & value );

		/** Sets the value of a given parameter identified by a key (id) to a real. */
		bool set( const String & id, const real value );

		/** Sets the value of a given parameter identified by a key (id) to a Pointer wrapper object. */
		bool set( const String & id, Pointer & value );
	private:
		typedef templates::AssocVector< String, Param > ParamMap;
		ParamMap	mParams;
	};

}
}
#endif
