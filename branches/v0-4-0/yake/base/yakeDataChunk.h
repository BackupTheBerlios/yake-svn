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
#ifndef YAKE_BASE_DATACHUNK_H
#define YAKE_BASE_DATACHUNK_H

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
// Standard headers
#ifndef YAKE_BASE_PREREQUISITES_H
#	include "yakePrerequisites.h"
#endif
// Yake

//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================
namespace yake {

/**
*/
class YAKE_BASE_API DataChunk
{
YAKE_DECLARE_CLASS( yake::base::DataChunk )
// Class
protected:
	DataChunk( const DataChunk& );
public:
	DataChunk( uint32 size = 0, uint8* pData = 0 );
	~DataChunk();

// Methods
public:
	size_t reset( uint32 size, uint8* pData = 0 );

	size_t read( uint8* pBuffer, size_t len );
	size_t write( const uint8* pBuffer, size_t len );

	size_t seek( size_t pos, bool fromFront = true );
	bool isEOF() const;

	uint8* getDataPtr();
	const uint8* getDataPtr() const;
	size_t getSize() const;

// Data
protected:
	uint8*  mData;
	size_t	mSize;
	size_t	mPos;
};

} // yake

#endif // YAKE_BASE_DATACHUNK_H
