
#include <cmath>
#include <functional>

#include <root/TH2D.h>
#include <root/TCanvas.h>
#include <root/TGraph.h>
#include <root/TH2Poly.h>

#include <cfit/dataset.hh>
#include <cfit/function.hh>

#include <atools/contour.hh>
#include <atools/utils.hh>

#include <atools/data.hh>
#include <atools/adaptivedalitz.hh>



// Constructor.
AdaptiveDalitz::AdaptiveDalitz( const PhaseSpace& ps, const unsigned& minEntries )
  : _name      ( ""           ),
    _title     ( ""           ),
    _minEntries( minEntries   ),
    _ps        ( ps           ),
    _mMother   ( ps.mMother() ),
    _m1        ( ps.m1()      ),
    _m2        ( ps.m2()      ),
    _m3        ( ps.m3()      )
{
  // Minimum and maximum values of the Dalitz variables.
  _mSq12min = std::pow( _m1      + _m2, 2 );
  _mSq12max = std::pow( _mMother - _m3, 2 );

  _mSq13min = std::pow( _m1      + _m3, 2 );
  _mSq13max = std::pow( _mMother - _m2, 2 );

  // Leave an extra margin for the upper and lower limits of the plot.
  _xmin = _mSq12min - 0.10;
  _xmax = _mSq12max + 0.10;

  _ymin = _mSq13min - 0.10;
  _ymax = _mSq13max + 0.10;

  // Define the color palette.
  _color = { 53, 55, 58, 60, 63, 65, 68, 70, 73, 75, 78, 80, 83, 85, 88, 90, 93, 95, 98, 100 };
  _nColor = _color.size();
}



AdaptiveDalitz::~AdaptiveDalitz()
{
  // Free the memory allocated for the pdfs.
  typedef std::vector< PdfBase* >::iterator pIter;
  pIter begin = _pdfs.begin();
  pIter end   = _pdfs.end();
  for ( pIter pdf = begin; pdf != end; ++pdf )
    delete *pdf;
}



void AdaptiveDalitz::draw( const std::string& file )
{
  // Retrieve the histogram bins, each with their data content.
  std::vector< Bin > bins = _data.adaptiveBins( _mSq12min, _mSq12max, _mSq13min, _mSq13max, _minEntries );

  const unsigned& nData = _data.size();

  _zmin = 0.0;
  _zmax = 0.0;

  // Initialize the normalized residual.
  double res = 0.0;

  std::vector< double > vals( 3 );
  const double& dxy = 1.0e-3;
  for ( std::vector< Bin >::iterator bin = bins.begin(); bin != bins.end(); ++bin )
  {
    // Evaluate the pdf projection over the bin.
    double z = 0.0;
    for ( double x = bin->xlo(); x < bin->xhi(); x += dxy )
      for ( double y = bin->ylo(); y < bin->yhi(); y += dxy )
        z += _pdfs[ 0 ]->project( "mSq12", "mSq13", x, y );
    z *= std::pow( dxy, 2 );

    // Evaluate the normalized residual.
    res = Utils::residual( bin->content(), nData * z );

    bin->setContent( res );

    // Update the minimum and maximum values of the residual.
    _zmin = std::min( _zmin, res );
    _zmax = std::max( _zmax, res );
  }

  TH2Poly hist( ( "hist_" + _name ).data(), _title.c_str(), _xmin, _xmax, _ymin, _ymax );

  // Bin index.
  unsigned idx;

  // Vectors to contain the bin content.
  std::vector< double > xBin;
  std::vector< double > yBin;

  for ( std::vector< Bin >::const_iterator bin = bins.begin(); bin != bins.end(); ++bin )
  {
    // Define the bin contour.
    xBin.clear();
    xBin.push_back( bin->xlo() );
    xBin.push_back( bin->xlo() );
    xBin.push_back( bin->xhi() );
    xBin.push_back( bin->xhi() );
    xBin.push_back( bin->xlo() );

    yBin.clear();
    yBin.push_back( bin->ylo() );
    yBin.push_back( bin->yhi() );
    yBin.push_back( bin->yhi() );
    yBin.push_back( bin->ylo() );
    yBin.push_back( bin->ylo() );

    // Add a new bin to the histogram with the previously specified contour.
    idx = hist.AddBin( xBin.size(), xBin.data(), yBin.data() );

    // Set the histogram bin content to the bin object content.
    hist.SetBinContent( idx, bin->content() );
  }

  // Plot the data histogram.
  TCanvas canvas( _name.c_str(), _name.c_str(), 800, 800 );

  // Configure the properties of the plot.
  hist.SetStats( false );
  hist.SetMinimum( -5.0 );
  hist.SetMaximum(  5.0 );

  bool withCol = true;
  hist.Draw( withCol ? "colz" : "" );

  // Draw the contour of the Dalitz plot.
  DalitzContour contour( _ps );
  contour.draw();

  // Write all of these things to the latest open file.
  canvas.Write();

  // If any file name has been given, print the plot on a file.
  if ( ! file.empty() )
    canvas.Print( file.c_str() );
}




void AdaptiveDalitz::setData( const Dataset& data, const std::string& field1, const std::string& field2 )
{
  // Set all bin contents to zero.
  _data.clear();

  // Fill the bin contents with every event.
  _data.add( data, field1, field2 );
}


void AdaptiveDalitz::addData( const Dataset& data, const std::string& field1, const std::string& field2 )
{
  // Fill the bin contents with every event.
  _data.add( data, field1, field2 );
}


void AdaptiveDalitz::addPdf( const PdfModel& pdf )
{
  _pdfs.push_back( pdf.copy() );
}


void AdaptiveDalitz::addPdf( const PdfExpr& pdf )
{
  _pdfs.push_back( new PdfExpr( pdf ) );
}
