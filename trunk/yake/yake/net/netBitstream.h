#ifndef NET_BITSTREAM_H
#define NET_BITSTREAM_H

#include <bitset>

namespace net {

	/**
		\todo - optimize: use bit masks ? => have to profile this. newer processor
					architectures aren't that fast with bit operations...
				- optimize: remove casts
	*/
	template<typename DataStreamType>
	class ibstream_base
	{
	public:
		ibstream_base& operator = (const ibstream_base&);
		ibstream_base(const ibstream_base&);
		ibstream_base( DataStreamType* pds = 0 );
		~ibstream_base();
		void setDataSource( DataStreamType*, const bool takeOwnership = false );
		uint8 read( bool& );
		uint8 read( void* pd, size_t numBits );
		uint8 read( uint8&, uint8 numBits );
		uint8 read( float&, size_t numBits );
	private:
		bool _inByte( uint8& );
		DataStreamType*	mSource;
		uint8					mBitPos;
		std::bitset<8>		mBuf;
		size_t				mTotalBits;
		size_t				mTotalBytes;
		bool					mDestroyDataStream;
	};

	template<class DataStreamType>
	class obstream_base
	{
	public:
		obstream_base& operator = (const obstream_base&);
		obstream_base(const obstream_base&);
		obstream_base( DataStreamType* pds = 0 );
		~obstream_base();
		void setDataSink( DataStreamType* s, const bool takeOwnership = false );
		uint8 write( const uint8, uint8 numBits );
		void write( const bool );
		void write( const void*, size_t numBits );

		void write( const uint16, uint8 numBits );
		void write( const uint32, uint8 numBits );
		void write( const float, uint8 numBits );
		void write( const double, uint8 numBits );
		void flush();

		size_t numBits() const;
		//size_t numBytes() const; //pre or post flush?

		void setNumBits(const uint8 numBitsForNextStreamOperations);
		obstream_base& operator << (const uint8);
		obstream_base& operator << (const uint16);
		obstream_base& operator << (const uint32);
		obstream_base& operator << (const int8);
		obstream_base& operator << (const int16);
		obstream_base& operator << (const int32);
		obstream_base& operator << (const float);
		obstream_base& operator << (const double);
	private:
		void _outByte( uint8 );
		//std::ostream*	mStream;
		DataStreamType*	mStream;
		uint8					mBitPos;
		std::bitset<8>		mBuf;
		size_t				mTotalBits;
		size_t				mTotalBytes;
		uint8					mNumBitsForNextStreamOperations;
		bool					mDestroyDataStream;
	};

	struct setNumBits
	{
		setNumBits(uint8 numBits) : b(numBits)
		{}
		uint8 b;
	};
	template<class ostream_type>
	inline ostream_type& operator << (ostream_type& lhs, const setNumBits& rhs)
	{
		lhs.setNumBits(rhs.b);
		return lhs;
	}
} // namespace net

#include "netBitStream.inl"

#endif
