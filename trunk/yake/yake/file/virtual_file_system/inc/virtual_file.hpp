#ifndef _VIRTUAL_FILE_HPP_
#define _VIRTUAL_FILE_HPP_

#include <vector>

#include <virtualfs/inc/config.hpp>
#include <virtualfs/inc/file_system.hpp>

#include "config.hpp"

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>

namespace filesystem
{
namespace virtualfs
{

class virtual_archive;

static const dword chunk_size = 10 * 1024;

class virtual_file : public file
{
	typedef std::vector< byte > data;

public:
	virtual_file( const virtual_archive & ar );
	virtual_file( const path & ph, const virtual_archive & ar );
	virtual_file( const path & ph, std::ios_base::openmode mode, 
		virtual_archive & ar );
	~virtual_file();

public:
	const virtual_archive & get_archive() const;
	const std::string & get_name() const;
	void close();
	const file_system & get_file_system() const;
	int read( unsigned char * buffer, dword bytes_to_read );
	int read_complete_file( unsigned char * buffer, dword bytes_to_read = 0 );

private:
	// load
	friend class virtual_archive;
	void load_data_from_file( const path & ph );
	void load_data_from_stream( in_archive * iarchive );

	// data offsets
	void set_data_offset( dword offset );
	dword get_data_size();

	// boost serialization
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) const
  {
    // stream headers
		ar & file_name_;
		ar & data_offset_;
		ar & compressed_size_;
		ar & uncompressed_size_;
  }

  template<class Archive>
  void save_data(Archive & ar, const unsigned int version) const
  {
    // stream data size
		ar << data_.size();
		// stream data
		ar.save_binary( &( data_[0] ), data_.size() );
	}

private:
	// serialized
	virtual_archive & archive_;
	std::string file_name_;
	dword data_offset_;						// needed to load the right data chunk of the archive file
	dword compressed_size_;
	dword uncompressed_size_;
	data data_;

	// runtime data
	dword data_position_;
	bool is_loaded_;
};

} // virtualfs
} // filesystem

namespace boost 
{
namespace serialization 
{

template<class Archive>
inline void save_construct_data( 
		Archive & ar, 
		const filesystem::virtualfs::virtual_file * t, 
		const unsigned int file_version )
{
    ar << t->get_name();
    ar << & ( t->get_archive() );
}

template<class Archive>
inline void load_construct_data(
		Archive & ar, 
		filesystem::virtualfs::virtual_file * t, 
		const unsigned int file_version )
{
    std::string file_name;
    ar >> file_name;

    filesystem::virtualfs::virtual_archive * archive;
    ar >> archive;

    ::new(t)filesystem::virtualfs::virtual_file( file_name, *archive );
}

} // serialization
} // boost

#endif // _VIRTUAL_FILE_HPP_