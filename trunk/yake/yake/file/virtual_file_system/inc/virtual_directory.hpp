#ifndef _VIRTUAL_DIRECTORY_HPP_
#define _VIRTUAL_DIRECTORY_HPP_

#include <map>
#include <string>
#include <istream>

#include "virtual_archive.hpp"
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
class virtual_directory
{
public:
	typedef std::map< std::string, virtual_directory* > dir_map;
	typedef std::map< std::string, virtual_file* > file_map;


	typedef archive_file_map::iterator file_iterator;

public:
	virtual_archive() {}

	~virtual_archive() 
	{
	}

public:
	// names
	const std::string & get_dir_name() const;

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
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
	// content
	archive_file_map files_; // todo
	std::string dir_name_;

};

} // virtualfs
} // filesystem

#endif // _VIRTUAL_DIRECTORY_HPP_