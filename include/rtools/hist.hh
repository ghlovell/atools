#ifndef __HIST_HH__
#define __HIST_HH__

#include <string>
#include <vector>

#include <cfit/dataset.hh>
#include <cfit/pdfbase.hh>

#include <root/TH1D.h>
#include <root/TPad.h>

class Hist
{
private:
  std::string             _name;
  std::string             _title;
  bool                    _withResiduals;
  unsigned                _nbins;
  double                  _min;
  double                  _max;
  std::vector< double >   _binContent;
  std::vector< PdfBase* > _pdfs;

  double                  _underflow;
  double                  _overflow;

  PdfBase*                _pdf;  // Pointer to the total pdf, wrt which
                                 //    residuals have to be computed.

  std::string             _field; // Temporary argument until
                                  //    PdfBase::project( field ) is defined.

  bool                    _logscale;

  TH1D* residuals( const TH1D& data, const TH1D* pdf ) const;

  // TEMPORARY FUNCTION WHILE THERE'S NO PdfBase PROJECTION FUNCTION.
  TH1D* project( const std::string& field, const double& area ) const;

  TH1D* getHist( const std::string& field, const double& area ) const;

  void cosmeticsData  ( TH1D& hist ) const;
  void cosmeticsResids( TH1D* hist ) const;
  void cosmeticsPads  ( TPad* hist, TPad* resid ) const;

  void draw( const TCanvas& canvas, TH1D& data, TH1D* resids, TH1D* pdf ) const;


public:
  Hist( const unsigned& nbins, const double& min, const double& max )
    : _name         ( ""    ),
      _title        ( ""    ),
      _withResiduals( true  ),
      _nbins        ( nbins ),
      _min          ( min   ),
      _max          ( max   ),
      _underflow    ( 0.0   ),
      _overflow     ( 0.0   ),
      _pdf          ( 0     ),
      _logscale     ( false )
    {
    }

  void setName ( const std::string& name  ) { _name  = name;  }
  void setTitle( const std::string& title ) { _title = title; }
  void setData ( const Dataset&  data, const std::string& field );
  void addData ( const Dataset&  data, const std::string& field );
  void addPdf  ( const PdfModel& pdf );
  void addPdf  ( const PdfExpr&  pdf );

  const double pdf( const double& x ) const;

  void setLog() { _logscale = true;  }
  void setLin() { _logscale = false; }

  // The field argument is temporary, until PdfBase::projection( field ) gets defined.
  void setField( const std::string& field ) { _field = field; }

  void draw    ( const std::string& file = "" ) const;

  const int    bin       ( const double& val ) const;
  const double binCenter ( const int&    bin ) const;
  const double binContent( const int&    bin ) const { return _binContent[ bin ]; };
  static const double binCenter( const int& bin, const unsigned& nbins, const double& min, const double& max );
};


#endif

