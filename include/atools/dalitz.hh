#ifndef __DALITZ_HH__
#define __DALITZ_HH__

#include <cfit/phasespace.hh>
#include <cfit/dataset.hh>
#include <cfit/pdfexpr.hh>

class Dalitz
{
private:
  std::string _name;
  std::string _title;
  int         _nbins;
  std::vector< std::vector< double > > _binContent;

  PhaseSpace _ps;

  double _mMother;
  double _m1;
  double _m2;
  double _m3;

  double _mSq12min;
  double _mSq12max;

  double _min;
  double _max;

  const double binCenter( const int&    bin ) const;
  const int    bin      ( const double& val ) const;

public:
  Dalitz( const int& nbins, const PhaseSpace& ps );

  void setName ( const std::string& name  ) { _name  = name;  }
  void setTitle( const std::string& title ) { _title = title; }
  void setData ( const Dataset& data, const std::string& field1, const std::string& field2 );
  void addData ( const Dataset& data, const std::string& field1, const std::string& field2 );

  void setData ( Function data, const std::string& field1, const std::string& field2 );

  void draw( const std::string& file = "" );

  Dalitz* residuals( const PdfExpr& pdf, const std::string& field1, const std::string& field2, const std::string& field3 );
};

#endif
