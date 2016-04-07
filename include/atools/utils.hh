#ifndef __UTILS_HH__
#define __UTILS_HH__

#include <string>
#include <iomanip>

#include <atools/blind.hh>

class Amplitude;
class ConfigFile;
class FunctionMinimum;
class MnUserParameters;
class MnUserCovariance;
class Parameter;
class Coef;
class MinimizerExpr;
class FunctionMinimum;

class Utils
{
public:
  template <class T>
  static T fromStr( const std::string& str )
  {
    std::istringstream tempStr( str );
    T num;
    if ( tempStr >> num )
      return num;

    return Blind( 4 ).unblind< double >( str );
  }

  template <class T>
  static std::string toStr( const T& val, const int& digits = 0 )
  {
    std::ostringstream tempStr;
    if ( digits )
      tempStr << std::setfill( '0' ) << std::setw( digits );

    if ( tempStr << val )
      return tempStr.str();

    return "";
  }


  static const unsigned hash( const std::string& input );
  static const unsigned hash( const Amplitude&   amp   );

  static const std::string replace( const std::string& str, const std::string& pattern, const std::string& replacement );
  static const std::string conjugate( const std::string& str );


  static const int         precision( const double& value, const int& digits = 20 );
  static const std::string getOutput( const FunctionMinimum&  min  );
  static const std::string getOutput( const MnUserParameters& pars );
  static const std::string getOutput( const MnUserParameters& pars, const MnUserCovariance& cov );

  static const Parameter   makePar ( const std::string& name, std::string info        );
  static const Parameter   makePar ( const ConfigFile&  file, const std::string& name );
  static const Coef        makeCoef( const ConfigFile&  name, const std::string& re  , const std::string& im );

  static const double      residual( const double& datum, const double& pdf );

  static void              contour( const unsigned& sigmas,
                                    const std::string& outnameM, const std::string& outnameP,
                                    MinimizerExpr& nlls, const FunctionMinimum& min );
};


#endif
