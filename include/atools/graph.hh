#ifndef __GRAPH_HH__
#define __GRAPH_HH__

#include <string>
#include <vector>

#include <cfit/pdfbase.hh>

class Graph
{
private:
  std::string             _name;
  std::string             _title;
  unsigned                _nbins;
  double                  _min;
  double                  _max;
  std::vector< double >   _binContent;
  std::vector< PdfBase* > _pdfs;

  PdfBase*                _pdf;  // Pointer to the total pdf, wrt which
                                 //    residuals have to be computed.

public:
  Graph( const unsigned& nbins, const double& min, const double& max )
    : _name         ( ""    ),
      _title        ( ""    ),
      _nbins        ( nbins ),
      _min          ( min   ),
      _max          ( max   ),
      _pdf          ( 0     )
    {
    }

  void setName ( const std::string& name  ) { _name  = name;  }
  void setTitle( const std::string& title ) { _title = title; }
  void addPdf  ( const PdfModel& pdf );
  void addPdf  ( const PdfExpr&  pdf );

  void draw    ( const std::string& file = "" ) const;
};


#endif

