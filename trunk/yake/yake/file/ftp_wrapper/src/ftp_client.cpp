#include <inc/ftp_client.hpp>

namespace filesystem
{
namespace virtualfs
{

namespace
{
	// library stuff
	static FTPLibraryInfo lib;
	static bool lib_inited = false;

	void init()
	{
		 // init library
		if( !lib_inited )
		{
			InitWinsock();

			if( FTPInitLibrary(&lib) < 0 ) 
			{
        std::cout << "err: FTPInitLibrary" << std::endl;
				DisposeWinsock();
				throw exception();
			}

			lib_inited = true;
		}
	}
} // unnamed

ftp_client::ftp_client()
	: is_connected_(false)
{
	 // init library
	init();
}

ftp_client::ftp_client( const std::string & url )
	: url_(url), is_connected_(false)
{
	// init library
	init();
	// connect
	connect( url );
}

ftp_client::~ftp_client()
{
	disconnect();
}

bool ftp_client::connect( const std::string & url )
{
	if( is_connected_ )
		disconnect();

	// store url
	url_ = url;

   // build connection info
	if( FTPInitConnectionInfo( &lib, &fi_, kDefaultFTPBufSize ) < 0 ) 
	{
		std::cout << "err: FTPInitConnectionInfo" << std::endl;
		DisposeWinsock();
		return false;
	}
	fi_.dataPortMode = kFallBackToSendPortMode;
	fi_.debugLog = NULL;
	fi_.errLog = stderr;
	fi_.xferTimeout = 60 * 60;
	fi_.connTimeout = 30;
	fi_.ctrlTimeout = 135;
	fi_.host[0] = '\0';

	// build url
	FTPLineList cdlist;
	std::string urlfile;
	FTPDecodeURL(&fi_, &(url_[0]), &cdlist, &(urlfile[0]), urlfile.size(), (int *) 0, NULL);

	// connect to host
	if( FTPOpenHost(&fi_) < 0 ) 
	{
		std::cout << "err: FTPOpenHost: " << FTPStrError(fi_.errNo) << std::endl;
		DisposeWinsock();
		return false;
	}

	return is_connected_ = true;
}

void ftp_client::disconnect()
{
	if( is_connected_ )
	{
		FTPCloseHost(&fi_);
		is_connected_ = false;
	}
}

void ftp_client::list_files( 
	const std::string & dir, 
	file_list & list )
{
	if( !is_connected_ )
		throw exception();

	// change to the directory
	change_dir( dir );

	// get filelist
	FTPLineList fileList;
	if( FTPListToMemory(&fi_, ".", &fileList, "") < 0 )
	{
		std::cout << "err: FTPList" << std::endl;
		DisposeWinsock();
		throw exception();
	}

	// add file to list and do not add directories
	for ( FTPLinePtr lp = fileList.first; lp != NULL; lp = lp->next)
	{
		if( FTPIsDir( &fi_, lp->line ) == 0 )
			list.push_back( lp->line );
	}
}

bool ftp_client::exists( const std::string & file_name )
{
	return FTPFileExists( &fi_, file_name.c_str() ) == 0;
}

void ftp_client::get_file( 
	const std::string & src_file_name,
	const std::string & des_file_name )
{
	if( FTPGetOneFile2(&fi_, src_file_name.c_str(), des_file_name.c_str(), 
		kTypeBinary, -1, kResumeYes, kAppendNo) < 0 )
	{
		std::cout << "err: FTPGetOneFile2" << std::endl;
		DisposeWinsock();
		throw exception();
	}
}

void ftp_client::change_dir( const std::string & new_path )
{
	if( ( new_path ) == current_dir_ ) return;
  FTPChdir( &fi_, new_path.c_str() );
	current_dir_ = new_path;
}

void ftp_client::change_to_root()
{
  FTPChdir( &fi_, "/" );
	current_dir_ = "";	
}

bool ftp_client::is_connected() const
{
  return is_connected_;
}

} // virtualfs
} // filesystem