
#include <iostream>
#include <algorithm>
#include <functional>

#include <cfit/dataset.hh>

#include <root/TGraph.h>
#include <root/TH2D.h>
#include <root/TCanvas.h>
#include <root/TColor.h>

#include <rtools/hist2d.hh>



void Hist2D::push_back( const Bin& bin )
{
  _zmin = ( _bins.empty() ? bin.content() : std::min( _zmin, bin.content() ) );
  _zmax = ( _bins.empty() ? bin.content() : std::max( _zmax, bin.content() ) );

  _bins.push_back( bin );
}



void Hist2D::insert( const std::vector< Bin >& bins )
{
  std::for_each( bins.begin(), bins.end(), std::bind( &Hist2D::push_back, this, std::placeholders::_1 ) );
}



void Hist2D::draw( const std::string& filename, const bool& withCol ) const
{
  std::vector< double > x( 5 );
  std::vector< double > y( 5 );

  const unsigned& nBins = _bins.size();

  // Each bin is a graph.
  TGraph graph;
  std::vector< TGraph > graphs;
  graphs.reserve( nBins );

  for ( std::vector< Bin >::const_iterator bin = _bins.begin(); bin != _bins.end(); ++bin )
  {
    x.clear();
    x.push_back( bin->xlo() );
    x.push_back( bin->xlo() );
    x.push_back( bin->xhi() );
    x.push_back( bin->xhi() );
    x.push_back( bin->xlo() );

    y.clear();
    y.push_back( bin->ylo() );
    y.push_back( bin->yhi() );
    y.push_back( bin->yhi() );
    y.push_back( bin->ylo() );
    y.push_back( bin->ylo() );

    // Calculate the color from the bin content.
    unsigned color = std::min( _nColor - 1, unsigned( _nColor * ( bin->content() - _zmin ) / ( _zmax - _zmin ) ) );

    // Keep this bin to the vector of graphs that will be drawn.
    graph = TGraph( 5, &x[0], &y[0] );
    graph.SetFillColor( _color[ color ] );
    graphs.push_back( graph );
  }

  TCanvas canvas;

  // Draw an empty histogram to produce the axes.
  TH2D hist( "nom", _title.c_str(), 1, _xmin, _xmax, 1, _ymin, _ymax );
  hist.SetStats( false );
  hist.SetBinContent( 0, 0, _zmin );
  hist.SetBinContent( 2, 2, _zmax );
  hist.SetMinimum( _zmin );
  hist.SetMaximum( _zmax );
  hist.Draw( withCol ? "colz" : "" );

  // Draw all the bins.
  std::for_each( graphs.begin(), graphs.end(), std::bind2nd( std::mem_fun_ref( &TGraph::Draw ), "LF" ) );

  // Save the plot to a file only if requested.
  if ( ! filename.empty() )
    canvas.Print( filename.c_str() );
}

