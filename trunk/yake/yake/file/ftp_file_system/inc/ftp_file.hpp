#ifndef _FTP_FILE_HPP_
#define _FTP_FILE_HPP_

#include <virtualfs/inc/file_system.hpp>

#include "config.hpp"

namespace filesystem
{
namespace virtualfs
{

class ftp_file : public file
{
public:
	ftp_file( const std::string & file_name );
	~ftp_file();

public:
	const std::string & get_name() const;
	void close();
	const file_system & get_file_system() const;
	int read( unsigned char * buffer, dword bytes_to_read );
	int read_complete_file( unsigned char * buffer, dword bytes_to_read = 0 );

private:
	std::string file_name_;
	bool is_loaded_;
	nativefs::handle handle_;
};

} // virtualfs
} // filesystem

#endif // _FTP_FILE_HPP_