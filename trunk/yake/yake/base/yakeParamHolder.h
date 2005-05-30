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

namespace yake {

	/** ParamHolder holds any amount of parameters. A parameter is a key-value pair where
		the key is a String and the value is stored in a boost::any object.
	*/
	class YAKE_BASE_API ParamHolder
	{
	public:
		ParamHolder();
		~ParamHolder();

		/** Parameter value */
		typedef boost::any Value;

		/** A vector of strings */
		typedef Vector<String> StringVector;

		/** Returns all keys stored in the parameter holder object. */
		StringVector getKeys() const;

		/** Returns the parameter value for a given key (id). */
		const Value& get( const String & id ) const;

		/** Returns the parameter value for a given key (id). */
		Value operator [] ( const String & id ) const;

		/** Sets the value of a given parameter identified by a key (id) to an uint32. */
		bool set( const String & id, const Value& value );

		bool has( const String & id ) const;
	private:
		typedef AssocVector< String, Value > ParamMap;
		ParamMap		mParams;
		Value			mParamNone;
	};

}
#endif
