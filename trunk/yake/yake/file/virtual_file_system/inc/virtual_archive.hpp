#ifndef _VIRTUAL_ARCHIVE_HPP_
#define _VIRTUAL_ARCHIVE_HPP_

#include <map>
#include <string>
#include <istream>

#include "virtual_file.hpp"
#include "config.hpp"

// we need to access the input stream of basic_text_iprimitive
#define BOOST_NO_MEMBER_TEMPLATE_FRIENDS
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/map.hpp>

namespace filesystem
{
namespace virtualfs
{

// todo add directories
class virtual_archive
{
public:
	typedef std::map< std::string, virtual_file* > archive_file_map;
	typedef archive_file_map::iterator file_iterator;

public:
	virtual_archive() : archive_(NULL) {}

	virtual_archive( in_archive * archive, const std::string & file_name )
		: archive_(archive), file_name_(file_name)
	{
		extract_archive_name();
	}

	virtual_archive( const std::string & file_name )
		: file_name_(file_name), archive_(NULL)
	{
		extract_archive_name();
	}

	~virtual_archive() 
	{
		// unload the serialization archive
		if( archive_ )
		{
			// delete input stream
			delete &( archive_->is );
			// delete archive
			delete archive_;
		}

		// unload the files
		for( archive_file_map::iterator iter( files_.begin() );
			iter != files_.end();
			iter++ )
		{
			delete iter->second;
		}
	}

private:
	void extract_archive_name()
	{
		// get archive file without extension
    archive_name_ = file_name_;
	  archive_name_.resize( archive_name_.length() - std::string( "." + archive_id ).length() );
	  archive_name_.erase( 0, archive_name_.rfind( "/" ) + 1 );
	}

public:
	// names
	const std::string & get_file_name() const;
	const std::string & get_archive_name() const;

	// files
	bool contains_file( const std::string & file_name );

	file_iterator begin()
	{
		return files_.begin();
	}

	file_iterator end()
	{
		return files_.end();
	}

	// data offsets
	std::istream::pos_type get_data_offset() const
	{
		return data_offset_;    
	}

private:
	// serialization
	friend class virtual_file_system;
	void add_virtual_file_from( const path & ph );
	virtual_file & load_file_from_stream( const std::string & file_name );

	// boost serialization
  friend class boost::serialization::access;
  template<class Archive>
  void save(Archive & ar, const unsigned int version) const
  {
    // stream headers
		ar & files_;
		// stream data block
		for( archive_file_map::const_iterator iter = files_.begin();
			iter != files_.end();
			iter++ )
		{
      iter->second->save_data( ar, version );
		}
  }

  template<class Archive>
  void load(Archive & ar, const unsigned int version)
  {
    // stream headers
		ar & files_;
		// the stream position after the file headers must be the data offset
		data_offset_ = ar.is.tellg();

		// TODO DOES THIS WORK FOR VIRTUAL BUFFER?
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
	// content
	archive_file_map files_;
	std::istream::pos_type data_offset_; // todo needed by virtual_file if we want to open more than one file

	// archive file
	std::string file_name_;
	std::string archive_name_;

	// store archive
	in_archive * archive_;
};

} // virtualfs
} // filesystem

#endif // _VIRTUAL_ARCHIVE_HPP_