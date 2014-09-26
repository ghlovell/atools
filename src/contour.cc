
#include <utility>
#include <vector>

#include <cfit/phasespace.hh>

#include <root/TGraph.h>

#include <rtools/contour.hh>


// Establish a numerical precision to avoid the evaluation of the contours at unphysical points.
#define PRECISION ( 1.0e-8 )

// Constructor.
DalitzContour::DalitzContour( const PhaseSpace& ps, const unsigned& nPoints )
  : _nPoints  ( nPoints        ),
    _ps       ( ps             ),
    _mMother  ( ps.mMother()   ),
    _m1       ( ps.m1()        ),
    _m2       ( ps.m2()        ),
    _m3       ( ps.m3()        ),
    _mSqMother( ps.mSqMother() ),
    _mSq1     ( ps.mSq1()      ),
    _mSq2     ( ps.mSq2()      ),
    _mSq3     ( ps.mSq3()      ),
    _dalitzUp ( 0              ),
    _dalitzDn ( 0              )
{
  _mSq12min = std::pow( _m1      + _m2, 2 );
  _mSq12max = std::pow( _mMother - _m3, 2 );

  _mSq13min = ( _m1   * _mSqMother + _m2   * _mSq3 ) / ( _m1      + _m2 ) - _m1 * _m2;
  _mSq13max = ( _mSq1 * _mMother   - _mSq2 * _m3   ) / ( _mMother - _m3 ) + _m3 * _mMother;
}


DalitzContour::~DalitzContour()
{
  if ( _dalitzUp ) delete _dalitzUp;
  if ( _dalitzDn ) delete _dalitzDn;
}


// Implementation of the Kallen lambda function: x^2 + y^2 + z^2 - 2xy - 2xz - 2yz.
const double DalitzContour::kallen( const double& x, const double& y, const double& z )
{
  double result = 0.;
  result += std::pow( x, 2 );
  result += std::pow( y, 2 );
  result += std::pow( z, 2 );
  result -= 2. * x * y;
  result -= 2. * x * z;
  result -= 2. * y * z;

  return result;
}


// Implementation of the lower curve of the edge of the Dalitz plot.
const double DalitzContour::mSq13min( const double& mSq12 ) const
{
  if ( mSq12 > _mSq12max - PRECISION )
    return _mSq13max;

  if ( mSq12 < _mSq12min + PRECISION )
    return _mSq13min;

  double first  = std::pow( _mSqMother + _mSq1 - _mSq2 - _mSq3, 2 );
  double second = std::sqrt( kallen( mSq12, _mSq1     , _mSq2 ) );
  double third  = std::sqrt( kallen( mSq12, _mSqMother, _mSq3 ) );

  return ( first - std::pow( second + third, 2 ) ) / ( 4. * mSq12 );
}


// Implementation of the upper curve of the edge of the Dalitz plot.
const double DalitzContour::mSq13max( const double& mSq12 ) const
{
  if ( mSq12 > _mSq12max - PRECISION )
    return _mSq13max;

  if ( mSq12 < _mSq12min + PRECISION )
    return _mSq13min;

  double first  = std::pow( _mSqMother + _mSq1 - _mSq2 - _mSq3, 2 );
  double second = std::sqrt( kallen( mSq12, _mSq1     , _mSq2 ) );
  double third  = std::sqrt( kallen( mSq12, _mSqMother, _mSq3 ) );

  return ( first - std::pow( second - third, 2 ) ) / ( 4. * mSq12 );
}


const std::pair< std::vector< double >, std::vector< double > > DalitzContour::contourUp() const
{
  double mSq12 = 0.0;

  std::pair< std::vector< double >, std::vector< double > > cont;
  cont.first .reserve( _nPoints );
  cont.second.reserve( _nPoints );

  cont.first .push_back( _mSq12min );
  cont.second.push_back( _mSq13min );
  for ( unsigned pos = 1; pos < _nPoints - 1; ++pos )
  {
    mSq12 = _mSq12min + ( _mSq12max - _mSq12min ) * pos / double( _nPoints );
    cont.first .push_back( mSq12             );
    cont.second.push_back( mSq13max( mSq12 ) );
  }
  cont.first .push_back( _mSq12max );
  cont.second.push_back( _mSq13max );

  return cont;
}


const std::pair< std::vector< double >, std::vector< double > > DalitzContour::contourDn() const
{
  double mSq12 = 0.0;

  std::pair< std::vector< double >, std::vector< double > > cont;
  cont.first .reserve( _nPoints );
  cont.second.reserve( _nPoints );

  cont.first .push_back( _mSq12min );
  cont.second.push_back( _mSq13min );
  for ( unsigned pos = 1; pos < _nPoints - 1; ++pos )
  {
    mSq12 = _mSq12min + ( _mSq12max - _mSq12min ) * pos / double( _nPoints );
    cont.first .push_back( mSq12             );
    cont.second.push_back( mSq13min( mSq12 ) );
  }
  cont.first .push_back( _mSq12max );
  cont.second.push_back( _mSq13max );

  return cont;
}


void DalitzContour::draw()
{
  // Declare the upper and lower graphs of the Dalitz contour.
  const std::pair< std::vector< double >, std::vector< double > >& contUp = contourUp();
  const std::pair< std::vector< double >, std::vector< double > >& contDn = contourDn();

  std::pair< std::vector< double >, std::vector< double > > fillUp = contUp;
  std::pair< std::vector< double >, std::vector< double > > fillDn = contDn;

  const double& delta = 1.0e-2;

  // fillUp.first .push_back( _mSq12max );
  // fillUp.second.push_back( _mSq12max );
  // fillUp.first .push_back( _mSq12min );
  // fillUp.second.push_back( _mSq12max );
  // fillUp.first .push_back( _mSq12min );
  // fillUp.second.push_back( *contUp.second.begin() );

  // fillDn.first .push_back( _mSq12max );
  // fillDn.second.push_back( _mSq12min );
  // fillDn.first .push_back( _mSq12min );
  // fillDn.second.push_back( _mSq12min );
  // fillDn.first .push_back( _mSq12min );
  // fillDn.second.push_back( *contUp.second.begin() );


  fillUp.first .push_back( _mSq12max + delta );
  fillUp.second.push_back( *contUp.second.rbegin() );
  fillUp.first .push_back( _mSq12max + delta );
  fillUp.second.push_back( _mSq12max + delta );
  fillUp.first .push_back( _mSq12min - delta );
  fillUp.second.push_back( _mSq12max + delta );
  fillUp.first .push_back( _mSq12min - delta );
  fillUp.second.push_back( *contUp.second.begin() );
  fillUp.first .push_back( *contUp.first .begin() );
  fillUp.second.push_back( *contUp.second.begin() );

  fillDn.first .push_back( _mSq12max + delta );
  fillDn.second.push_back( *contUp.second.rbegin() );
  fillDn.first .push_back( _mSq12max + delta );
  fillDn.second.push_back( _mSq12min - delta );
  fillDn.first .push_back( _mSq12min - delta );
  fillDn.second.push_back( _mSq12min - delta );
  fillDn.first .push_back( _mSq12min - delta );
  fillDn.second.push_back( *contUp.second.begin() );
  fillDn.first .push_back( *contUp.first .begin() );
  fillDn.second.push_back( *contUp.second.begin() );

  _fillUp = new TGraph( fillUp.first.size(), &fillUp.first[0], &fillUp.second[0] );
  _fillDn = new TGraph( fillDn.first.size(), &fillDn.first[0], &fillDn.second[0] );

  _fillUp->SetLineWidth( 0 );
  _fillDn->SetLineWidth( 0 );

  _fillUp->SetFillColor( kWhite );
  _fillDn->SetFillColor( kWhite );

  _fillUp->Draw( "lf" );
  _fillDn->Draw( "lf" );

  _dalitzUp = new TGraph( contUp.first.size(), &contUp.first[0], &contUp.second[0] );
  _dalitzDn = new TGraph( contDn.first.size(), &contDn.first[0], &contDn.second[0] );

  // Specify a line width.
  _dalitzUp->SetLineWidth( 2 );
  _dalitzDn->SetLineWidth( 2 );

  // Specify a line color.
  _dalitzUp->SetLineColor( kRed );
  _dalitzDn->SetLineColor( kRed );

  // Draw the upper and lower curves of the Dalitz contour.
  _dalitzUp->Draw( "c" );
  _dalitzDn->Draw( "c" );
}

