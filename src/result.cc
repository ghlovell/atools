
#include <cfit/parameter.hh>

#include <atools/ConfigFile.hh>

#include <atools/result.hh>
#include <atools/utils.hh>


Result::Result( const std::string& file )
{
  ConfigFile result;

  try
  {
    result = ConfigFile( file, "=", "------" );
  }
  catch( ConfigFile::file_not_found& fnf )
  {
    std::cout << "File " << fnf.filename << " does not exist." << std::endl;
    std::cout << "Working with default configuration." << std::endl;
    return;
  }

  typedef std::map< std::string, std::string >::const_iterator mIter;
  const std::map< std::string, std::string > content = result.content();

  for( mIter entry = content.begin(); entry != content.end(); ++entry )
    _pars[ entry->first ] = Utils::makePar( entry->first, entry->second );
}
