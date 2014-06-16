#ifndef __BLIND_HH__
#define __BLIND_HH__

#include <string>
#include <sstream>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include <atools/base64.hh>

class Blind
{
private:
  int _keyLength;
public:
  Blind( int keyLength = 10 ) : _keyLength( keyLength ) {}
  std::string blind  ( const std::string& str );
  std::string unblind( const std::string& str );
  template< class T > std::string blind  ( T value );
  template< class T > T           unblind( const std::string& data );
};



template< class T >
std::string Blind::blind( T value )
{
  // Read a random number from the random device.
  char* key = new char[ _keyLength ];
  int fd = open( "/dev/urandom", O_RDONLY );
  if ( fd == -1 )
    std::exit( 1 );
  read( fd, key, _keyLength );
  close( fd );

  char* bytes = (char*) &value;
  for( unsigned int pos = 0; pos < sizeof( T ); ++pos )
    bytes[ pos ] ^= key[ pos % _keyLength ];

  std::string output;
  std::copy( key  , key   + _keyLength , std::back_inserter( output ) );
  std::copy( bytes, bytes + sizeof( T ), std::back_inserter( output ) );

  return "B" + base64_encode( output );
}


template< class T >
T Blind::unblind( const std::string& input )
{
  if ( input[ 0 ] != 'B' )
  {
    T output;
    std::istringstream in( input );
    in >> output;
    return output;
  }

  // If the leading character just identifies a blind string, remove it.
  std::string data = base64_decode( input.substr( input.size() % 4 ) );

  std::string key;
  std::string bytes;
  std::copy( data.begin()             , data.begin() + _keyLength, std::back_inserter( key   ) );
  std::copy( data.begin() + _keyLength, data.end()               , std::back_inserter( bytes ) );

  for( unsigned int pos = 0; pos < sizeof( T ); pos++ )
    bytes[ pos ] ^= key[ pos % _keyLength ];

  return *((T*) &bytes[0]);
}

#endif

