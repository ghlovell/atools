#ifndef __RESULT_HH__
#define __RESULT_HH__

#include <map>
#include <string>

#include <cfit/parameter.hh>

class Result
{
private:
  std::map< std::string, Parameter > _pars;
public:
  Result( const std::string& file );
  const std::map< std::string, Parameter >& parameters() const { return _pars; }
};

#endif
