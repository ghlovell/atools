
#include <iostream>

#include <string>
#include <sstream>
#include <algorithm>

#include <root/TChain.h>
#include <root/TBranch.h>
#include <root/TIterator.h>

#include <rtools/tupledata.hh>

// Constructor.
TupleData::TupleData( const std::string& branch, const std::string& files )
  : _chain( 0 )
{
  if ( files == "" )
    return;

  _chain = new TChain( branch.data() );
  _current = -1;

  // Tokenize the elements of the files string to add them individually.
  std::istringstream stream( files );
  std::string file;
  int nFiles = 0;
  std::cout << "Adding the following files:" << std::endl;

  // Add the specified files to the chain.
  while( stream >> file )
  {
    std::cout << "   * " << file << std::endl;
    nFiles += _chain->Add( file.c_str() );
  }
  setAddresses();

  std::cout << "Added " << nFiles << " files with " << _chain->GetEntries() << " events." << std::endl;
}


// Destructor
TupleData::~TupleData()
{
  if ( _chain )
    delete _chain;
}


// Print the list of branches.
void TupleData::listBranches() const
{
  // Iterate over all the branches.
  TIter iter( _chain->GetListOfBranches() );

  while( TBranch* branch = (TBranch*) iter() )
    std::cout << branch->GetName() << "     \t" << branch->GetTitle() << std::endl;
}


// Run SetBranchAddress for every branch in the chain.
void TupleData::setAddresses()
{
  if ( ! _chain )
    return;

  // Iterate over all the branches.
  TIter iter( _chain->GetListOfBranches() );

  while( TBranch* branch = (TBranch*) iter() )
  {
    const std::string& name  = branch->GetName();
    const std::string& title = branch->GetTitle();

    // Do not accept array fields.
    if ( title.find( "[" ) != std::string::npos )
      throw BranchException( "Refusing to handle the array branch " + name + "." );

    // Allocate space for an array of the required length for array branches.
    if ( *( title.rbegin() ) == std::string( "O" ) )
      _values[ name.c_str() ] = new bool;
    if ( *( title.rbegin() ) == std::string( "B" ) )
      _values[ name.c_str() ] = new char;
    if ( *( title.rbegin() ) == std::string( "b" ) )
      _values[ name.c_str() ] = new unsigned char;
    if ( *( title.rbegin() ) == std::string( "S" ) )
      _values[ name.c_str() ] = new short;
    if ( *( title.rbegin() ) == std::string( "s" ) )
      _values[ name.c_str() ] = new unsigned short;
    if ( *( title.rbegin() ) == std::string( "I" ) )
      _values[ name.c_str() ] = new int;
    if ( *( title.rbegin() ) == std::string( "i" ) )
      _values[ name.c_str() ] = new unsigned int;
    if ( *( title.rbegin() ) == std::string( "L" ) )
      _values[ name.c_str() ] = new long;
    if ( *( title.rbegin() ) == std::string( "l" ) )
      _values[ name.c_str() ] = new unsigned long;
    if ( *( title.rbegin() ) == std::string( "F" ) )
      _values[ name.c_str() ] = new float;
    if ( *( title.rbegin() ) == std::string( "D" ) )
      _values[ name.c_str() ] = new double;

    // Link the pointer to the variable name to its corresponding branch in the chain.
    _chain->SetBranchAddress( name.c_str(), _values[ name.c_str() ] );
  }
}


// Fill the object with specified entry.
int TupleData::getEntry( const long& entry )
{
  // Set the environment to read one entry
  if ( ! _chain )
    return 0;

  long centry = _chain->LoadTree( entry );
  if ( ( centry < 0 ) || ( _chain->IsA() != TChain::Class() ) )
    return 0;

  if ( _chain->GetTreeNumber() != _current )
    _current = _chain->GetTreeNumber();

  // Read contents of specified entry and return the entry number.
  return _chain->GetEntry( entry );
}

