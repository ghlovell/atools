#ifndef __TUPLEDATA_HH__
#define __TUPLEDATA_HH__

#include <map>
#include <string>
#include <exception>

#include <root/TChain.h>
#include <root/TBranch.h>

// Exception to be thrown when a non-existing branch is requested.
class BranchException : public std::exception
{
    std::string _msg;

    public:
    BranchException( const std::string& msg ) : _msg( msg ) {};
    ~BranchException()                        throw() {};
    const char* what()                  const throw() { return _msg.c_str(); };
};


// Data buffer.
class TupleData
{
    private:
        int         _current;
        TChain*     _chain;
        std::map< std::string, void* > _values;

    public:
        TupleData( const std::string& branch, const std::string& files = "" );
        ~TupleData();

        void listBranches() const;

        // Configuration.
        void setAddresses(                    );
        int  getEntry    ( const long& entry  );

        // Getters.
        const unsigned        nEvt(                            ) const { return _chain->GetEntries();                 };
        const unsigned        nEvt( const std::string& varname ) const { return _chain->GetEntries( varname.data() ); };
        const double          min ( const std::string& varname, const double& def = 0.0 ) const;
        const double          max ( const std::string& varname, const double& def = 0.0 ) const;

        template< class T > T get ( const std::string& varname ) const throw( BranchException );
        template< class T > T get ( const std::string& varname, const T& defaultVal ) const;
};


// Get the value of the specified variable name.
template< class T > T TupleData::get( const std::string& varname ) const throw( BranchException )
{
    std::map< std::string, void* >::const_iterator entry = _values.find( varname );
    if ( entry == _values.end() )
        throw BranchException( "Data do not contain a " + varname + " field." );

    return *( (T*) entry->second );
}


// Get the value of the specified variable name.
template< class T > T TupleData::get( const std::string& varname, const T& defaultVal ) const
{
    std::map< std::string, void* >::const_iterator entry = _values.find( varname );
    if ( entry == _values.end() )
        return defaultVal;

    return *( (T*) entry->second );
}



// Template specialization for int arrays.
template<> inline int* TupleData::get< int* >( const std::string& varname ) const throw( BranchException )
{
    std::map< std::string, void* >::const_iterator entry = _values.find( varname );
    if ( entry == _values.end() )
        throw BranchException( "Data do not contain a " + varname + " field." );

    return (int*) entry->second;
}

// Template specialization for float arrays.
template<> inline float* TupleData::get< float* >( const std::string& varname ) const throw( BranchException )
{
    std::map< std::string, void* >::const_iterator entry = _values.find( varname );
    if ( entry == _values.end() )
        throw BranchException( "Data do not contain a " + varname + " field." );

    return (float*) entry->second;
}

// Template specialization for double arrays.
template<> inline double* TupleData::get< double* >( const std::string& varname ) const throw( BranchException )
{
    std::map< std::string, void* >::const_iterator entry = _values.find( varname );
    if ( entry == _values.end() )
        throw BranchException( "Data do not contain a " + varname + " field." );

    return (double*) entry->second;
}


#endif // __TUPLEDATA_HH__
