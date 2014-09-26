#ifndef __ADAPTIVEDALITZ_HH__
#define __ADAPTIVEDALITZ_HH__

#include <cfit/phasespace.hh>
#include <cfit/dataset.hh>

#include <cfit/pdfbase.hh>
#include <cfit/pdfexpr.hh>

#include <atools/data.hh>



class AdaptiveDalitz
{
private:
  std::string _name;
  std::string _title;

  unsigned _minEntries; // Minimum number of entries per bin.

  Data _data;

  PhaseSpace _ps;

  double _mMother;
  double _m1;
  double _m2;
  double _m3;

  double _mSq12min;
  double _mSq12max;

  double _mSq13min;
  double _mSq13max;

  // Horizontal and vertical limits of the histogram.
  double _xmin;
  double _xmax;
  double _ymin;
  double _ymax;

  // Minimum and maximum bin content.
  double _zmin;
  double _zmax;

  std::vector< PdfBase* > _pdfs;

  // Color palette.
  std::vector< unsigned > _color;
  unsigned                _nColor;

public:
  AdaptiveDalitz( const PhaseSpace& ps, const unsigned& minEntries = 10 );
  ~AdaptiveDalitz();

  void setName ( const std::string& name  ) { _name  = name;  }
  void setTitle( const std::string& title ) { _title = title; }
  void setData ( const Dataset& data, const std::string& field1, const std::string& field2 );
  void addData ( const Dataset& data, const std::string& field1, const std::string& field2 );

  void draw( const std::string& file = "" );

  void addPdf( const PdfModel& pdf );
  void addPdf( const PdfExpr&  pdf );
};

#endif
