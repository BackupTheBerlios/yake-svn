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
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <yake/base/yakePCH.h>
#include <yake/base/yakeDataChunk.h>

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

	DataChunk::DataChunk(uint32 size, uint8 * pData /* = 0  */) : mData( pData ), mSize( size )
	{
		if (!mData && mSize > 0)
		{
			mData = new uint8[ mSize ];
		}
	}

	size_t DataChunk::reset( uint32 size, uint8 * pData /*= 0*/ )
	{
		if (mData)
		{
			delete [] mData;
			mData = 0;
		}
		mSize = size;
		if (mSize > 0)
			mData = new uint8[ mSize ];
		return mSize;
	}

	DataChunk::~DataChunk()
	{
		if (mData)
			delete [] mData;
	}

	size_t DataChunk::getSize() const
	{
		return mSize;
	}

	size_t DataChunk::read( uint8 * buf, size_t len )
	{
		if (mPos >= mSize)
			return 0;
		size_t bytesToRead = len;
		if (mPos + bytesToRead > mSize)
			bytesToRead = mSize - mPos;
		memcpy( buf, mData, bytesToRead );
		return bytesToRead;
	}

	size_t DataChunk::write( const uint8 * buf, size_t len )
	{
		if (mPos >= mSize)
			return 0;
		size_t bytesToWrite = len;
		if (mPos + bytesToWrite > mSize)
			bytesToWrite = mSize - mPos;
		memcpy( mData, buf, bytesToWrite );
		return bytesToWrite;
	}

	size_t DataChunk::seek( size_t pos, bool bFromFront/* = true */)
	{
		if (bFromFront)
		{
			if (pos > mSize)
				mPos = mSize;
			else
				mPos = pos;
		}
		else
		{
			if (mSize-pos < 0)
				mPos = 0;
			else
				mPos = mSize-pos;
		}
		return mPos;
	}

	bool DataChunk::isEOF() const
	{
		return (mPos == mSize);
	}

	uint8* DataChunk::getDataPtr()
	{
		return mData;
	}

	const uint8* DataChunk::getDataPtr() const
	{
		return mData;
	}

} // yake
