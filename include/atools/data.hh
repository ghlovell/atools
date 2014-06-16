#ifndef __DATA_HH__
#define __DATA_HH__


#include <utility>
#include <vector>
#include <string>

class Datum
{
private:
  double _x;
  double _y;
public:
  Datum( const double& x, const double& y )
    : _x( x ), _y( y )
  {}

  const double x() const { return _x; }
  const double y() const { return _y; }

  static const Datum mkDatum( const double& x, const double& y )
  {
    return Datum( x, y );
  }

  const unsigned quadrant   ( const double& xc, const double& yc )         const;
  const unsigned quadrant   ( const std::pair< double, double > centroid ) const;
  const bool     isQuadrant0( const std::pair< double, double > centroid ) const;
  const bool     isQuadrant1( const std::pair< double, double > centroid ) const;
  const bool     isQuadrant2( const std::pair< double, double > centroid ) const;
  const bool     isQuadrant3( const std::pair< double, double > centroid ) const;
};



class QuadrantSort
{
private:
  double _xc;
  double _yc;
public:
  QuadrantSort( const double& xc, const double& yc )
    : _xc( xc ), _yc( yc )
  {}

  QuadrantSort( const std::pair< double, double > centroid )
    : _xc( centroid.first ), _yc( centroid.second )
  {}

  const bool operator()( const Datum& left, const Datum& right )
  {
    return ( left.quadrant( _xc, _yc ) < right.quadrant( _xc, _yc ) );
  }
};



class Bin
{
private:
  double _xlo;     //
  double _xhi;     // Bin vertices.
  double _ylo;     //
  double _yhi;     //

  double _content; // Bin content.

public:
  Bin( const double& xlo, const double& xhi, const double& ylo, const double& yhi, const double& content )
    : _xlo( xlo ), _xhi( xhi ), _ylo( ylo ), _yhi( yhi ), _content( content )
  {}

  // Getters.
  const double& xlo()     const { return _xlo;     }
  const double& xhi()     const { return _xhi;     }
  const double& ylo()     const { return _ylo;     }
  const double& yhi()     const { return _yhi;     }
  const double& content() const { return _content; }

  // Setters.
  void setContent( const double& content ) { _content = content; }
};



class Dataset;

class Data
{
private:
  std::vector< Datum > _data;
  std::vector< Bin   > _bins;

  bool _binsDone;

  const std::pair< double, double > centroid( const std::vector< Datum >::const_iterator& begin,
                                              const std::vector< Datum >::const_iterator& end );

  void adapt( const std::vector< Datum >::iterator& begin, const std::vector< Datum >::iterator& end,
              const double& xmin, const double& xmax, const double& ymin, const double& ymax,
              const unsigned& minEntries );

public:
  Data() : _binsDone( false ) {}

  const unsigned size() const { return _data.size(); }

  void clear()
  {
    _data.clear();
    _bins.clear();
  }

  void add( const double& x, const double y );
  void add( const Dataset& data, const std::string& xField, const std::string& yField );

  std::vector< Bin > adaptiveBins( const double& xmin, const double& xmax,
                                   const double& ymin, const double& ymax,
                                   const unsigned& min );
};


#endif
