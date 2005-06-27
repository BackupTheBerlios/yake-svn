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
#ifndef YAKE_BASE_RESOURCE_H
#define YAKE_BASE_RESOURCE_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef INC_YAKE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake
#include <yake/base/yakeDataChunk.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

	/** Resource interface.
	*/
	class YAKE_BASE_API Resource
	{
		Resource();
	protected:
		String		mName;
		size_t		mSize;
		Resource( const String & name ) : mName( name ), mSize ( 0 )
		{}
	public:
		virtual ~Resource();

		/** Loads the resource data.
			If the resource has already been loaded the function
			immediately returns.
		*/
		virtual void load() = 0;

		/** Unloads this resource's data.
			If resource data has already been unloaded the fcuntion
			immediately returns.
		*/
		virtual void unload() = 0;

		virtual void reload()
		{
			if (mLoaded)
				unload();
			load();
		}

		inline bool isLoaded()
		{ return mLoaded; }

		void touch();

		size_t getSize() const
		{ return mSize; }
	protected:
		bool		mLoaded;
		real		mLastTouched;
	};

	/** Resource manager base class.
	*/
	template < typename T >
	class YAKE_BASE_API ResourceManager
	{
	public:
		typedef T ResourceType;
	protected:
		ResourceManager()
		{
			mMemoryBudget = std::numeric_limits<size_t>::max();
			mMemoryUsage = 0;
		}
	protected:
		struct ResourceEntry
		{
			T					* resource_;
			int				priority;
			ResourceEntry& operator = ( const ResourceEntry & rhs )
			{
				resource_ = rhs.resource_;
				priority  = rhs.priority;
			}
		};
		typedef HashMap< String, ResourceEntry, _StringHash > ResourceList;
		ResourceList	mResources;
		size_t		mMemoryBudget;
		size_t		mMemoryUsage;
	public:
		virtual ~ResourceManager() 
		{ 
			unloadAndDestroyAll();
		}

		void setMemoryBudget( size_t budgetInBytes );

		/** Add the resource with the given name.
			The resource is owned by the ResourceManager.
			\return a pointer to the resource object, or 0.
		*/
		virtual T* add( const String & name, int priority = 0 ) = 0;

		/** Get an existing resource associated with the given name. 
			Ownership stays with the ResourceManager.
			\return a pointer to the resource object, or 0.
		*/
		virtual T* get( const String & name );

		/** Unload all resources.
		*/
		virtual void unloadAll();

		/** Unload and destroy all resources.
		*/
		virtual void unloadAndDestroyAll();

		virtual void update( real timeElapsed )
		{
		}

	};

	class RawDataResourceManager;
	class YAKE_BASE_API RawDataResource : public Resource
	{
	protected:
		DataChunk				mData;
		String					mSource;
		RawDataResourceManager	* mResMgr;
	public:
		RawDataResource( const String & name, RawDataResourceManager * resourceManager ) : Resource( name ), mSource( name ), mResMgr( resourceManager )
		{
		}
		virtual void load();
		virtual void unload()
		{
			if (!mLoaded)
				return;
			mData.reset(0);
		}
		DataChunk & getData();
		const DataChunk & getData() const;
	};

	class YAKE_BASE_API RawDataResourceManager : public ResourceManager< RawDataResource >
	{
	public:
		RawDataResourceManager()
		{
		}

		virtual ResourceType* add( const String & name, int priority = 0 )
		{
			RawDataResource* pRes = new RawDataResource( name, this );
			YAKE_ASSERT( pRes ).debug("Out of memory ?");
			if (!pRes)
				return 0;

			ResourceEntry entry;
			entry.resource_ = pRes;
			entry.priority = priority;
			mResources.insert( std::make_pair<String,ResourceEntry>(name, entry) );

			return pRes;
		}

		ErrorCode _readFile( const String & name, DataChunk & chunk )
		{
			chunk.reset( 0 );
			return eOk;
		}
	};

} // yake

#endif // YAKE_BASE_RESOURCE_H

