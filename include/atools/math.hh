
class Math
{
private:
  static const double invnormaldistribution( const double& y0 );
  static const double lngamma( double x );
  static const double incompletegamma    ( const double& a, const double& x  );
  static const double incompletegammac   ( const double& a, const double& x  );
  static const double invincompletegammac( const double& a, const double& y0 );
  static const double invchisquaredistribution( const double& v, const double& y );

public:
  static const double errorLo( const double& n, const double& cl = 0.682689492137085963 );
  static const double errorHi( const double& n, const double& cl = 0.682689492137085963 );
};

