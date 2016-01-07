
#include <iostream>
#include <iomanip>

#include <atools/blind.hh>


// Unblind a randomized blind value.
int main( int argc, char** argv )
{
  Blind blinder( 4 );

  for ( int arg = 1; arg < argc; ++arg )
    std::cout << std::setprecision( 40 ) << blinder.unblind<double>( argv[ arg ] ) << std::endl;

  return 0;
}
