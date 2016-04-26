#include <atools/ConfigFile.hh>

using std::string;

ConfigFile::ConfigFile( string filename, string delimiter,
                        string comment, string sentry )
  : myDelimiter(delimiter), myComment(comment), mySentry(sentry)
{
  // Construct a ConfigFile, getting keys and values from given file

  std::ifstream in( filename.c_str() );

  if( !in ) throw file_not_found( filename );

  in >> (*this);
}


ConfigFile::ConfigFile()
  : myDelimiter( string(1,'=') ), myComment( string(1,'#') )
{
  // Construct a ConfigFile without a file; empty
}



ConfigFile::mapci ConfigFile::find( const std::string& section, const std::string& wildcard ) const
{
  // std::map< std::string, std::map< std::string, std::string > >::const_iterator secmap = myContents.at( section );

  std::map< std::string, std::map< std::string, std::string > >::const_iterator secmap = myContents.find( section );

  if ( secmap == myContents.end() )
    throw section_not_found( section );

  return std::find_if( secmap->second.begin(), secmap->second.end(),
                       [&]( std::pair< std::string, std::string > const& name )
                       {
                         return std::regex_match( name.first, std::regex( wildcard ) );
                       } );

  // return std::find_if( myContents.at( section ).begin(), myContents.at( section ).end(),
  //                      [&]( std::pair< std::string, std::string > const& name)
  //                      {
  //                        return std::regex_match( name.first, std::regex( wildcard ) );
  //                      } );
}


ConfigFile::mapci ConfigFile::find( const std::string& wildcard ) const
{
  return find( "default", wildcard );

  // return std::find_if( myContents[ "default" ].begin(), myContents[ "default" ].end(),
  //                      [&]( std::pair< std::string, std::string > const& name)
  //                      {
  //                        return std::regex_match( name.first, std::regex( wildcard ) );
  //                      } );
}



void ConfigFile::remove( const string& key )
{
  // Remove key and its value
  myContents.erase( myContents.find( key ) );
  return;
}


bool ConfigFile::keyExists( const std::string& section, const string& key ) const
{
  const std::map< std::string, std::string >& secmap = myContents.at( section );

  // Indicate whether key is found
  mapci p = secmap.find( key );
  return ( p != secmap.end() );
}



bool ConfigFile::keyExists( const string& key ) const
{
  // Indicate whether key is found in the default section.
  return keyExists( "default", key );
}


/* static */
void ConfigFile::trim( string& s )
{
  // Remove leading and trailing whitespace
  static const char whitespace[] = " \n\t\v\r\f";
  s.erase( 0, s.find_first_not_of(whitespace) );
  s.erase( s.find_last_not_of(whitespace) + 1U );
}


// Dump default section. Modify to dump whole file
std::ostream& operator<<( std::ostream& os, const ConfigFile& cf )
{
  // Save a ConfigFile to os
  for( ConfigFile::mapci p = cf.myContents.at( "default" ).begin();
       p != cf.myContents.at( "default" ).end();
       ++p )
    {
      os << p->first << " " << cf.myDelimiter << " ";
      os << p->second << std::endl;
    }
  return os;
}


std::istream& operator>>( std::istream& is, ConfigFile& cf )
{
  // Load a ConfigFile from is
  // Read in keys and values, keeping internal whitespace
  typedef string::size_type pos;
  const string& delim  = cf.myDelimiter;  // separator
  const string& comm   = cf.myComment;    // comment
  const string& sentry = cf.mySentry;     // end of file sentry
  const pos skip = delim.length();        // length of separator

  string nextline = "";  // might need to read ahead to see where value ends

  // Assign the contents to the default section by default.
  std::string section = "default";

  while( is || nextline.length() > 0 )
    {
      // Read an entire line at a time
      string line;
      if( nextline.length() > 0 )
	{
	  line = nextline;  // we read ahead; use it now
	  nextline = "";
	}
      else
	{
	  std::getline( is, line );
	}

      // Ignore comments
      line = line.substr( 0, line.find(comm) );

      // Check for end of file sentry
      if( sentry != "" && line.find(sentry) != string::npos ) return is;

      // Parse the section name if it is specified.
      if ( line.find( '[' ) == 0 )
      {
        const std::size_t& len = line.find( ']' ) - 1;
        section = line.substr( 1, len );
        continue;
      }

      // Parse the line if it contains a delimiter
      pos delimPos = line.find( delim );
      if( delimPos < string::npos )
	{
	  // Extract the key
	  string key = line.substr( 0, delimPos );
	  line.replace( 0, delimPos+skip, "" );

	  // See if value continues on the next line
	  // Stop at blank line, next line with a key, end of stream,
	  // or end of file sentry
	  bool terminate = false;
	  while( !terminate && is )
	    {
	      std::getline( is, nextline );
	      terminate = true;

	      string nlcopy = nextline;
	      ConfigFile::trim(nlcopy);
	      if( nlcopy == "" ) continue;

	      nextline = nextline.substr( 0, nextline.find(comm) );
	      if( nextline.find(delim) != string::npos )
		continue;
	      if( sentry != "" && nextline.find(sentry) != string::npos )
		continue;

	      nlcopy = nextline;
	      ConfigFile::trim(nlcopy);
	      if( nlcopy != "" ) line += "\n";
	      line += nextline;
	      terminate = false;
	    }

	  // Store key and value
	  ConfigFile::trim(key);
	  ConfigFile::trim(line);
	  cf.myContents[section][key] = line;  // overwrites if key is repeated
	}
    }

  return is;
}
