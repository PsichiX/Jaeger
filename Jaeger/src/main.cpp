#include "../include/main.h"
#include "../include/builder.h"

int DLL_EXPORT onPreProcess( I4::CompilationStatePtr& outState, const std::string& inputPath, const std::vector< std::string >& args )
{
    Builder builder( args );
    outState = builder.buildCompilationState( inputPath );
    return outState ? 0 : 1;
}

void DLL_EXPORT offPreProcess( I4::CompilationStatePtr& state )
{
    state.reset();
}

bool DLL_EXPORT checkSyntax( const std::string& inputPath )
{
    std::vector< std::string > args;
    Builder builder( args );
    return builder.checkSyntax( inputPath );
}
