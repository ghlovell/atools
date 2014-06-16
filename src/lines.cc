#include <atools/lines.hh>

Lines::Lines( double xMin, double xMax )
{
  midLine = new TLine( xMin,  0., xMax,  0. );
  uppLine = new TLine( xMin,  2., xMax,  2. );
  lowLine = new TLine( xMin, -2., xMax, -2. );

  uppLine->SetLineColor( kRed );
  lowLine->SetLineColor( kRed );
}


Lines::Lines( const TH1D* resid )
{
  // Calcula el mínim i màxim de l'histograma de resultats.
  double xMin = resid->GetXaxis()->GetXmin();
  double xMax = resid->GetXaxis()->GetXmax();

  midLine = new TLine( xMin,  0., xMax,  0. );
  uppLine = new TLine( xMin,  2., xMax,  2. );
  lowLine = new TLine( xMin, -2., xMax, -2. );

  uppLine->SetLineColor( kRed );
  lowLine->SetLineColor( kRed );
}



void Lines::draw()
{
  midLine->Draw( "same" );
  uppLine->Draw( "same" );
  lowLine->Draw( "same" );
}
