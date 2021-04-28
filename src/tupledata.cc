
#include <iostream>

#include <string>
#include <sstream>
#include <algorithm>

#include <root/TChain.h>
#include <root/TBranch.h>
#include <root/TIterator.h>

#include <rtools/tupledata.hh>

// This appears to just be a front end to interfacing with root - this effectively holds a root file, or a series of root files, and allows you to probe some very specific elements of them.
// This is markedly more complicated than just interfacing with root directly

// Constructor.
    TupleData::TupleData( const std::string& branch, const std::string& files )
: _chain( 0 )
{
    if ( files == "" )
        return;

    _chain = new TChain( branch.data() ); // branch is "gamma/kshh" which is the TDirectory/TTree address of the data TTree in the file. I assume this tells the TTree constructor to look for the data in this location, but idk why branch by itself wouldn't work
    _current = -1; // Just initialising this to a dummy value

    // Tokenize the elements of the files string to add them individually.
    std::istringstream stream( files ); // makes a ~vector of strings called stream, of all the files provided in the input argument "-i" at cmd line
    std::string file; // dummy var
    int nFiles = 0;
    std::cout << "Adding the following files:" << std::endl; // This is the first proper printout in the fitter, and it isn't even in the fitter...
    
    // Add the specified files to the chain.
    while( stream >> file ) // I usually only have one file so this should just do the one loop
    {
        std::cout << "   * " << file << std::endl; // just listing each file in a bulleted list 
        nFiles += _chain->Add( file.data() ); //adding whatever ttree is in the file in this list of files
    }
    setAddresses(); // this sets the branch address for every branch in the chain (i.e. associates a pointer of the correct type to the address in memory of the data in the TChain)

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
        // As each title should have the type as the final character, this figures out the type for each branch and makes an appropriate variable
        // _values is a std::map<std::string, *void>, whatever that means... Maybe a key-value pair?
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


const double TupleData::min( const std::string& varname, const double& def ) const
{
    if ( nEvt( varname ) )
        return _chain->GetMinimum( varname.data() );

    return def;
}


const double TupleData::max( const std::string& varname, const double& def ) const
{
    if ( nEvt( varname ) )
        return _chain->GetMaximum( varname.data() );

    return def;
}

