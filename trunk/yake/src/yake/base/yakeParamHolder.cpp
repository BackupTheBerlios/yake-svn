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
#include <yake/base/yakePCH.h>
#include <yake/base/yakeParamHolder.h>

namespace yake {

	ParamHolder::ParamHolder() : mParamNone( bool(false) )
	{
	}
	ParamHolder::~ParamHolder()
	{
		mParams.clear();
	}
	bool ParamHolder::has( const String & id ) const
	{
		return (mParams.end() != mParams.find( id ));
	}
	ParamHolder::StringVector ParamHolder::getKeys() const
	{
		StringVector keys;
		for (ParamMap::const_iterator it = mParams.begin(); it != mParams.end(); ++it)
			keys.push_back( it->first );
		return keys;
	}
	const ParamHolder::Value& ParamHolder::get( const String & id ) const
	{
		ParamMap::const_iterator itFind = mParams.find( id );
		if (itFind != mParams.end())
			return itFind->second;
		return mParamNone;
	}
	ParamHolder::Value ParamHolder::operator [] ( const String & id ) const
	{
		return get( id );
	}
	bool ParamHolder::set( const String & id, const Value& value )
	{
		YAKE_ASSERT( !id.empty() );
		if (id.empty())
			return false;
		mParams[ id ] = value;
		return true;
	}

}
