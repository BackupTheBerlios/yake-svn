#ifndef NET_BITSTREAMADAPTERS_H
#define NET_BITSTREAMADAPTERS_H

#include "netPrerequisites.h"
#include "netBitstream.h"

namespace net {

	template<class ContainerType>
	struct stlsequence_data_sink
	{
		typedef ContainerType container_type;

		stlsequence_data_sink(container_type* data = 0) : data_(data)
		{}
		stlsequence_data_sink& operator << (const uint8 rhs)
		{
			assert(data_);
			for (size_t i=0; i<tmp_.size(); ++i)
			{
				assert(tmp_[i] == data_->at(i));
			}
			data_->push_back(rhs);
			tmp_.push_back(rhs);
			return *this;
		}
	private:
		container_type*	data_;
		container_type	tmp_;
	};
	typedef stlsequence_data_sink<std::vector<uint8> > ByteVectorSink;
	typedef obitstream_base<ByteVectorSink> obitstream_vector;

	template<class ContainerType>
	struct stlsequence_data_source
	{
		typedef ContainerType container_type;

		stlsequence_data_source(container_type* data = 0) : data_(data), pos_(0)
		{}
		stlsequence_data_source& operator >> (uint8& rhs)
		{
			assert(data_);
			assert(pos_ < data_->size());
			rhs = data_->at(pos_);
			++pos_;
			return *this;
		}
	private:
		container_type*	data_;
		size_t			pos_;
	};
	typedef stlsequence_data_source<std::vector<uint8> > ByteVectorSource;
	typedef ibitstream_base<ByteVectorSource> ibitstream_vector;

	struct bitstream_source_voidptr
	{
	private:
		bitstream_source_voidptr();
	public:
		bitstream_source_voidptr(const void* data, const size_t len) :
			data_(0), end_(0), curr_(0)
		{
			data_ = reinterpret_cast<const uint8*>(data);
			end_ = data_ + len;
			curr_ = const_cast<uint8*>(data_);
		}
		bool hasMoreData() const
		{
			return curr_ != end_;
		}
		bool read(net::uint8& c) const
		{
			if (!hasMoreData())
				return false;
			c = *curr_++;
			return true;
		}
	private:
		mutable size_t currPos;
		const uint8*			data_;
		const uint8*			end_;
		mutable uint8*			curr_;
	};
	typedef net::ibitstream_base<bitstream_source_voidptr> ibitstream_voidptr;
} // namespace net

#endif
