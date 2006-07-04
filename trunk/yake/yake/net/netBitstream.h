#ifndef NET_BITSTREAM_H
#define NET_BITSTREAM_H

#include "netPrerequisites.h"

template<typename T>
struct type_bit_size
{
	static const size_t size = sizeof(T)*8;
};

namespace net {

	/** Generic output bitstream.
		@note When using it has to be specialized for a sink type.
		@note It outputs data in single bytes via the specified sink.
		@todo Make more generic to support different sizes of underlying storage primitives (not just uint8).
		@sa ibitstream_base
	*/
	template<typename DataSinkType>
	struct obitstream_base
	{
		typedef DataSinkType sink_type;

		obitstream_base(sink_type* sink = 0) :
			sink_(sink),
			byte_(0),
			bitPos_(0),
			totalBits_(0)
		{}
		/** Specify a different sink for receiving the storage primitives.
			@note Do always flush the stream before changing to a different sink!
			@note If you want to reset the total bit counter then call reset()
				before setSink().
		*/
		void setSink(sink_type* sink)
		{
			flush();
			sink_ = sink;
		}
		/** Write a single bit into the stream. */
		void write(const bool bit)
		{
			assert(sink_);
			byte_ |= ((bit?1:0) << bitPos_++);

			assert(bitPos_ <= 8);
			if (bitPos_==8)
				flushByte();
		}
		/** Write any number of bits into the stream.
			@note The implementation takes care to use special algorithms to improve
				performance when possible (e.g. for byte aligned writes).
		*/
		void write(const void* data, const size_t numBits)
		{
			const uint8* ps = reinterpret_cast<const uint8*>(data);
			assert(ps);
			assert(numBits>0);
			assert(sink_);

			assert( bitPos_ <= 7 );
			uint8 leftShift = uint8(bitPos_ & 0x7); // aka upShift
			uint8 rightShift = 8 - leftShift; // aka downShift

			// if numBits < 8 and if it fits into the current byte_:
			//if (rightShift >= numBits)
			if (numBits + bitPos_ <= 8)
			{
				assert(numBits<=8);
				uint8 mask = ((1 << uint8(numBits))-1); // used to mask 'source' byte
				byte_ |= ((*ps & mask) << leftShift) & 0xff;
				bitPos_ += numBits;
				flushByte();
				return;
			}

			// byte aligned writes
			if (bitPos_ == 0)
			{
				// copy full bytes
				size_t leftOver = bitPos_ + numBits;
				for (; leftOver>=8; leftOver-=8)
				{
					byte_ = *ps++ & 0xff;
					*sink_ << byte_;
					totalBits_ += 8; // Have to do this as we don't use flushByte() here!
				}
				byte_ = 0;
				// copy remaining bits (if any)
				if (leftOver)
				{
					const uint8 mask = (1 << uint8(leftOver)) - 1;
					byte_ |= (*ps & mask) & 0xff;
					bitPos_ += leftOver;
					flushByte();
				}
				return;
			}

			// non byte aligned writes
			size_t leftOver = numBits;
			for (; leftOver>=8; leftOver-=8)
			{
				// write first bunch of bits
				uint8 mask = ((1 << rightShift)-1); //@todo cache this?
				// mask to be used for source!


				byte_ |= ((*ps & mask) << leftShift) & 0xff;

				bitPos_ += rightShift;

				// flush byte
				assert( bitPos_ == 8 );
				flushByte();

				// write second bunch of bits
				mask = ~mask;
				byte_ |= ((*ps & mask) >> rightShift) & 0xff;
				
				bitPos_ += leftShift;

				++ps;
			}

			// copy remaining bits (if any)
			if (leftOver <= rightShift) // fits into remaining bits
			{
				assert(leftOver<8);
				uint8 mask = ((1<<uint8(leftOver))-1) << uint8(bitPos_);
				byte_ |= ((*ps & mask) & 0xff); //@todo verify: is that here working correctly?
				bitPos_ += leftOver;
			}
			else // write remaining bits into current and next byte
			{
				assert(leftOver <= 8);
				// write first bunch of bits into remaining (=rightShift) bits of destination byte
				uint8 mask = ((1 << rightShift)-1);
				byte_ |= ((*ps & mask ) << leftShift) & 0xff;

				bitPos_ += rightShift;
				leftOver -= rightShift;

				// flush byte
				assert( bitPos_ == 8 );
				flushByte();

				// write remaining bits (if any)
				if (leftOver)
				{
					assert(leftOver<8);
					uint8 mask = ((1<<uint8(leftOver))-1) << rightShift;
					byte_ |= ((*ps & mask) >> rightShift) & 0xff;
					bitPos_ += leftOver;
				}
			}
			assert( bitPos_ <= 8 );
			flushByte();
		}
		/** Flush any remaining cached bits into the sink.
			@note This fills the remaining bits up with 0's.
			@sa setSink()
		*/
		void flush()
		{
			if (bitPos_)
			{
				assert( sink_ );
				*sink_ << byte_;
				totalBits_ += bitPos_;
				bitPos_ = 0;
				byte_ = 0;
			}
		}
		/** Return the total number of bits (!) written into the stream.
		*/
		size_t size() const
		{
			return totalBits_;
		}
		/** Flushes and resets the stream as well as the count of total bits written.
			@note Use this if you want to reuse the bit stream for a new set of writing operations.
		*/
		void reset()
		{
			flush();
			totalBits_ = 0;
		}
		/*
		template<typename T>
		void write(const T value, const size_t numBits = type_bit_size<T>::size)
		{
			assert( numBits <= type_bit_size<T>::size );
			this->write(&value,numBits);
		}
		*/
		void write(const float value, const size_t numBits = type_bit_size<float>::size)
		{
			assert( numBits <= type_bit_size<float>::size );
			this->write(&value,numBits);
		}
		void write(const double value, const size_t numBits = type_bit_size<double>::size)
		{
			assert( numBits <= type_bit_size<double>::size );
			this->write(&value,numBits);
		}
		void write(const char value, const size_t numBits = type_bit_size<char>::size)
		{
			assert( numBits <= type_bit_size<char>::size );
			this->write(&value,numBits);
		}
		void write(const wchar_t value, const size_t numBits = type_bit_size<wchar_t>::size)
		{
			assert( numBits <= type_bit_size<wchar_t>::size );
			this->write(&value,numBits);
		}
		void write(const uint16 value, const size_t numBits = type_bit_size<uint16>::size)
		{
			assert( numBits <= type_bit_size<int16>::size );
			this->write(&value,numBits);
		}
		void write(const int16 value, const size_t numBits = type_bit_size<int16>::size)
		{
			assert( numBits <= type_bit_size<int16>::size );
			this->write(&value,numBits);
		}
		void write(const uint32 value, const size_t numBits = type_bit_size<uint32>::size)
		{
			assert( numBits <= type_bit_size<int32>::size );
			this->write(&value,numBits);
		}
		void write(const int32 value, const size_t numBits = type_bit_size<int32>::size)
		{
			assert( numBits <= type_bit_size<int32>::size );
			this->write(&value,numBits);
		}
	private:
		obitstream_base(const obitstream_base&);
		obitstream_base& operator=(const obitstream_base&);
	private:
		void flushByte()
		{
			assert( bitPos_ <= 8 );
			if (bitPos_ == 8)
			{
				assert(sink_);
				*sink_ << byte_;
				byte_ = 0;
				totalBits_ += bitPos_;
				bitPos_ = 0;
			}
		}
	private:
		sink_type*			sink_;
		uint8				byte_;
		size_t				bitPos_;
		size_t				totalBits_;
	};

	/** Generic input bit stream.
		@note It has to be specialized for a specific byte source to be used in client code.
		@todo Make more generic to support different storage primitives (not just uint8 streams).
		@sa obitstream_base
	*/
	template<typename DataSourceType>
	struct ibitstream_base
	{
		typedef DataSourceType source_type;

		ibitstream_base(source_type* source = 0) :
			source_(source),
			byte_(0),
			bitPos_(8),
			totalBitsRead_(0)
		{}
		void setSource(source_type* source)
		{
			source_ = source;
		}

		/** Reads a single bit from the stream and returns it in "bit".
			@return Number of bits read, i.e. 1 or 0 (if an error occurred).
		*/
		size_t read(bool& bit)
		{
			assert(source_);
			if (!updateByte())
				return 0;
			assert( bitPos_ < 8 );
			const uint8 mask = (1 << uint8(bitPos_++));
			bit = (byte_ & mask) != 0;
			++totalBitsRead_;
			return 1;
		}
	private:
		bool updateByte()
		{
			assert(bitPos_<=8);
			if (bitPos_ == 8)
			{
				if (!readByteFromSource())
				{
					assert("out of data" && 0);
					return false;
				}
			}
			return true;
		}
	public:
		/** Reads the specified number of bits into the specified buffer.
			@return The number of bits read. If all goes well it should be equal to
				numBits. If it is not then an error occurred, e.g. the stream (source)
				could not deliver any more data.
			@note This methods takes care of using optimized implementations for, for example,
				byte aligned reads.
		*/
		size_t read(void* data, const size_t numBits)
		{
			assert(data);
			assert(bitPos_ <= 8);
			if (!updateByte())
				return 0;
			assert(bitPos_ < 8);

			uint8* pd = (uint8*)data;
			const uint8 leftShift = uint8(bitPos_) & 0x7;
			const uint8 rightShift = 8 - leftShift;

			// case: requested bits are completely within the cached byte
			if (numBits + bitPos_ <= 8)
			{
				const uint8 mask = ((1 << uint8(numBits))-1) << leftShift;
				*pd |= (byte_ & mask) >> leftShift;
				bitPos_ += numBits;
				totalBitsRead_ += numBits;
				return numBits;
			}

			// case: requested bits start byte-aligned
			size_t bitsRead = 0;
			if (!leftShift)
			{
				// read full bytes
				size_t leftOver = bitPos_ + numBits;
				while (leftOver>=8)
				{
					*pd++ = byte_;
					bitsRead += 8;
					leftOver-=8;
					bitPos_+= 8;
					if (leftOver)
					{
						if (!updateByte())
						{
							assert(0 && "failed to read");
							totalBitsRead_ += bitsRead;
							return bitsRead;
						}
					}
				}
				//byte_ = 0;
				// read remaining bits (if any)
				if (leftOver)
				{
					if (!updateByte())
					{
						assert(0 && "failed to read");
						totalBitsRead_ += bitsRead;
						return bitsRead;
					}
					const uint8 mask = (1 << uint8(leftOver)) - 1;

					//++pd;

					*pd |= (byte_ & mask);

					bitsRead += leftOver;
					bitPos_ += leftOver;
				}
				assert( bitsRead == numBits );
				totalBitsRead_ += bitsRead;
				return bitsRead;
			}

			// case: non-byte-aligned
			size_t leftOver = numBits;
			for (; leftOver>=8; leftOver-=8)
			{
				// read first bits
				uint8 mask = ((1<<rightShift)-1) << leftShift;
				*pd |= (byte_ & mask) >> leftShift; // NB Set to = instead of |= if the destination
													// should be cleared (set to 0's).
				bitPos_ += rightShift;
				bitsRead += rightShift;

				// request next byte to read from
				if (!updateByte())
				{
					assert(0 && "failed to read");
					totalBitsRead_ += bitsRead;
					return bitsRead;
				}

				// read second bunch of bits
				mask = (~mask);

				*pd |= (byte_ & mask) << rightShift;

				bitPos_ += leftShift;
				bitsRead += leftShift;

				++pd;
			}

			// remaining bits (if any)
			if (leftOver <= rightShift) // fits into remaining bits
			{
				assert(leftOver<8);
				const uint8 mask = ((1<<uint8(leftOver))-1) << uint8(bitPos_);
				*pd |= (byte_ & mask);

				bitsRead += leftOver;
				bitPos_ += leftOver;
			}
			else // read remaining bits of current and next byte into a single destination byte
			{
				// read first bunch of bits (current byte)
				uint8 mask = ((1<<rightShift)-1) << leftShift;
				*pd |= (byte_ & mask) >> leftShift; // NB Use = instead of |= if the destination
													// should be cleared (set to 0's by default)!

				bitPos_ += rightShift;
				bitsRead += rightShift;
				leftOver -= rightShift;

				// get next byte
				assert( bitPos_ == 8 );
				if (!updateByte())
				{
					assert(0 && "failed to read");
					totalBitsRead_ += bitsRead;
					return bitsRead;
				}

				// read remaining bits ("next" byte)
				if (leftOver)
				{
					assert(leftOver<8);
					uint8 mask = ((1<<uint8(leftOver))-1);
					*pd |= (byte_ & mask) << rightShift;
					bitPos_ += leftOver;
					bitsRead += leftOver;
				}
			}
			assert( bitsRead == numBits );
			totalBitsRead_ += bitsRead;
			return bitsRead;
		}
		/** Resets the ibitstream_base object for a new set of operations.
			@note Also resets the counter of "total bits read".
		*/
		void reset()
		{
			byte_ = 0;
			bitPos_ = 0;
			totalBitsRead_ = 0;
		}
		size_t size() const
		{
			return totalBitsRead_;
		}
		size_t read(float& value, const size_t numBits = type_bit_size<float>::size)
		{
			assert( numBits <= type_bit_size<float>::size );
			return this->read(&value,numBits);
		}
		size_t read(double& value, const size_t numBits = type_bit_size<double>::size)
		{
			assert( numBits <= type_bit_size<double>::size );
			return this->read(&value,numBits);
		}
		size_t read(char& value, const size_t numBits = type_bit_size<char>::size)
		{
			assert( numBits <= type_bit_size<char>::size );
			return this->read(&value,numBits);
		}
		size_t read(wchar_t& value, const size_t numBits = type_bit_size<wchar_t>::size)
		{
			assert( numBits <= type_bit_size<wchar_t>::size );
			return this->read(&value,numBits);
		}
		size_t read(uint8& value, const size_t numBits = type_bit_size<uint8>::size)
		{
			assert( numBits <= type_bit_size<uint8>::size );
			return this->read(&value,numBits);
		}
		size_t read(int8& value, const size_t numBits = type_bit_size<int8>::size)
		{
			assert( numBits <= type_bit_size<int8>::size );
			return this->read(&value,numBits);
		}
		size_t read(uint16& value, const size_t numBits = type_bit_size<uint16>::size)
		{
			assert( numBits <= type_bit_size<uint16>::size );
			return this->read(&value,numBits);
		}
		size_t read(int16& value, const size_t numBits = type_bit_size<int16>::size)
		{
			assert( numBits <= type_bit_size<int16>::size );
			return this->read(&value,numBits);
		}
		size_t read(uint32& value, const size_t numBits = type_bit_size<uint32>::size)
		{
			assert( numBits <= type_bit_size<uint32>::size );
			return this->read(&value,numBits);
		}
		size_t read(int32& value, const size_t numBits = type_bit_size<int32>::size)
		{
			assert( numBits <= type_bit_size<int32>::size );
			return this->read(&value,numBits);
		}
	private:
		bool readByteFromSource()
		{
			assert(source_);
			*source_ >> byte_;
			bitPos_ = 0;
			return true;
		}
	private:
		source_type*		source_;
		uint8				byte_;
		size_t				bitPos_;
		size_t				totalBitsRead_;
	};

} // namespace net

#include "netBitStream.inl"

#endif
