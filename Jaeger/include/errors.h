#ifndef __ERRORS_H__
#define __ERRORS_H__

#include "grammar.h"
#include <pegtl.hh>

namespace Errors
{
    template< typename Rule >
    struct control : pegtl::normal< Rule >
    {
        static const std::string errorMessage;

        template< typename Input, typename ... States >
        static void raise( const Input & in, States && ... )
        {
            //throw pegtl::parse_error( errorMessage, in );
            char* lb = (char*)in.begin() - in.column();
            char* le = (char*)in.begin();
            while( le && *le && le < in.end() - 1 && *(le + 1) != '\n' && *(le + 1) != '\r' )
                le += 1;
            std::string v = std::string_replace( std::string( lb, le ), "\t", "    " );
            std::size_t offset = v.find( std::string( (char*)in.begin(), le ) );
            std::stringstream ss;
            ss << errorMessage << std::endl << std::endl;
            ss << v << std::endl;
            ss << std::string( offset == std::string::npos ? 0 : offset, ' ' ) << '^' << std::endl;
            throw pegtl::parse_error( ss.str(), in );
        }
    };

    template<> const std::string control< pegtl::opt< pegtl::sor< Grammar::comment, pegtl::shebang >, Grammar::whitespaces_any > >::errorMessage = "expected optional comment or shebang expression";
    template<> const std::string control< Grammar::statements >::errorMessage = "expected statements";
    template<> const std::string control< Grammar::whitespaces_any >::errorMessage = "expected optional whitespaces";
    template<> const std::string control< Grammar::whitespaces >::errorMessage = "expected whitespaces";
    template<> const std::string control< pegtl::internal::until< pegtl::internal::eolf > >::errorMessage = "expected to consume characters up to end of line feed";
    template<> const std::string control< pegtl::sor< Grammar::directive_strict, Grammar::directive_import, Grammar::directive_start, Grammar::directive_jaegerify, Grammar::directive_marshal, Grammar::directive_asm_global > >::errorMessage = "expected one of directives: import, start, jaegerify, marshal, asm";
    template<> const std::string control< pegtl::one< '/' > >::errorMessage = "expected `/` character";
    template<> const std::string control< pegtl::sor< Grammar::directive_let, Grammar::directive_asm, Grammar::directive_return, Grammar::directive_set, Grammar::directive_if, Grammar::directive_while > >::errorMessage = "expected one of directives: let, asm, return, set, if, while";
    template<> const std::string control< Grammar::String::escaped >::errorMessage = "expected escaped string character";
    template<> const std::string control< Grammar::identifier_saved >::errorMessage = "expected identifier";
    template<> const std::string control< pegtl::xdigit >::errorMessage = "expected hexadecimal digit character";
    template<> const std::string control< pegtl::plus< Grammar::whitespaces_any, pegtl::if_then_else< Grammar::code_injection, Grammar::Flags::build_field_from_asm, Grammar::field > > >::errorMessage = "expected list of fields (jaeger or assembly)";
    template<> const std::string control< pegtl::plus< Grammar::whitespaces_any, Grammar::field > >::errorMessage = "expected non-empty list of fields";
    template<> const std::string control< pegtl::star< Grammar::whitespaces_any, Grammar::field > >::errorMessage = "expected list of fields";
    template<> const std::string control< pegtl::one< ')' > >::errorMessage = "expected `)` character";
    template<> const std::string control< pegtl::one< '}' > >::errorMessage = "expected `}` character";
    template<> const std::string control< pegtl::if_then_else< pegtl::seq< Grammar::whitespaces, Grammar::identifier_saved >, pegtl::success, Grammar::Flags::push_empty_value > >::errorMessage = "expected optional type identifier";
    template<> const std::string control< pegtl::opt< Grammar::whitespaces, Grammar::identifier_saved > >::errorMessage = "expected optional type identifier";
    template<> const std::string control< Grammar::function_body >::errorMessage = "expected function body";
    template<> const std::string control< pegtl::opt< Grammar::whitespaces, Grammar::function_call_sections > >::errorMessage = "expected function body sections";
    template<> const std::string control< pegtl::one< '[' > >::errorMessage = "expected `[` character";
    template<> const std::string control< pegtl::one< ']' > >::errorMessage = "expected `]` character";
    template<> const std::string control< Grammar::function_args >::errorMessage = "expected function arguments list";
    template<> const std::string control< Grammar::string_saved >::errorMessage = "expected string lateral";
    template<> const std::string control< Grammar::string >::errorMessage = "expected string lateral";
    template<> const std::string control< pegtl::string< 'f', 'r', 'o', 'm' > >::errorMessage = "expected `from` keyword";
    template<> const std::string control< pegtl::string< 'p', 'l', 'a', 'c', 'e', 'm', 'e', 'n', 't' > >::errorMessage = "expected `placement` keyword";
    template<> const std::string control< Grammar::function_call >::errorMessage = "expected function call";
    template<> const std::string control< pegtl::plus< Grammar::whitespaces_any, Grammar::function_call > >::errorMessage = "expected non-empty list of function calls";
    template<> const std::string control< Grammar::code_injection >::errorMessage = "expected code injection sequence";
    template<> const std::string control< pegtl::string< 'a', 's' > >::errorMessage = "expected `as` keyword";
    template<> const std::string control< pegtl::if_then_else< pegtl::seq< pegtl::string< 'a', 's' >, Grammar::whitespaces >, pegtl::seq< Grammar::function_definition, Grammar::whitespaces >, Grammar::Flags::clear_last_function_definition > >::errorMessage = "expected `as` keyword followed by function definition";
    template<> const std::string control< pegtl::string< 't', 'o' > >::errorMessage = "expected `to` keyword";
    template<> const std::string control< pegtl::string< 'w', 'i', 't', 'h' > >::errorMessage = "expected `with` keyword";
    template<> const std::string control< pegtl::sor< Grammar::directive_marshal_assembly_jaeger, Grammar::directive_marshal_jaeger_assembly > >::errorMessage = "expected either assembly-to-jaeger or jaeger-to-assembly marshalling";
    template<> const std::string control< Grammar::value >::errorMessage = "expected value sequence";
    template<> const std::string control< Grammar::data >::errorMessage = "expected value data expression";
    template<> const std::string control< pegtl::string< 't', 'h', 'e', 'n' > >::errorMessage = "expected `then` keyword";
    template<> const std::string control< pegtl::string< 'e', 'l', 'i', 'f' > >::errorMessage = "expected `elif` keyword";
    template<> const std::string control< pegtl::string< 'e', 'l', 's', 'e' > >::errorMessage = "expected `else` keyword";
    template<> const std::string control< Grammar::directive_if_cond >::errorMessage = "expected condition statement";
    template<> const std::string control< pegtl::star< Grammar::whitespaces, Grammar::directive_if_elif > >::errorMessage = "expected optional list of `elif` statements";
    template<> const std::string control< pegtl::opt< Grammar::whitespaces, Grammar::directive_if_else > >::errorMessage = "expected optional `else` statement";
    template<> const std::string control< pegtl::eof >::errorMessage = "expected end of file character";
    template<> const std::string control< pegtl::success >::errorMessage = "expected dummy success";
    template<> const std::string control< Grammar::Flags::store_expressions_stack >::errorMessage = "expected flag: store expressions stack";
    template<> const std::string control< Grammar::Flags::store_conditions_stack >::errorMessage = "expected flag: store conditions stack";
    template<> const std::string control< Grammar::Flags::push_sections_target >::errorMessage = "expected flag: push sections target";
}

#endif
