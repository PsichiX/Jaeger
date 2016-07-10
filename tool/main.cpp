#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define VERSION "1.0.0"

enum ArgMode
{
    AM_NONE,
    AM_INTUICIO_PATH,
    AM_JAEGER_PREPROCESSOR_PATH
};

int main( int argc, char* argv[] )
{
    ArgMode argMode = AM_NONE;
    std::string arg;
    std::string intuicioPath = "intuicio";
    std::string jaegerPreprocessorPath = "I4Jaeger";
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
            #ifdef DEBUG
            else if( arg == "-ip" || arg == "--intuicio-path" )
                argMode = AM_INTUICIO_PATH;
            else if( arg == "-jpp" || arg == "--jaeger-preprocessor-path" )
                argMode = AM_JAEGER_PREPROCESSOR_PATH;
            #endif
            else
                a.push_back( arg );
        }
        #ifdef DEBUG
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
        #endif
    }
    std::stringstream ss;
    ss << intuicioPath << " -epp " << jaegerPreprocessorPath << " --";
    for( auto& v : a )
        ss << " " << v;
    ss << " -ep I4Run -mcs 8";
    return system( ss.str().c_str() );
}
