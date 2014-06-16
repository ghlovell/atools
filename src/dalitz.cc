
#include <cmath>
#include <functional>

#include <root/TH2D.h>
#include <root/TCanvas.h>
#include <root/TGraph.h>

#include <cfit/function.hh>

#include <atools/dalitz.hh>
#include <atools/utils.hh>
#include <atools/contour.hh>



// Auxiliary function to compute the center of a bin.
const double Dalitz::binCenter( const int& bin ) const
{
  return ( _max - _min ) / double( _nbins ) * ( bin + .5 ) + _min;
}


// Auxiliary function to compute the bin that corresponds to a value.
const int Dalitz::bin( const double& val ) const
{
  return int( _nbins * ( val - _min ) / ( _max - _min ) );
}


// Constructor.
Dalitz::Dalitz( const int& nbins, const PhaseSpace& ps )
  : _name   ( ""           ),
    _title  ( ""           ),
    _nbins  ( nbins        ),
    _ps     ( ps           ),
    _mMother( ps.mMother() ),
    _m1     ( ps.m1()      ),
    _m2     ( ps.m2()      ),
    _m3     ( ps.m3()      )
{
  _mSq12min = std::pow( _m1      + _m2, 2 );
  _mSq12max = std::pow( _mMother - _m3, 2 );

  _min = _mSq12min - 0.10;
  _max = _mSq12max + 0.10;

  // Set all bin contents to zero.
  _binContent.resize( _nbins );
  typedef std::vector< std::vector< double > >::iterator vIter;
  for ( vIter row = _binContent.begin(); row != _binContent.end(); ++row )
    row->assign( _nbins, 0.0 );
}



void Dalitz::draw( const std::string& file )
{
  TH2D data( _name.c_str(), _title.c_str(), _nbins, _min, _max, _nbins, _min, _max );
  data.SetStats( false );

  for ( int i = 0; i < _nbins; ++i )
    for ( int j = 0; j < _nbins; ++j )
      data.SetBinContent( 1 + i, 1 + j, _binContent[ i ][ j ] );

  // Plot the data histogram.
  TCanvas canvas( _name.c_str(), _name.c_str(), 800, 800 );
  data.Draw( "col" );

  // Draw the upper and lower curves of the Dalitz contour.
  DalitzContour contour( _ps );
  contour.draw();

  // Write all of these things to the latest open file.
  canvas.Write();

  // If any file name has been given, print the plot on a file.
  if ( ! file.empty() )
    canvas.Print( file.c_str() );
}


void Dalitz::setData( const Dataset& data, const std::string& field1, const std::string& field2 )
{
  // Set all bin contents to zero.
  typedef std::vector< std::vector< double > >::iterator vIter;
  for ( vIter row = _binContent.begin(); row != _binContent.end(); ++row )
    row->assign( _nbins, 0.0 );

  // Fill the bin contents with every event.
  std::vector< double > values1 = data.values( field1 );
  std::vector< double > values2 = data.values( field2 );
  typedef std::vector< double >::const_iterator dIter;
  dIter dat2 = values2.begin();
  for ( dIter dat1 = values1.begin(); dat1 != values1.end(); ++dat1, ++dat2 )
    _binContent[ bin( *dat1 ) ][ bin( *dat2 ) ]++;
}


void Dalitz::addData( const Dataset& data, const std::string& field1, const std::string& field2 )
{
  // Fill the bin contents with every event.
  std::vector< double > values1 = data.values( field1 );
  std::vector< double > values2 = data.values( field2 );
  typedef std::vector< double >::const_iterator dIter;
  dIter dat2 = values2.begin();
  for ( dIter dat1 = values1.begin(); dat1 != values1.end(); ++dat1, ++dat2 )
    _binContent[ bin( *dat1 ) ][ bin( *dat2 ) ]++;
}


void Dalitz::setData( Function data, const std::string& field1, const std::string& field2 )
{
  // Set all bin contents to zero.
  typedef std::vector< std::vector< double > >::iterator vIter;
  for ( vIter row = _binContent.begin(); row != _binContent.end(); ++row )
    row->assign( _nbins, 0.0 );

  // Fill the bin contents with the value of the function at every bin center.
  double x = 0.0;
  double y = 0.0;
  for ( int binX = 0; binX < _nbins; ++binX )
    for ( int binY = 0; binY < _nbins; ++binY )
    {
      x = binCenter( binX );
      y = binCenter( binY );
      if ( _ps.contains( x, y ) )
      {
        data.setVar( field1, x );
        data.setVar( field2, y );
        _binContent[ binX ][ binY ] = data.evaluate();
      }
    }
}


Dalitz* Dalitz::residuals( const PdfExpr& pdf, const std::string& field1, const std::string& field2, const std::string& field3 )
{
  PdfExpr model( pdf );

  // Evaluate the number of data.
  double integral = 0.0;
  for ( int i = 0; i < _nbins; ++i )
    for ( int j = 0; j < _nbins; ++j )
      integral += _binContent[ i ][ j ];

  // Initialize some variables used in the evaluation of the pdf.
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  const double& mSqSum = _ps.mSqMother() + _ps.mSq1() + _ps.mSq2() + _ps.mSq3();

  // Fill the bin contents with the value of the function at every bin center.
  Dalitz* dalitz = new Dalitz( _nbins, _ps );

  for ( int binX = 0; binX < _nbins; ++binX )
    for ( int binY = 0; binY < _nbins; ++binY )
    {
      x = binCenter( binX );
      y = binCenter( binY );
      z = mSqSum - x - y;
      if ( _ps.contains( x, y ) )
      {
        model.setVar( field1, x );
        model.setVar( field2, y );
        model.setVar( field3, z );

        double pdfval = integral * std::pow( ( _max - _min ) / double( _nbins ), 2 ) * model.evaluate();

        dalitz->_binContent[ binX ][ binY ] = Utils::residual( _binContent[ binX ][ binY ], pdfval );
      }
    }

  return dalitz;
}
