
#include <algorithm>

#include <cfit/dataset.hh>

#include <atools/data.hh>


// Return quadrant number wrt a centroid.
const unsigned Datum::quadrant( const double& xc, const double& yc ) const
{
  return 2 * ( _x > xc ) + ( _y > yc );
}

// Return quadrant number wrt a centroid.
const unsigned Datum::quadrant( const std::pair< double, double > centroid ) const
{
  return 2 * ( _x > centroid.first ) + ( _y > centroid.second );
}

const bool Datum::isQuadrant0( const std::pair< double, double > centroid ) const
{
  return ( quadrant( centroid ) == 0 );
}

const bool Datum::isQuadrant1( const std::pair< double, double > centroid ) const
{
  return ( quadrant( centroid ) == 1 );
}

const bool Datum::isQuadrant2( const std::pair< double, double > centroid ) const
{
  return ( quadrant( centroid ) == 2 );
}

const bool Datum::isQuadrant3( const std::pair< double, double > centroid ) const
{
  return ( quadrant( centroid ) == 3 );
}



const std::pair< double, double > Data::centroid( const std::vector< Datum >::const_iterator& begin,
                                                  const std::vector< Datum >::const_iterator& end )
{
  // Centroid position.
  double xc = 0.0;
  double yc = 0.0;

  for ( std::vector< Datum >::const_iterator datum = begin; datum != end; ++datum )
  {
    xc += datum->x();
    yc += datum->y();
  }

  const unsigned& nData = end - begin;

  xc /= nData;
  yc /= nData;

  return std::make_pair( xc, yc );
}



void Data::adapt( const std::vector< Datum >::iterator& begin, const std::vector< Datum >::iterator& end,
                  const double& xmin, const double& xmax, const double& ymin, const double& ymax,
                  const unsigned& minEntries )
{
  // Centroid position.
  const std::pair< double, double >& cent = centroid( begin, end );

  const double& xc = cent.first;
  const double& yc = cent.second;

  // Sort the range by quadrant wrt the centroid.
  std::sort( begin, end, QuadrantSort( cent ) );

  // Find the 3 positions of separation between the quadrants wrt the centroid.
  std::vector< Datum >::iterator it1 = std::find_if( begin, end, std::bind2nd( std::mem_fun_ref( &Datum::isQuadrant1 ), cent ) );
  std::vector< Datum >::iterator it2 = std::find_if( it1  , end, std::bind2nd( std::mem_fun_ref( &Datum::isQuadrant2 ), cent ) );
  std::vector< Datum >::iterator it3 = std::find_if( it2  , end, std::bind2nd( std::mem_fun_ref( &Datum::isQuadrant3 ), cent ) );

  // If any quadrant contains fewer elements than required by minEntries, create a bin.
  if ( ( it1 - begin < minEntries ) ||
       ( it2 - it1   < minEntries ) ||
       ( it3 - it2   < minEntries ) ||
       ( end - it3   < minEntries ) )
  {
    _bins.push_back( Bin( xmin, xmax, ymin, ymax, end - begin ) );
    return;
  }

  // Recurse.
  adapt( begin, it1, xmin, xc  , ymin, yc  , minEntries );
  adapt( it1  , it2, xmin, xc  , yc  , ymax, minEntries );
  adapt( it2  , it3, xc  , xmax, ymin, yc  , minEntries );
  adapt( it3  , end, xc  , xmax, yc  , ymax, minEntries );
}



void Data::add( const double& x, const double y )
{
  _data.push_back( Datum( x, y ) );

  // Since new data have been added, it's not possible to reuse previously computed bins.
  _binsDone = false;
}



void Data::add( const Dataset& data, const std::string& xField, const std::string& yField )
{
  std::vector< double > x = data.values( xField );
  std::vector< double > y = data.values( yField );

  std::transform( x.begin(), x.end(), y.begin(), std::back_inserter( _data ), Datum::mkDatum );

  // Since new data have been added, it's not possible to reuse previously computed bins.
  _binsDone = false;
}



std::vector< Bin > Data::adaptiveBins( const double& xmin, const double& xmax,
                                       const double& ymin, const double& ymax,
                                       const unsigned& minEntries )
{
  // If the bins are already evaluated, don't recompute them.
  if ( _binsDone )
    return _bins;

  _bins.clear();
  adapt( _data.begin(), _data.end(), xmin, xmax, ymin, ymax, minEntries );

  // Mark the bins calculation as done, to avoid recomputing it unnecessarily.
  _binsDone = true;

  return _bins;
}

