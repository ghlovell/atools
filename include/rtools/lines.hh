#ifndef __LINES_HH__
#define __LINES_HH__

#include <root/TLine.h>
#include <root/TH1D.h>

class Lines
{
private:
  TLine* midLine;
  TLine* uppLine;
  TLine* lowLine;
public:
  Lines( double xMin, double xMax );
  Lines( const TH1D* resid );
  void draw();
};


#endif
