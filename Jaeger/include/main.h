#ifndef __MAIN_H__
#define __MAIN_H__

#include <i4/compilation_state.h>
#include <string>

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

int DLL_EXPORT onPreProcess( I4::CompilationStatePtr& outState, const std::string& inputPath, const std::vector< std::string >& args );

void DLL_EXPORT offPreProcess( I4::CompilationStatePtr& state );

bool DLL_EXPORT checkSyntax( const std::string& inputPath );

#ifdef __cplusplus
}
#endif

#endif
