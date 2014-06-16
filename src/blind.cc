#include <fstream>

#include <atools/base64.hh>
#include <atools/blind.hh>


std::string Blind::blind( const std::string& input )
{
  // Read a random number from the random device.
  std::ifstream random( "/dev/urandom" );
  char* rnd = new char[ _keyLength ];
  random.read( rnd, _keyLength );
  random.close();

  // Define the key as this random number. Cannot do key = std::string( rnd ), since
  //    rnd may contain the null character.
  std::string key;
  key.resize( _keyLength );
  for ( int i = 0; i < _keyLength; i++ )
    key[ i ] = rnd[ i ];
  delete[] rnd;

  // Concatenate the key with the message.
  std::string output = key + input;

  // Blind the message but not the key.
  for ( unsigned int i = _keyLength; i < output.size(); ++i )
    output[ i ] ^= key[ i % _keyLength ];

  // Return the value in base 64.
  return base64_encode( output );
}


std::string Blind::unblind( const std::string& input )
{
  // Decode the value in base 64.
  // If the leading character just identifies a blind string, remove it.
  std::string output = base64_decode( input.substr( input.size() % 4 ) );

  // Retrieve the key and the blinded message from the input.
  std::string key    = output.substr( 0, _keyLength );
  std::string result = output.substr( _keyLength );

  // Unblind the message.
  for ( unsigned int i = 0; i < result.size(); ++i )
    result[ i ] ^= key[ i % _keyLength ];

  return result;
}
