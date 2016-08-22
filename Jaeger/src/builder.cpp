#include "../include/builder.h"
#include "../include/grammar.h"
#include "../include/compiler_errors.h"
#include "../include/compiler_actions.h"
#include <i4/vm.h>
#include <pegtl/analyze.hh>
#include <exception>
#include <regex>
#include <fstream>
#include <chrono>

bool Builder::implementTemplate( ProgramPtr& program, const std::string& content, const std::vector< std::string >& types )
{
    std::string c = program->preprocessTemplate( content, types );
    pegtl::analyze< Grammar::grammar_template_body >();
    return pegtl::parse< Grammar::grammar_template_body, CompilerActions::actions, Errors::control >( c, "<template>", program );
}

Builder::Builder( const std::vector< std::string >& args )
: m_printPST( false )
, m_stackSize( 8192 )
, m_profile( false )
{
    ArgMode argMode = AM_NONE;
    std::vector< std::string > sourceDirs;
    for( auto& arg : args )
    {
        if( argMode == AM_NONE )
        {
            if( arg == "-sd" || arg == "--source-dir" )
                argMode = AM_SOURCE_DIR;
            else if( arg == "-p" || arg == "--print" )
                m_printPST = true;
            else if( arg == "-ss" || arg == "--stack-size" )
                argMode = AM_STACK_SIZE;
            else if( arg == "-ao" || arg == "--asm-output" )
                argMode = AM_ASM_OUTPUT;
            else if( arg == "-r" || arg == "--profile" )
                m_profile = true;
        }
        else if( argMode == AM_SOURCE_DIR )
        {
            sourceDirs.push_back( arg );
            argMode = AM_NONE;
        }
        else if( argMode == AM_STACK_SIZE )
        {
            std::stringstream ss( arg );
            ss >> m_stackSize;
            argMode = AM_NONE;
        }
        else if( argMode == AM_ASM_OUTPUT )
        {
            m_asmOutput = arg;
            argMode = AM_NONE;
        }
    }
    m_loadFileListener = I4::VM::LoadFileListenerPtr( new I4::VM::LoadFileListener( sourceDirs ) );
}

Builder::~Builder()
{
}

void Builder::pushDirectory( const std::string& path )
{
    std::string pd = getPathDir( path );
    m_dirs.push( m_dirs.empty() ? pd : ( m_dirs.top() + pd ) );
}

void Builder::popDirectory()
{
    m_dirs.pop();
}

std::string Builder::loadFile( const std::string& path )
{
    if( !m_dirs.empty() )
    {
        try
        {
            return m_loadFileListener->onLoadInput( m_dirs.top() + path );
        }
        catch( const std::exception& ex ) {}
    }
    return m_loadFileListener->onLoadInput( path );
}

std::string Builder::getPathDir( const std::string& path )
{
    std::size_t found = path.rfind( '/' );
    if( found == std::string::npos )
        found = path.rfind( '\\' );
    if( found == std::string::npos )
        return "";
    return path.substr( 0, found ) + "/";
}

bool Builder::checkSyntax( const std::string& inputPath )
{
    pegtl::analyze< Grammar::grammar >();
    try
    {
        std::string input = loadFile( inputPath );
        return input.empty() || pegtl::parse< Grammar::grammar, pegtl::nothing, Errors::control >( input, inputPath );
    }
    catch( const pegtl::parse_error& ex )
    {
        std::cout << ex.what() << std::endl;
        return false;
    }
    catch( const std::exception& ex )
    {
        std::cout << ex.what() << std::endl;
        return false;
    }
}

ProgramPtr Builder::buildProgram( const std::string& inputPath, bool isImport )
{
    pegtl::analyze< Grammar::grammar >();
    ProgramPtr program( new Program( inputPath ) );
    try
    {
        std::string input = loadFile( inputPath );
        if( input.empty() )
            return program;
        if( pegtl::parse< Grammar::grammar, CompilerActions::actions, Errors::control >( input, inputPath, program ) )
        {
            if( program )
            {
                pushDirectory( getPathDir( inputPath ) );
                program->loadImports( this );
                popDirectory();
                if( !isImport )
                    program->validate( this );
                if( m_printPST && !isImport )
                    program->write( std::cout );
                return program;
            }
            return nullptr;
        }
        return nullptr;
    }
    catch( const pegtl::parse_error& ex )
    {
        std::cout << ex.what() << std::endl;
        return nullptr;
    }
    catch( const std::exception& ex )
    {
        std::cout << ex.what() << std::endl;
        return nullptr;
    }
    return nullptr;
}

Program::FunctionPtr Builder::makeFunction( const std::string& content )
{
    pegtl::analyze< Grammar::grammar_function_definition >();
    ProgramPtr program( new Program( "" ) );
    try
    {
        if( pegtl::parse< Grammar::grammar, CompilerActions::actions, Errors::control >( content, "", program ) )
        {
            if( program )
            {
                std::string f = program->getLastFunction();
                if( !program->functions.count( f ) )
                    throw std::runtime_error( "Cannot make function! There is no unction built under name: " + f );
                return program->functions[f];
            }
            return nullptr;
        }
        return nullptr;
    }
    catch( const pegtl::parse_error& ex )
    {
        std::cout << ex.what() << std::endl;
        return nullptr;
    }
    catch( const std::exception& ex )
    {
        std::cout << ex.what() << std::endl;
        return nullptr;
    }
    return nullptr;
}

I4::CompilationStatePtr Builder::buildCompilationState( const std::string& inputPath )
{
    if( m_profile )
    {
        auto start = std::chrono::high_resolution_clock::now();
        ProgramPtr program = buildProgram( inputPath );
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast< std::chrono::milliseconds >( end - start ).count();
        std::cout << "Jaeger program built in " << ms << "ms!" << std::endl;
        if( !program )
        {
            std::cout << "Cannot build program from: " << inputPath << std::endl;
            return nullptr;
        }
        start = std::chrono::high_resolution_clock::now();
        auto result = program->assemble( this, m_stackSize );
        end = std::chrono::high_resolution_clock::now();
        ms = std::chrono::duration_cast< std::chrono::milliseconds >( end - start ).count();
        std::cout << "Jaeger program assembled in " << ms << "ms!" << std::endl;
        return result;
    }
    else
    {
        ProgramPtr program = buildProgram( inputPath );
        if( !program )
        {
            std::cout << "Cannot build program from: " << inputPath << std::endl;
            return nullptr;
        }
        return program->assemble( this, m_stackSize );
    }
}

void Builder::writeAsmToFile( const std::string& content )
{
    if( m_asmOutput.empty() )
        return;
    std::ofstream f( m_asmOutput, std::ofstream::out | std::ofstream::binary );
    if( f.good() )
        f << content;
    f.close();
}
