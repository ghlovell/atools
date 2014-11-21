
#include <cmath>

#include <cfit/pdfbase.hh>
#include <cfit/pdfmodel.hh>
#include <cfit/pdfexpr.hh>

#include <atools/utils.hh>

#include <rtools/hist.hh>
#include <rtools/lines.hh>

#include <root/TCanvas.h>
#include <root/TList.h>
#include <root/TPad.h>



const int Hist::bin( const double& val ) const
{
  return int( _nbins * ( val - _min ) / ( _max - _min ) );
}


// Auxiliary function to compute the center of a bin.
const double Hist::binCenter( const int& bin ) const
{
  return ( _max - _min ) / double( _nbins ) * ( bin + .5 ) + _min;
}


// Auxiliary function to compute the center of a bin.
const double Hist::binCenter( const int& bin, const int& nbins, const double& min, const double& max )
{
  return ( max - min ) / double( nbins ) * ( bin + .5 ) + min;
}


// Auxiliary function to evaluate the pdf at a given point.
const double Hist::pdf( const double& x ) const
{
  return _pdf->evaluate( x );
}


void Hist::setData( const Dataset& data, const std::string& field )
{
  _binContent.assign( _nbins, 0. );

  std::vector< double > values = data.values( field );
  typedef std::vector< double >::const_iterator dIter;
  int binIdx; // Bin index.
  for ( dIter dat = values.begin(); dat != values.end(); ++dat )
  {
    binIdx = bin( *dat );
    if ( binIdx < 0 )
      _underflow++;
    else if ( binIdx > _nbins )
      _overflow++;
    else
      _binContent[ binIdx ]++;
  }
}


void Hist::addData( const Dataset& data, const std::string& field )
{
  std::vector< double > values = data.values( field );
  typedef std::vector< double >::const_iterator dIter;
  int binIdx; // Bin index.
  for ( dIter dat = values.begin(); dat != values.end(); ++dat )
  {
    binIdx = bin( *dat );
    if ( binIdx < 0 )
      _underflow++;
    else if ( binIdx > _nbins )
      _overflow++;
    else
      _binContent[ binIdx ]++;
  }
}


void Hist::addPdf( const PdfModel& pdf )
{
  PdfBase* copy = pdf.copy();

  if ( _pdfs.empty() )
    _pdf = copy;

  _pdfs.push_back( copy );
}


void Hist::addPdf( const PdfExpr& pdf )
{
  PdfBase* copy = new PdfExpr( pdf );

  if ( _pdfs.empty() )
    _pdf = copy;

  _pdfs.push_back( copy );
}



// ALERT: this function does not deallocate the memory it allocates.
TH1D* Hist::residuals( const TH1D& data, const TH1D* pdf ) const
{
  TH1D* residuals = new TH1D( ( "residuals_" + _name ).c_str(), "", _nbins, _min, _max );
  double datum;
  double pdfval;
  for ( int bin = 0; bin < _nbins; ++bin )
  {
    // Root starts numbering from 1.
    datum  = data .GetBinContent( bin + 1 );
    pdfval = pdf ->GetBinContent( bin + 1 );

    residuals->SetBinContent( bin + 1, Utils::residual( datum, pdfval ) );
    residuals->SetBinError  ( bin + 1, 1.0 );
  }

  return residuals;
}


TH1D* Hist::project( const std::string& field, const double& area ) const
{
  TH1D* pdf = new TH1D( ( "pdf_" + _name ).c_str(), _title.c_str(), _nbins, _min, _max );

  const double&& yield = _pdf->yield();

  // Evaluate the model in a wide region of points.
  double pdfval = 0.0;
  for ( int x = 0; x < _nbins; ++x )
  {
    pdfval = _pdf->project( field, binCenter( x, _nbins, _min, _max ) );

    pdf->SetBinContent( x + 1, area * pdfval * ( _max - _min ) / double( _nbins * yield ) );
  }

  return pdf;
}



TH1D* Hist::getHist( const std::string& field, const double& area ) const
{
  TH1D* pdf = new TH1D( ( "pdf_" + _name ).c_str(), _title.c_str(), _nbins, _min, _max );

  std::vector< double > vals( 1 );

  const double&& yield = _pdf->yield();

  // Evaluate the model in a wide region of points.
  for ( int x = 0; x < _nbins; ++x )
  {
    vals[ 0 ] = binCenter( x, _nbins, _min, _max );

    double yVal = _pdf->evaluate( vals );

    pdf->SetBinContent( x + 1, area * yVal * ( _max - _min ) / double( _nbins * yield ) );
  }

  return pdf;
}



void Hist::draw( const std::string& file ) const
{
  TH1D data( ( "data_" + _name ).c_str(), _title.c_str(), _nbins, _min, _max );
  int bin = 0;
  double area = 0.;
  typedef std::vector< double >::const_iterator bIter;
  for ( bIter val = _binContent.begin(); val != _binContent.end(); ++val )
  {
    area += *val;
    data.SetBinContent( 1 + bin++, *val );
  }

  data.SetBinErrorOption( TH1::kPoisson );

  // ALERT: this is an allocation. This histogram has to be deallocated in this function.
  TH1D* pdf = 0;
  if ( _field == "mSq12" || _field == "mSq13" || _field == "mSq23" )
    pdf = project( _field, area );
  else
    pdf = getHist( _field, area );

  // ALERT: this is an allocation. This histogram has to be deallocated in this function.
  TH1D* resids = residuals( data, pdf );

  // Prepare the canvas to draw the histograms.
  TCanvas canvas( _name.c_str() );
  if ( _withResiduals )
    canvas.Divide( 1, 2, .1, .1 );
  else
    canvas.Divide( 1, 1, .1, .1 );

  // Draw everything on the canvas.
  draw( canvas, data, resids, pdf );

  // Save the canvas.
  canvas.Write();

  if ( file != "" )
    canvas.Print( file.c_str() );

  delete resids;
  delete pdf;
}


void Hist::draw( const TCanvas& canvas, TH1D& data, TH1D* resids, TH1D* pdf ) const
{
  // Find the canvas pads.
  TPad* padHisto = (TPad*) canvas.GetListOfPrimitives()->At( 0 );
  TPad* padResid = (TPad*) canvas.GetListOfPrimitives()->At( 1 ); // Returns 0 if it does not exist.

  if ( _logscale )
    padHisto->SetLogy();

  // Do the cosmetics.
  cosmeticsData  ( data   );
  cosmeticsResids( resids );
  cosmeticsPads  ( padHisto, padResid );
  pdf->SetLineColor( kBlue );
  pdf->SetLineWidth( 2 );

  // Plot the data histogram and the function.
  padHisto->cd();
  if ( data.Integral() )
  {
    data .Draw( "e01"   );
    pdf ->Draw( "samec" );
  }
  else
    pdf->Draw( "c" );

  // Plot the residuals histogram.
  if ( padResid )
  {
    padResid->cd();
    resids->Draw();
    Lines( resids ).draw();
    resids->Draw( "same" );
  }
}


void Hist::cosmeticsData( TH1D& hist ) const
{
  // Do the cosmetics.
  double r = 0.2;
  double sd = 1. / ( 1. - r );

  hist.SetStats( false );

  hist.SetMarkerStyle( 8   );
  hist.SetMarkerSize ( 0.8 );

  TAxis* xAxis = hist.GetXaxis();
  xAxis->SetTickLength ( sd * xAxis->GetTickLength() );
  xAxis->CenterTitle   (      );
  xAxis->SetLabelSize  ( 0    );
  xAxis->SetTitleSize  ( 0.04 );
  xAxis->SetTitleOffset( 0.4  );

  TAxis* yAxis = hist.GetYaxis();
  yAxis->SetLabelSize( sd * yAxis->GetLabelSize() );

  hist.SetLineColor( kBlack );
}



void Hist::cosmeticsResids( TH1D* hist ) const
{
  // Do the cosmetics.
  double r = 0.2;
  double sr = 1. / r;

  hist->SetMinimum    ( -5.0  );
  hist->SetMaximum    (  5.0  );
  hist->SetStats      ( false );
  hist->SetMarkerStyle(  8    );
  hist->SetMarkerSize (  0.8  );

  TAxis* xAxis = hist->GetXaxis();
  xAxis->SetTickLength ( sr * xAxis->GetTickLength()  );
  xAxis->SetLabelSize  ( sr * xAxis->GetLabelSize()   );
  xAxis->SetTitleSize  ( sr * xAxis->GetTitleSize()   );
  xAxis->SetLabelOffset( sr * xAxis->GetLabelOffset() );

  TAxis* yAxis = hist->GetYaxis();
  yAxis->SetNdivisions ( 504                          );
  yAxis->SetLabelSize  ( sr * yAxis->GetLabelSize()   );
}


void Hist::cosmeticsPads( TPad* hist, TPad* resid ) const
{
  // Do the cosmetics.
  double r = 0.2;
  double small = 0.05;

  hist->SetPad( 0., r , 1., 1. );
  hist->SetBottomMargin( small );

  if ( resid )
  {
    resid->SetPad( 0., 0., 1., r  );
    resid->SetBottomMargin( 0.25  );
    resid->SetTopMargin   ( small );
  }
}

