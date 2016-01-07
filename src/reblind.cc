
#include <iostream>

#include <atools/blind.hh>
#include <rtools/rblind.hh>

#include <root/RooUnblindOffset.h>
#include <root/RooUnblindUniform.h>


// Convert a root blind value into a randomized blind value.
int main( int argc, char** argv )
{
  std::string blindStr = "default blinding string";
  double      scale    = 0.02;
  std::string blindTyp = "uniform";
  if ( argc > 2 ) blindStr = argv[ 2 ];
  if ( argc > 3 ) scale    = atof( argv[ 3 ] );
  if ( argc > 4 ) blindTyp = argv[ 4 ];

  RBlind rblinder( blindStr, scale );
  Blind  blinder( 4 );

  if ( blindTyp == "uniform" )
    std::cout << blinder.blind( rblinder.unblind< RooUnblindUniform >( atof( argv[ 1 ] ) ) ) << std::endl;
  else
    std::cout << blinder.blind( rblinder.unblind< RooUnblindOffset  >( atof( argv[ 1 ] ) ) ) << std::endl;

  return 0;
}
