#ifndef __COMPILER_ACTIONS_H__
#define __COMPILER_ACTIONS_H__

#include "std_extension.h"
#include "grammar.h"
#include "program.h"
#include <pegtl.hh>
#include <iostream>

namespace CompilerActions
{
    template< typename Rule > struct actions : pegtl::nothing< Rule > {};

    template<> struct actions< Grammar::Flags::push_empty_value >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->save( "" );
        }
    };

    template<> struct actions< Grammar::Flags::push_sections_target >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->pushSectionsTarget();
        }
    };

    template<> struct actions< Grammar::Flags::clear_last_function_definition >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->clearLastFunctionDefinition();
        }
    };

    template<> struct actions< Grammar::Flags::store_values_stack >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->store();
        }
    };

    template<> struct actions< Grammar::Flags::store_expressions_stack >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->storeExpressions();
        }
    };

    template<> struct actions< Grammar::Flags::store_conditions_stack >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->storeConditions();
        }
    };

    template<> struct actions< Grammar::Flags::build_field_from_asm >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildFieldFromAsm();
        }
    };

    template<> struct actions< Grammar::identifier_saved >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->save( in.string() );
        }
    };

    template<> struct actions< Grammar::Number::integer_literal_saved >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            std::stringstream ss( in.string() );
            I4::i32 v = 0;
            ss >> v;
            program->buildConstantInteger( v );
        }
    };

    template<> struct actions< Grammar::Number::float_literal_saved >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            std::stringstream ss( in.string() );
            I4::f32 v = 0.0f;
            ss >> v;
            program->buildConstantFloat( v );
        }
    };

    template<> struct actions< Grammar::Number::hex_literal_saved >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            std::stringstream ss( in.string().substr( 2 ) );
            I4::i32 v = 0;
            ss >> std::hex >> v;
            program->buildConstantInteger( v );
        }
    };

    template<> struct actions< Grammar::string >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            std::string v = in.string();
            v = std::string_unescape( v.substr( 1, v.size() - 2 ) );
            program->save( v );
        }
    };

    template<> struct actions< Grammar::string_saved >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            std::string v = in.string();
            v = v.substr( 1, v.size() - 2 );
            program->buildConstantString( v );
        }
    };

    template<> struct actions< Grammar::code_injection >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            std::string v = in.string();
            v = std::string_trim( v.substr( 2, v.size() - 4 ) );
            program->save( v );
        }
    };

    template<> struct actions< Grammar::null_keyword >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildConstantNull();
        }
    };

    template<> struct actions< Grammar::false_keyword >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildConstantBool( false );
        }
    };

    template<> struct actions< Grammar::true_keyword >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildConstantBool( true );
        }
    };

    template<> struct actions< Grammar::value >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            std::vector< std::string > ids;
            std::size_t p = program->restore();
            while( program->getStackSize() > p )
                ids.push_back( program->load() );
            program->buildValue( ids );
        }
    };

    template<> struct actions< Grammar::function_call_section >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildFunctionCallSection();
        }
    };

    template<> struct actions< Grammar::function_call >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildFunctionCall();
        }
    };

    template<> struct actions< Grammar::structure >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildStructure();
        }
    };

    template<> struct actions< Grammar::field >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildField();
        }
    };

    template<> struct actions< Grammar::function_args >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildFunctionArgs();
        }
    };

    template<> struct actions< Grammar::function_definition >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildFunction();
        }
    };

    template<> struct actions< Grammar::directive_strict >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->markStrict();
        }
    };

    template<> struct actions< Grammar::directive_import >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildImport();
        }
    };

    template<> struct actions< Grammar::directive_start >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->startFunction = program->load();
        }
    };

    template<> struct actions< Grammar::directive_jaegerify >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildJaegerify();
        }
    };

    template<> struct actions< Grammar::directive_marshal_assembly_jaeger >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildMarshalFromAssembly();
        }
    };

    template<> struct actions< Grammar::directive_marshal_jaeger_assembly >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildMarshalToAssembly();
        }
    };

    template<> struct actions< Grammar::directive_let >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildFunctionVars();
        }
    };

    template<> struct actions< Grammar::directive_asm >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildAsm( false );
        }
    };

    template<> struct actions< Grammar::directive_asm_global >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildAsm( true );
        }
    };

    template<> struct actions< Grammar::directive_return >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->markReturnPlacement();
        }
    };

    template<> struct actions< Grammar::directive_set >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildVariableSet();
        }
    };

    template<> struct actions< Grammar::directive_if >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildIf();
        }
    };

    template<> struct actions< Grammar::directive_if_cond >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildCond();
        }
    };

    template<> struct actions< Grammar::directive_if_elif >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildElif();
        }
    };

    template<> struct actions< Grammar::directive_if_else >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildElse();
        }
    };

    /*template<> struct actions< Grammar::directive_while >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildWhile();
        }
    };*/

    template<> struct actions< Grammar::grammar_function_definition >
    {
        static void apply( const pegtl::input& in, ProgramPtr& program )
        {
            program->buildFunction();
        }
    };

}

#endif
