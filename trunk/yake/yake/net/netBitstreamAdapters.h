#ifndef NET_BITSTREAMADAPTERS_H
#define NET_BITSTREAMADAPTERS_H

namespace net {

	template<class ctr_type>
	struct bitstream_sink_stlcontainer
	{
	private:
		bitstream_sink_stlcontainer();
	public:
		typedef ctr_type container_type;
		bitstream_sink_stlcontainer(ctr_type& ctr) : ctr_(ctr) {}
		void write(const net::uint8 c)
		{
			ctr_.push_back( c );
		}
		ctr_type&	ctr_;
	};
	typedef bitstream_sink_stlcontainer<std::vector<net::uint8> > bitstream_stlvector_sink;
	typedef net::obstream_base<bitstream_stlvector_sink> obitstream_vector;

	template<class ctr_type>
	struct bitstream_source_stlcontainer
	{
	private:
		bitstream_source_stlcontainer();
	public:
		typedef ctr_type container_type;
		bitstream_source_stlcontainer(const container_type& ctr) : currPos(ctr.begin()), itEnd(ctr.end()), ctr_(ctr)
		{}
		bool hasMoreData() const
		{
			return currPos != itEnd;
		}
		bool read(net::uint8& c) const
		{
			if (!hasMoreData())
				return false;
			c = *currPos++;
			return true;
		}
	private:
		mutable typename container_type::const_iterator currPos;
		mutable typename container_type::const_iterator itEnd;
		const container_type&	ctr_;
	};
	typedef bitstream_source_stlcontainer<std::vector<net::uint8> > bitstream_stlvector_source;
	typedef net::ibstream_base<bitstream_stlvector_source> ibitstream_vector;

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
	typedef net::ibstream_base<bitstream_source_voidptr> ibitstream_voidptr;
} // namespace net

#endif
