#include <iostream>
#include <string>
#include <vector>
#include <boost/io/stream_facade.hpp>
#include <boost/io/categories.hpp>

struct seekable_ressource 
{
    typedef char char_type;
    typedef boost::io::seekable_resource_tag category;

    seekable_ressource( const char * buffer, int count )
      : position_(0)
		{
      for( int i = 0; i < count; i++ )
        data_.push_back( buffer[i] );
    }

    std::streamsize read( char * s, std::streamsize n ) 
    {
      // Reads up to n characters from the controlled 
      // sequence into the buffer s, returning the number 
      // of characters read. Returning a value less than n 
      // indicates end-of-sequence.			
      int bytes_to_read, bytes_to_read_return;
      bytes_to_read = bytes_to_read_return =
        ( data_.size() - position_  > n ?
          n : data_.size() - position_ );

      for( ; bytes_to_read-- > 0; position_++ )
        s[position_] = data_[position_];

      return bytes_to_read_return;
    }

    void write( const char* s, std::streamsize n ) 
    {
      // Writes n characters from the buffer s to the 
      // controlled sequence.
      throw exception();
    }

    std::streamoff seek( std::streamoff off, std::ios_base::seekdir way ) 
    {
      // Advances the read/write head by off characters, 
      // returning the new position, where the offset is 
      // calculated from:
      //  - the start of the sequence if way == ios_base::beg
      //  - the current position if way == ios_base::cur
      //  - the end of the sequence if way == ios_base::end
      if( way == std::ios_base::beg )
      {
        position_ = static_cast<int>( off ); 
      }
      else if( way == std::ios_base::cur )
      {
        position_ += static_cast<int>( off );           
      }
      else if( way == std::ios_base::end )
      {
        position_ = data_.size() - static_cast<int>( off ); 
      }
      return position_;
    }

private:
  std::vector<char> data_;
  int position_;
};

typedef boost::io::streambuf_facade<seekable_ressource> filebuf;

void seekable_file_test()
{
    // seekable buffer and std stream
    const char * h = "Hello World!";
    int length = std::strlen( h );
    filebuf in( h, length );
    std::istream ins( &in );

    {
      // just read
      std::string hello;
      std::getline( ins, hello );
      std::cout << hello << std::endl;
      assert( hello == h );
    }

    {
      // set position back to bof
      std::string hello;
      ins.seekg( 0, std::ios_base::beg );
      assert( static_cast<int>( ins.tellg() ) == 0 );
      // clear eof, get data
      ins.clear();
      std::getline( ins, hello );
      std::cout << hello << std::endl;
      assert( hello == h );
    }
}

int main()
{
  seekable_file_test();
  std::cin.get();
  return 0;
}