#ifndef __BUILDER_H__
#define __BUILDER_H__

#include <i4/compilation_state.h>
#include <i4/vm.h>
#include <vector>
#include <map>
#include <stack>
#include "program.h"

class Builder
{
public:
    enum ArgMode
    {
        AM_NONE,
        AM_SOURCE_DIR,
        AM_STACK_SIZE,
        AM_ASM_OUTPUT
    };

    static bool implementTemplate( ProgramPtr& program, const std::string& content, const std::vector< std::string >& types );

    Builder( const std::vector< std::string >& args );
    ~Builder();

    bool canWriteAsmToFile() { return !m_asmOutput.empty(); }
    bool canProfile() { return m_profile; }

    void pushDirectory( const std::string& path );
    void popDirectory();
    bool checkSyntax( const std::string& inputPath );
    ProgramPtr buildProgram( const std::string& inputPath, bool isImport = false );
    Program::FunctionPtr makeFunction( const std::string& content );
    I4::CompilationStatePtr buildCompilationState( const std::string& inputPath );
    void writeAsmToFile( const std::string& content );

private:
    std::string loadFile( const std::string& path );
    std::string getPathDir( const std::string& path );

    std::stack< std::string > m_dirs;
    I4::VM::LoadFileListenerPtr m_loadFileListener;
    bool m_printPST;
    std::size_t m_stackSize;
    std::string m_asmOutput;
    bool m_profile;
};

#endif
