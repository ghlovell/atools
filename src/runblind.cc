
#include <iostream>
#include <iomanip>

#include <rtools/rblind.hh>

#include <root/RooUnblindOffset.h>
#include <root/RooUnblindUniform.h>


// Unblind a roofit blind value.
int main( int argc, char** argv )
{
  const double& value = atof( argv[ 1 ] );

  std::string blindStr = "default blinding string";
  double      scale    = 0.02;
  if ( argc > 2 ) blindStr = argv[ 2 ];
  if ( argc > 3 ) scale    = atof( argv[ 3 ] );

  RBlind blinder( blindStr, scale );

  std::cout << "UnblindOffset:  " << std::setprecision( 40 ) << blinder.unblind< RooUnblindOffset  >( value ) << std::endl;
  std::cout << "UnblindUniform: " << std::setprecision( 40 ) << blinder.unblind< RooUnblindUniform >( value ) << std::endl;

  return 0;
}
