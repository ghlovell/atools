
#include <cfit/pdfbase.hh>
#include <cfit/pdfmodel.hh>
#include <cfit/pdfexpr.hh>

#include <rtools/graph.hh>

#include <root/TCanvas.h>
#include <root/TGraph.h>
#include <root/TAxis.h>


void Graph::addPdf( const PdfModel& pdf )
{
  PdfBase* pdfcopy = pdf.copy();

  if ( _pdfs.empty() )
    _pdf = pdfcopy;

  _pdfs.push_back( pdf.copy() );
}


void Graph::addPdf( const PdfExpr& pdf )
{
  PdfBase* pdfcopy = new PdfExpr( pdf );

  if ( _pdfs.empty() )
    _pdf = pdfcopy;

  _pdfs.push_back( pdfcopy );
}


void Graph::draw( const std::string& file ) const
{
  const double& step = ( _max - _min ) / double( _nbins );

  // Create a vector of the x and y values of each point of the graph.
  std::vector< double > xdata;
  std::vector< double > ydata;

  std::vector< double > vars( 1 );
  for ( double x = _min; x <= _max; x += step )
  {
    vars[ 0 ] = x;
    xdata.push_back( x                      );
    ydata.push_back( _pdf->evaluate( vars ) );
  }

  // Find the maximum y value.
  double maxy = *std::max_element( ydata.begin(), ydata.end() );

  // Create the graph.
  TGraph graph( xdata.size(), (double*) &(xdata[ 0 ]), (double*) &(ydata[ 0 ]) );

  // Create the canvas where the graph will be drawn.
  TCanvas canvas( _name.c_str() );

  // Do some cosmetics.
  graph.SetTitle( _title.c_str() );
  graph.SetMaximum( maxy * 1.05 );
  graph.GetXaxis()->SetLimits( _min, _max );
  graph.SetLineWidth( 2.0 );
  graph.SetLineColor( kBlue );

  // Draw the graph and save it in the latest open root file.
  graph.Draw( "al" );
  canvas.Write();

  if ( file != "" )
    canvas.Print( file.c_str() );
}

