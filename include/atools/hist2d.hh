#ifndef __HIST2D_HH__
#define __HIST2D_HH__


#include <atools/data.hh>


class Hist2D
{
private:
  // Horizontal and vertical limits of the histogram.
  double _xmin;
  double _xmax;
  double _ymin;
  double _ymax;

  // Minimum and maximum bin content.
  double _zmin;
  double _zmax;

  // Histogram title.
  std::string _title;

  // Color palette.
  std::vector< unsigned > _color;
  unsigned                _nColor;

  // Histogram bins.
  std::vector< Bin > _bins;

public:
  Hist2D( const double& xmin, const double& xmax, const double& ymin, const double& ymax, const std::string& title )
    : _xmin( xmin )    , _xmax( xmax )    ,
      _ymin( ymin )    , _ymax( ymax )    ,
      _zmin( 0.0 )     , _zmax( 0.0 )     ,
      _title( title )
  {
    // Define the color palette.
    _color = { 53, 55, 58, 60, 63, 65, 68, 70, 73, 75, 78, 80, 83, 85, 88, 90, 93, 95, 98, 100 };
    _nColor = _color.size();
  }

  void push_back( const Bin& bin );

  void insert( const std::vector< Bin >& bins );

  void draw( const std::string& filename, const bool& withCol = false ) const;
};


#endif
