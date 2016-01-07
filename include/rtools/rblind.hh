#ifndef __RBLIND_HH__
#define __RBLIND_HH__

#include <string>

#include <root/RooRealVar.h>


class RBlind
{
private:
  std::string _blindStr;
  double      _scale;
public:
  RBlind( const std::string& blindStr, const double& scale )
    : _blindStr( blindStr ), _scale( scale )
    {}
  template< class T >
  const double unblind( const double& blindVal );
};


template< class T >
const double RBlind::unblind( const double& blindVal )
{
  // Unblind the blind value.
  RooRealVar blind  ( "blind"  , "blind"  , blindVal );
  T          unblind( "unblind", "unblind", _blindStr.data(), _scale, blind );

  // Return the hidden value.
  return unblind.getHiddenVal();
}


#endif

