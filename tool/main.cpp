#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define VERSION "1.0.0"

enum ArgMode
{
    AM_NONE,
    AM_SOURCE_DIR,
    AM_INTUICIO_PATH,
    AM_JAEGER_PREPROCESSOR_PATH
};

std::string string_replace( const std::string& val, const std::string& oldval, const std::string& newval )
{
    if( val.empty() )
        return val;
    std::string r( val );
    size_t oldlen = oldval.length();
    size_t newlen = newval.length();
    size_t pos = r.find( oldval );
    while( pos != std::string::npos )
    {
        r = r.replace( pos, oldlen, newval );
        pos = r.find( oldval, pos + newlen );
    }
    return r;
}

int main( int argc, char* argv[] )
{
    ArgMode argMode = AM_NONE;
    std::string arg;
    std::vector< std::string > sourceDirs;
    std::string intuicioPath = "intuicio";
    std::string jaegerPreprocessorPath = "I4Jaeger";
    bool profile = false;
    std::vector< std::string > a;
    for( int i = 1; i < argc; ++i )
    {
        arg = argv[i];
        if( argMode == AM_NONE )
        {
            if( arg == "-v" || arg == "--version" )
            {
                std::cout << "Jaeger v" << VERSION << std::endl;
                system( "intuicio -v" );
                return 0;
            }
            else if( arg == "-sd" || arg == "--source-dir" )
                argMode = AM_SOURCE_DIR;
            else if( arg == "-ip" || arg == "--intuicio-path" )
                argMode = AM_INTUICIO_PATH;
            else if( arg == "-jpp" || arg == "--jaeger-preprocessor-path" )
                argMode = AM_JAEGER_PREPROCESSOR_PATH;
            else if( arg == "-p" || arg == "--profile" )
                profile = true;
            else
                a.push_back( arg );
        }
        else if( argMode == AM_SOURCE_DIR )
        {
            sourceDirs.push_back( arg );
            argMode = AM_NONE;
        }
        else if( argMode == AM_INTUICIO_PATH )
        {
            intuicioPath = arg;
            argMode = AM_NONE;
        }
        else if( argMode == AM_JAEGER_PREPROCESSOR_PATH )
        {
            jaegerPreprocessorPath = arg;
            argMode = AM_NONE;
        }
    }
    std::string stdPath = getenv( "JAEGER_STD" );
    if( stdPath.empty() )
    {
        std::cout << "There is no registered JAEGER_STD environment variable that specify Jaeger standard libraries path!" << std::endl;
        return 1;
    }
    #ifdef BUILD_WIN
    intuicioPath = string_replace( intuicioPath, "\\", "\\\\" );
    jaegerPreprocessorPath = string_replace( jaegerPreprocessorPath, "\\", "\\\\" );
    stdPath = string_replace( stdPath, "\\", "\\\\" );
    #endif
    std::stringstream ss;
    ss << intuicioPath;
    for( auto& v : a )
        ss << " " << v;
    ss << ( profile ? " --profile" : "" ) << " -epp " << jaegerPreprocessorPath << ( profile ? " --profile" : "" ) << " -sd \"" << stdPath << "\"";
    for( auto& sd : sourceDirs )
        ss << " -sd \"" << sd << "\"";
    ss << " -- -ep I4Run -mcs 8" << ( profile ? " --profile" : "" );
    #ifdef DEBUG
    std::cout << ss.str() << std::endl;
    #endif
    return system( ss.str().c_str() );
}
