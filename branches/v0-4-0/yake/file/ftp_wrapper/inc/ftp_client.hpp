#ifndef _FTP_CLIENT_HPP_
#define _FTP_CLIENT_HPP_

#include <iostream>
#include <vector>
#include <string>

// ncftp
#include "syshdrs.h"

// undef ncftp defines, because they are interfering with the
// our headers
#undef open
#undef read
#undef close


namespace filesystem
{
namespace virtualfs
{

typedef std::vector< std::string > file_list;

class ftp_client
{
public:
	ftp_client();
	ftp_client( const std::string & url );
	~ftp_client();

public:
	bool connect( const std::string & url );

	void disconnect();

	void list_files( 
		const std::string & dir, 
		file_list & list );

	bool exists( const std::string & file_name );

	void get_file( 
		const std::string & src_file_name,
		const std::string & des_file_name );

	void change_dir( const std::string & new_path );

	void change_to_root();

	bool is_connected() const; 

private:
	std::string url_;
	FTPConnectionInfo fi_;
	bool is_connected_;
	std::string current_dir_;
};

} // virtualfs
} // filesystem

#endif // _FTP_CLIENT_HPP_