#ifndef NET_BITSTREAM_INL
#define NET_BITSTREAM_INL

namespace net {

	//--------------------------------------------------------------------------
	// Class: ibstream
	//--------------------------------------------------------------------------
	template<class DataStreamType>
	ibstream_base<DataStreamType>& ibstream_base<DataStreamType>::operator = (const ibstream_base<DataStreamType>& other)
	{
		mSource			= other.mSource;
		mBitPos			= other.mBitPos;
		mBuf				= other.mBuf;
		mTotalBits		= other.mTotalBits;
		mTotalBytes		= other.mTotalBytes;
		mDestroyDataStream = other.mDestroyDataStream;
		return *this;
	}
	template<class DataStreamType>
	ibstream_base<DataStreamType>::ibstream_base(const ibstream_base<DataStreamType>& other)
	{
		mSource			= other.mSource;
		mBitPos			= other.mBitPos;
		mBuf				= other.mBuf;
		mTotalBits		= other.mTotalBits;
		mTotalBytes		= other.mTotalBytes;
		mDestroyDataStream = other.mDestroyDataStream;
	}
	template<class DataStreamType>
	ibstream_base<DataStreamType>::ibstream_base( DataStreamType* pds /*= 0*/ ) : mSource(pds), mBitPos(8), mTotalBits(0), mTotalBytes(0), mDestroyDataStream(false)
	{
		mBuf.set(0);
	}
	template<class DataStreamType>
	ibstream_base<DataStreamType>::~ibstream_base()
	{
		if (mDestroyDataStream && mSource)
			delete mSource;
	#ifdef _OUTPUT_DEBUG_STUFF
		std::cout << "ibstream: " << uint32(mTotalBits) << " bits read.\n";
		std::cout << "ibstream: " << uint32(mTotalBytes) << " bytes read.\n";
	#endif
	}
	template<class DataStreamType>
	void ibstream_base<DataStreamType>::setDataSource( DataStreamType* s, const bool takeOwnership )
	{
		if (mDestroyDataStream && mSource)
			delete mSource;
		mDestroyDataStream = takeOwnership;
		mSource = s;
	}
	template<class DataStreamType>
	bool ibstream_base<DataStreamType>::_inByte( uint8& c )
	{
		if (!mSource)
			return false;
		if (!mSource->hasMoreData())
			return false;
		if (!mSource->read( c ))
			return false;
		++mTotalBytes;
		return true;
	}
	template<class DataStreamType>
	uint8 ibstream_base<DataStreamType>::read( uint8& bits, uint8 numBits )
	{
		if (!mSource)
			return 0;
		if (numBits > 8)
			numBits = 8;

		std::bitset<8> d(0);
		//for (uint8 i = numBits-1; i != 0xff; i--)
		for (uint8 i = 0; i < numBits; ++i)
		{
			bool b;
			read(b);
			d[i] = b;
	#ifdef _OUTPUT_DEBUG_STUFF
			std::cout << "reading bit " << uint32(i) << " = " << d[i] << "\n";
	#endif
			mTotalBits++;
		}

		bits = uint8(d.to_ulong() & 0xFF);

		return numBits;
	}
	template<class DataStreamType>
	uint8 ibstream_base<DataStreamType>::read( bool& bit )
	{
		if (mBitPos == 8)
		{
			uint8 c;
			_inByte( c );
			mBuf = c;
			mBitPos = 0;
		}

		bit = mBuf[mBitPos++];
		return 1;
	}
	template<class DataStreamType>
	uint8 ibstream_base<DataStreamType>::read( void* pd, size_t numBits )
	{
		uint8* pc = static_cast<uint8*>(pd);
		uint8* pe = pc+(numBits >> 3);
		if (pc == pe)
		{
			read( *pc, uint8(numBits) );
			return uint8(numBits);
		}

		size_t toread = numBits;
		while (pc != pe)
			toread -= read( *pc++, (toread > 8) ? 8 : uint8(toread) );

		if (toread > 0)
			read( *pc, uint8(toread) );

		return uint8(numBits);
	}
	template<class DataStreamType>
	uint8 ibstream_base<DataStreamType>::read( float& d, size_t numBits )
	{
		return read( &d, numBits );
	}

	//--------------------------------------------------------------------------
	// Class: obstream
	//--------------------------------------------------------------------------
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator = (const obstream_base<DataStreamType>& other)
	{
		mStream			= other.mStream;
		mBitPos			= other.mBitPos;
		mBuf				= other.mBuf;
		mTotalBits		= other.mTotalBits;
		mTotalBytes		= other.mTotalBytes;
		mDestroyDataStream = other.mDestroyDataStream;
		mNumBitsForNextStreamOperations = other.mNumBitsForNextStreamOperations;
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>::obstream_base(const obstream_base<DataStreamType>& other)
	{
		mStream			= other.mStream;
		mBitPos			= other.mBitPos;
		mBuf				= other.mBuf;
		mTotalBits		= other.mTotalBits;
		mTotalBytes		= other.mTotalBytes;
		mDestroyDataStream = other.mDestroyDataStream;
		mNumBitsForNextStreamOperations = other.mNumBitsForNextStreamOperations;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const uint8 rhs)
	{
		write( rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const uint16 rhs)
	{
		write( &rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const uint32 rhs)
	{
		write( &rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const int8 rhs)
	{
		write( &rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const int16 rhs)
	{
		write( &rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const int32 rhs)
	{
		write( &rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const float rhs)
	{
		write( &rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>& obstream_base<DataStreamType>::operator << (const double rhs)
	{
		write( &rhs, mNumBitsForNextStreamOperations );
		return *this;
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>::obstream_base( DataStreamType* pds /*= 0*/ ) : mStream(pds), mBitPos(0), mTotalBits(0), mTotalBytes(0), mNumBitsForNextStreamOperations(8), mDestroyDataStream(false)
	{
		mBuf.set(0);
	}
	template<class DataStreamType>
	obstream_base<DataStreamType>::~obstream_base()
	{
		if (mStream && mDestroyDataStream)
			delete mStream;
	#ifdef _OUTPUT_DEBUG_STUFF
		std::cout << "obstream: " << uint32(mTotalBits) << " bits written.\n";
		std::cout << "obstream: " << uint32(mTotalBytes) << " bytes written.\n";
	#endif
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::setNumBits(const uint8 numBitsForNextStreamOperations)
	{
		mNumBitsForNextStreamOperations = numBitsForNextStreamOperations;
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::setDataSink( DataStreamType* s, const bool takeOwnership )
	{
		if (mStream && mDestroyDataStream)
			delete mStream;
		mDestroyDataStream = takeOwnership;
		mStream = s;
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::_outByte( uint8 c )
	{
		if (!mStream)
			return;
		//*mStream << c;
		mStream->write( c );
		++mTotalBytes;
	}
	template<class DataStreamType>
	uint8 obstream_base<DataStreamType>::write( const uint8 bits, uint8 numBits )
	{
		if (!mStream)
			return 0;
		if (numBits > 8)
			numBits = 8;

		std::bitset<8> d(bits);

		//for (int8 i = numBits-1; i>=0; --i)
		for (int8 i = 0; i<numBits; ++i)
		{
	#ifdef _OUTPUT_DEBUG_STUFF
			std::cout << "writing bit " << uint32(i) << " = " << d[i] << "\n";
	#endif
			write( d[i] );
		}

		return numBits;
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::write( const bool bit )
	{
		mBuf[ mBitPos ++ ] = bit;
		if (mBitPos >= 8)
		{
			uint8 c = uint8( mBuf.to_ulong() & 0xFF );
			_outByte( c );
			mBitPos -= 8;
		}
		++mTotalBits;
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::flush()
	{
		if (mBitPos > 0)
		{
			_outByte( uint8(mBuf.to_ulong() & 0xFF) );
			mBitPos = 0;
		}
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::write( const void* pd, size_t numBits )
	{
		const uint8* pc = static_cast<const uint8*>(pd);
		const uint8* pe = pc+(numBits >> 3);
		if (pc == pe)
		{
			write( *pc, uint8(numBits) );
			return;
		}

		size_t towrite = numBits;
		while (pc != pe)
			towrite -= write( *pc++, (towrite > 8) ? 8 : uint8(towrite) );

		if (towrite > 0)
			write( *pc, uint8(towrite) );
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::write( const uint16 d, uint8 numBits )
	{
		write( &d, numBits );
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::write( const uint32 d, uint8 numBits )
	{
		write( &d, numBits );
	}
	template<class DataStreamType>
	void obstream_base<DataStreamType>::write( const float d, uint8 numBits )
	{
		write( &d, numBits );
	}
	template<class DataStreamType>
	size_t obstream_base<DataStreamType>::numBits() const
	{
		return mTotalBits;
	}

} // namespace net

#endif
