#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#define VERSION "1.0.0"

enum ArgMode
{
    AM_NONE,
    AM_SOURCE_DIR,
    AM_INTUICIO_PATH,
    AM_JAEGER_PREPROCESSOR_PATH,
    AM_ASSEMBLER_OUTPUT
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

std::string getpwd()
{
    char buff[2048];
    #ifdef BUILD_WIN
    std::string result = _getcwd( buff, 2048 );
    #else
    std::string result = getcwd( buff, 2048 );
    #endif
    return result;
}

int main( int argc, char* argv[] )
{
    ArgMode argMode = AM_NONE;
    std::string arg;
    std::vector< std::string > sourceDirs;
    std::string intuicioPath = "intuicio";
    std::string jaegerPreprocessorPath = "I4Jaeger";
    bool profile = false;
    std::string assemblerOutputPath;
    std::vector< std::string > a;
    auto cstdPath = getenv( "JAEGER_STD" );
    std::string stdPath = cstdPath ? cstdPath : "";
    if( stdPath.empty() )
    {
        std::cout << "There is no registered JAEGER_STD environment variable that specify Jaeger standard libraries path!" << std::endl;
        return 1;
    }
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
            else if( arg == "-ao" || arg == "--assembler-output" )
                argMode = AM_ASSEMBLER_OUTPUT;
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
        else if( argMode == AM_ASSEMBLER_OUTPUT )
        {
            assemblerOutputPath = arg;
            argMode = AM_NONE;
        }
    }
    #ifdef BUILD_WIN
    intuicioPath = string_replace( intuicioPath, "\\", "\\\\" );
    jaegerPreprocessorPath = string_replace( jaegerPreprocessorPath, "\\", "\\\\" );
    stdPath = string_replace( stdPath, "\\", "\\\\" );
    assemblerOutputPath = string_replace( assemblerOutputPath, "\\", "\\\\" );
    #endif
    sourceDirs.push_back( stdPath );
    sourceDirs.push_back( getpwd() + "/" );
    std::stringstream ss;
    ss << intuicioPath;
    for( auto& v : a )
        ss << " " << v;
    ss << ( profile ? " --profile" : "" );
    if( assemblerOutputPath.empty() )
    {
        ss << " -epp " << jaegerPreprocessorPath << ( profile ? " --profile" : "" ) << " -sd \"" << stdPath << "\"";
        for( auto& sd : sourceDirs )
            ss << " -sd \"" << sd << "\"";
        ss << " -- -ep I4Run -mcs 8" << ( profile ? " --profile" : "" );
    }
    else
    {
        ss << " -epp " << jaegerPreprocessorPath << ( profile ? " --profile" : "" ) << " -sd \"" << stdPath << "\"";
        for( auto& sd : sourceDirs )
            ss << " -sd \"" << sd << "\"";
        ss << " -ao \"" << assemblerOutputPath << "\"";
    }
    #ifdef DEBUG
    std::cout << ss.str() << std::endl;
    #endif
    return system( ss.str().c_str() );
}
