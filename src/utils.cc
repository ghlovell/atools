
#include <cmath>

#include <sstream>
#include <iomanip>
#include <vector>

#include <Minuit/FunctionMinimum.h>
#include <Minuit/MinuitParameter.h>
#include <Minuit/MnContours.h>

#include <cfit/parameter.hh>
#include <cfit/coef.hh>
#include <cfit/minimizerexpr.hh>
#include <cfit/amplitude.hh>

#include <atools/ConfigFile.hh>
#include <atools/blind.hh>
#include <atools/utils.hh>
#include <atools/math.hh>



// Compute the Jenkins one-at-a-time hash of an input string.
const unsigned Utils::hash( const std::string& input )
{
    unsigned h = 0;

    for ( std::string::const_iterator c = input.begin(); c != input.end(); ++c )
    {
        h += *c;
        h += ( h << 10 );
        h ^= ( h >> 6 );
    }

    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );

    return h;
}


// Calculate a hash based on all the parameter values of an amplitude.
const unsigned Utils::hash( const Amplitude& amp )
{
    const std::map< std::string, Parameter >& pars = amp.getPars();

    std::ostringstream str;
    for ( auto par : pars )
        str << std::setprecision( 10 ) << par.first << par.second.value();

    return hash( str.str() );
}




// Replace all the occurrences of a pattern for the specified replacement.
const std::string Utils::replace( const std::string& str, const std::string& pattern, const std::string& replacement )
{
    std::string::size_type pos = 0;
    std::string result = str;

    while ( ( pos = result.find( pattern, pos ) ) != std::string::npos )
    {
        result = result.replace( pos, pattern.size(), replacement );
        pos += replacement.size();
    }

    return result;
}


const std::string Utils::conjugate( const std::string& str )
{
    std::string result = str;

    for ( std::string::iterator ch = result.begin(); ch != result.end(); ++ch )
    {
        if      ( *ch == '+' ) *ch = '-';
        else if ( *ch == '-' ) *ch = '+';
    }

    return result;
}


// Return the charge of the latest instance + or -.
const std::string Utils::charge( const std::string& str )
{
    std::string result = "0";

    for ( std::string::const_iterator ch = str.begin(); ch != str.end(); ++ch )
        if ( *ch == '+' || *ch == '-' ) result = *ch;

    return result;
}


const std::string Utils::nosigns( const std::string& str )
{
    std::string result = str;
    result = Utils::replace( result, "+", "" );
    result = Utils::replace( result, "-", "" );

    return result;
}


bool Utils::startsWith( const std::string& str, const std::string& start )
{
    if ( str.length() < start.length() )
        return false;

    return str.compare( 0, start.length(), start ) == 0;
}


bool Utils::endsWith( const std::string& str, const std::string& end )
{
    if ( str.length() < end.length() )
        return false;

    return str.compare( str.length() - end.length(), end.length(), end ) == 0;
}


bool Utils::contains( const std::string& str, const std::string& substr )
{
    return str.find( substr ) != std::string::npos;
}


const int Utils::precision( const double& value, const int& digits )
{
    return digits - 2 + 2 * ( std::abs( value ) > 1 ) - ( value < 0.0 ) - std::abs( std::ceil( std::log10( std::abs( value ) ) ) );
}


const std::string Utils::getOutput( const FunctionMinimum& min )
{
    const bool&   valid  = min.isValid();            // True only if minuit converged.
    const bool&   hascov = min.hasValidCovariance(); // True only if the covariance matrix is valid.

    const int&    nfcn   = min.nfcn(); // Number of function calls.
    const double& fval   = min.fval(); // Minimum function value.
    const double& edm    = min.edm();  // Minimum edm.

    std::ostringstream os;
    //   os <<"minimum internal state vector: "<<min.parameters().vec()<<std::endl;
    //   if(min.hasValidCovariance()) 
    //     os <<"minimum internal covariance matrix: "<<min.error().matrix()<<std::endl;

    os << "---------------------------------------------------------------------------" << std::endl;
    os << std::left << std::setw( 22 ) << "valid"    << " = " << valid                  << std::endl;
    os << std::left << std::setw( 22 ) << "validcov" << " = " << hascov                 << std::endl;
    os << std::left << std::setw( 22 ) << "nfcn"     << " = " << nfcn                   << std::endl;
    os << std::left << std::setw( 22 ) << "fval"     << " = " << fval                   << std::endl;
    os << std::left << std::setw( 22 ) << "edm"      << " = " << edm                    << std::endl;
    os << "---------------------------------------------------------------------------" << std::endl;
    os << getOutput( min.userParameters() );
    os << "---------------------------------------------------------------------------" << std::endl;
    if ( hascov )
        os << getOutput( min.userParameters(), min.userCovariance() );
    //   os << "-------------------------------------------------------" << std::endl;
    //   os << min.userState().globalCC() << std::endl;

    return os.str();
}



const std::string Utils::getOutput( const MnUserParameters& pars )
{
    double value = 0.0;
    double error = 0.0;
    bool   fixed = false;
    bool   blind = false;

    double loLimit = 0.0;
    double upLimit = 0.0;
    bool   atLoLimit = false;
    bool   atUpLimit = false;

    const std::vector< MinuitParameter >& minuitPars = pars.parameters();

    // Get the length of the longest parameter name.
    unsigned namelen = 0;
    typedef std::vector< MinuitParameter >::const_iterator pIter;
    for ( pIter par = minuitPars.begin(); par != minuitPars.end(); ++ par )
        namelen = std::max( namelen, (unsigned) std::string( par->name() ).size() );

    const unsigned& width = 20;

    std::ostringstream os;

    std::streamsize defaultPrec = os.precision();

    Blind blinder( 4 );

    for ( pIter par = minuitPars.begin(); par != minuitPars.end(); ++ par )
    {
        value = par->value();
        error = par->error();

        fixed = ( par->isConst() || par->isFixed() );
        blind = par->isBlind();

        os << std::left << std::setw( namelen ) << par->name() << " = ";
        os << std::left << std::setw( width );

        if( fixed )
        {
            if ( ! blind )
                os << std::setprecision( defaultPrec ) << value;
            else
                os                                     << blinder.blind( value );
            os << "    " << std::left << std::setw( width ) << " " << " C";
        }
        else
        {
            if ( ! blind )
                os << std::setprecision( precision( value, width ) ) << value;
            else
                os                                                   << blinder.blind( value );

            // Print the error if any.
            if ( error > 0.0 )
                os << " +- " << std::left << std::setw( width ) << std::setprecision( precision( error, width ) ) << error;
            else
                os << " +- " << std::left << std::setw( width ) << "no";
        }

        // Set the precision back to default.
        os.precision( defaultPrec );

        // Print a B if the parameter is set to be blind.
        if ( blind )
        {
            os << std::setw( 2 * ( ! fixed ) + 1 ) << " ";
            os << "B";
        }

        if( par->hasLimits() )
        {
            loLimit   = par->lowerLimit();
            upLimit   = par->upperLimit();
            atLoLimit = std::abs( value - loLimit ) < pars.precision().eps2();
            atUpLimit = std::abs( value - upLimit ) < pars.precision().eps2();

            // Print the values of the limits, accompanied with a * if the result is at one of them.
            os << std::setw( 4 * ( ! fixed && ! blind ) + 2 * ( fixed && ! blind ) + 1 ) << " ";
            os << "L( "
                << ( atLoLimit ? "*" : "" ) << loLimit << ", "
                << ( atUpLimit ? "*" : "" ) << upLimit << " )";
        }

        os << std::endl;
    }

    return os.str();
}



const std::string Utils::getOutput( const MnUserParameters& pars, const MnUserCovariance& cov )
{
    // Length of the longest floating parameter name.
    unsigned namelen = 0;

    const unsigned& width = 30;

    // Obtain the vector of names of floating parameters.
    std::string name;
    std::vector< std::string > names;
    typedef std::vector< MinuitParameter >::const_iterator pIter;
    for( pIter par = pars.parameters().begin(); par != pars.parameters().end(); ++par )
    {
        bool fixed = ( par->isConst() || par->isFixed() );

        if ( ! fixed )
        {
            name = par->name();
            namelen = std::max( namelen, (unsigned) name.size() );
            names.push_back( name );
        }
    }

    std::ostringstream os;
    const unsigned nrow = cov.nrow();
    for ( unsigned r = 0; r < nrow; ++r )
        for ( unsigned c = r; c < nrow; ++c )
            os << "cov( "
                << std::left << std::setw( namelen ) << names[ r ] << " , "
                << std::left << std::setw( namelen ) << names[ c ] << " ) = "
                << std::left << std::setw( width )
                << std::setprecision( precision( cov( r, c ), width ) )
                << cov( r, c ) << std::endl;

    return os.str();
}


const Parameter Utils::makePar( const std::string& name, std::string info )
{
    info = Utils::replace( info, "(", " (" ); // Make the L a different token.
    info = Utils::replace( info, ",", " "  ); // Remove commas from ranges.
    info = Utils::replace( info, "*", " "  ); // Remove * from ranges, indicating
    //    result at the limit.
    double value   =  0.0;
    double error   = -1.0;
    double loLimit =  0.0;
    double upLimit =  0.0;
    bool   isFixed = false;
    bool   isBlind = false;
    bool   limited = false;
    bool   unset   = true;  // To check if value has been set.

    // Tokenize the resonance information.
    std::istringstream tokens( info );
    std::string token;
    while( tokens >> token )
    {
        if ( token == "C" )
            isFixed = true;
        else if ( token == "B" )
            isBlind = true;
        else if ( token == "L" )
        {
            tokens >> token >> loLimit >> upLimit; // token will be '('.
            limited = true;
        }
        else if ( token == "+-" )
            tokens >> error;
        else if ( token == "-" )
        {
            tokens >> value;
            value *= -1.0;
            unset = false;
        }
        else if ( unset )
        {
            value = Utils::fromStr< double >( token );
            unset = false;
        }
        else if ( token != ")" )
        {
            std::cerr << "\e[91mDon't know what to do with token " << token << "\e[0m" << std::endl;
            std::cerr << "\e[91mExpression \"\e[1m" << info << "\e[21m\" is not a parameter initialization.\e[0m" << std::endl;;
            throw std::exception();
        }
    }

    if ( unset )
        std::cout << "Something is wrong with parameter " << name << ", which is unset." << std::endl;

    // Create the parameter to be returned.
    Parameter par( name, value, error );

    // Fix, blind and limit if requested.
    if ( isFixed )
        par.fix();
    if ( isBlind )
        par.blind();
    if ( limited )
        par.setLimits( loLimit, upLimit );

    return par;
}



// Create a cfit Parameter from a configuration file and the
//    parameter name to be read from it.
const Parameter Utils::makePar( const ConfigFile& config, const std::string& name )
{
    const std::string& prefix = config.read< std::string >( "prefix", "" );
    const std::string& suffix = config.read< std::string >( "suffix", "" );

    std::string parname;
    parname += prefix + ( prefix.empty() ? "" : "_" );
    parname += name;
    parname += ( suffix.empty() ? "" : "_" ) + suffix;

    return makePar( parname, config.read< std::string >( name ) );
}



// Create a cfit Parameter from a configuration file and the
//    parameter name to be read from it. This is the one used in fitter.cc
const Parameter Utils::makePar( const ConfigFile& config, const std::string& section, const std::string& name )
{
    // No need to catch exceptions. If any of these section readings throws ConfigFile::section_not_found,
    //    this function would throw anyway at the return line.
    const std::string& prefix = config.readSection< std::string >( section, "prefix", "" );
    const std::string& suffix = config.readSection< std::string >( section, "suffix", "" );

    std::string parname;
    parname += prefix + ( prefix.empty() ? "" : "_" );
    parname += name;
    parname += ( suffix.empty() ? "" : "_" ) + suffix;

    return makePar( parname, config.readSection< std::string >( section, name ) );
}



// Create a cfit Parameter from a configuration file and the
//    parameter name to be read from it.
const Parameter Utils::makePar( const ConfigFile& config, const std::string& section, const std::string& name, const std::string& suffix )
{
    return makePar( name + ( suffix != "" ? "_" : "" ) + suffix, config.readSection< std::string >( section, name ) );
}



const Coef Utils::makeCoef( const ConfigFile& config, const std::string& re, const std::string& im )
{
    return Coef( makePar( config, re ), makePar( config, im ) );
}



const double Utils::residual( const double& datum, const double& pdf )
{
    double chi2 = 0.;

    if ( pdf > 0 )
        chi2 += 2. * ( pdf - datum );

    if ( datum > 0 && pdf > 0 )
        chi2 += 2. * datum * std::log( datum / pdf );

    return ( ( datum >= pdf ) ? std::sqrt( chi2 ) : -std::sqrt( chi2 ) );
}



void Utils::contour( const unsigned& sigmas,
        const std::string& outnameM, const std::string& outnameP,
        MinimizerExpr& nlls, const FunctionMinimum& min )
{
    // Find each parameter's index and value.
    const std::vector< MinuitParameter >& minuitPars = min.userParameters().parameters();
    typedef std::vector< MinuitParameter >::const_iterator pIter;
    std::map< const std::string, unsigned > parIdx;
    std::map< const std::string, double >   parVal;
    for ( pIter par = minuitPars.begin(); par != minuitPars.end(); ++par )
    {
        parIdx[ par->name() ] = par->number();
        parVal[ par->name() ] = par->value();
    }

    // Set the error definition to produce the requested contour.
    nlls.setUp( Math::chiSqLevel( sigmas ) );

    // Obtain the vectors of the contours.
    MnContours contours( nlls, min );
    std::vector< std::pair< double, double > > pointsM = contours( parIdx[ "xm" ], parIdx[ "ym" ] );
    std::vector< std::pair< double, double > > pointsP = contours( parIdx[ "xp" ], parIdx[ "yp" ] );

    // Go back to the original error definition.
    nlls.setUp( 1.0 );

    typedef std::vector< std::pair< double, double > >::const_iterator cIter;

    // Save the (x,y)_- points.
    std::ofstream outputM( outnameM );
    for ( cIter point = pointsM.begin(); point != pointsM.end(); ++point )
        outputM << point->first - parVal[ "xm" ] << " " << point->second - parVal[ "ym" ] << std::endl;

    // Save the (x,y)_+ points.
    std::ofstream outputP( outnameP );
    for ( cIter point = pointsP.begin(); point != pointsP.end(); ++point )
        outputP << point->first - parVal[ "xp" ] << " " << point->second - parVal[ "yp" ] << std::endl;
}
