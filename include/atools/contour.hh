#ifndef __CONTOUR_HH__
#define __CONTOUR_HH__

#include <string>

#include <cfit/phasespace.hh>

#include <root/TGraph.h>

class DalitzContour
{
private:
  unsigned _nPoints;

  PhaseSpace _ps;

  double _mMother;
  double _m1;
  double _m2;
  double _m3;

  double _mSqMother;
  double _mSq1;
  double _mSq2;
  double _mSq3;

  double _mSq12min;
  double _mSq12max;
  double _mSq13min; // mSq13 at mSq12min
  double _mSq13max; // mSq13 at mSq12max

  TGraph* _fillUp; // These must be pointers, or otherwise they don't get drawn
  TGraph* _fillDn; //    properly. Root things, you'll understand when you grow up.

  TGraph* _dalitzUp; // These must be pointers, or otherwise they don't get drawn
  TGraph* _dalitzDn; //    properly. Root things, you'll understand when you grow up.

  static const double kallen( const double& x, const double& y, const double& z );
  inline const double mSq13min( const double& mSq12 ) const;
  inline const double mSq13max( const double& mSq12 ) const;

  const std::pair< std::vector< double >, std::vector< double > > contourUp() const;
  const std::pair< std::vector< double >, std::vector< double > > contourDn() const;

public:
  DalitzContour( const PhaseSpace& ps, const unsigned& nPoints = 1000 );
  ~DalitzContour();

  void draw();
};

#endif

