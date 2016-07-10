#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include "std_extension.h"
#include <pegtl.hh>

namespace Grammar
{
    struct function_call;
    struct function_body;
    struct data;
    namespace Flags
    {
        struct push_empty_value : pegtl::success {};
        struct push_sections_target : pegtl::success {};
        struct clear_last_function_definition : pegtl::success {};
        struct store_values_stack : pegtl::success {};
        struct store_expressions_stack : pegtl::success {};
        struct store_conditions_stack : pegtl::success {};
        struct build_field_from_asm : pegtl::success {};
    }
    namespace Comment
    {
        struct inlined : pegtl::seq< pegtl::one< '#' >, pegtl::until< pegtl::eolf > > {};
        struct block : pegtl::seq< pegtl::string< '#', '#', '#' >, pegtl::until< pegtl::string< '#', '#', '#' > > > {};
    }
    struct comment : pegtl::sor< Comment::block, Comment::inlined > {};
    struct whitespaces : pegtl::plus< pegtl::sor< pegtl::space, comment > > {};
    struct whitespaces_any : pegtl::star< pegtl::sor< pegtl::space, comment > > {};
    struct identifier : pegtl::identifier {};
    struct identifier_saved : identifier {};
    namespace Number
    {
        struct integer_literal : pegtl::seq< pegtl::opt< pegtl::one< '+', '-' > >, pegtl::plus< pegtl::digit >, pegtl::opt< pegtl::sor< pegtl::one< 'k' >, pegtl::one< 'm' >, pegtl::one< 'g' > > > > {};
        struct integer_literal_saved : integer_literal {};
        struct float_literal : pegtl::seq< pegtl::opt< pegtl::one< '+', '-' > >, pegtl::plus< pegtl::digit >, pegtl::one< '.' >, pegtl::plus< pegtl::digit > > {};
        struct float_literal_saved : float_literal {};
        struct hex_literal : pegtl::seq< pegtl::string< '0', 'x' >, pegtl::plus< pegtl::xdigit > > {};
        struct hex_literal_saved : hex_literal {};
    }
    struct number : pegtl::sor< Number::hex_literal, Number::float_literal, Number::integer_literal > {};
    namespace String
    {
        struct unicode : pegtl::seq< pegtl::one< 'u' >, pegtl::rep< 4, pegtl::must< pegtl::xdigit > > > {};
        struct escaped_character : pegtl::one< '"', '\\', '/', 'b', 'f', 'n', 'r', 't', '0' > {};
        struct escaped : pegtl::sor< escaped_character, unicode > {};
        struct unescaped : pegtl::utf8::range< 0x20, 0x10FFFF > {};
        struct character : pegtl::if_then_else< pegtl::one< '\\' >, pegtl::must< escaped >, unescaped > {};
    }
    struct string : pegtl::seq< pegtl::one< '"' >, pegtl::until< pegtl::one< '"' >, String::character > > {};
    struct string_saved : string {};
    struct code_injection : pegtl::seq< pegtl::two< '~' >, pegtl::until< pegtl::two< '~' >, pegtl::any > > {};
    struct null_keyword : pegtl::string< 'n', 'u', 'l', 'l' > {};
    struct false_keyword : pegtl::string< 'f', 'a', 'l', 's', 'e' > {};
    struct true_keyword : pegtl::string< 't', 'r', 'u', 'e' > {};
    struct constant_value : pegtl::sor< Number::float_literal_saved, Number::hex_literal_saved, Number::integer_literal_saved, string_saved, null_keyword, false_keyword, true_keyword > {};
    struct value : pegtl::seq< Flags::store_values_stack, identifier_saved, pegtl::star< whitespaces_any, pegtl::one< '.' >, whitespaces_any, identifier_saved > > {};
    struct field : pegtl::seq< pegtl::one< '<' >, whitespaces_any, identifier_saved, whitespaces, identifier_saved, whitespaces_any, pegtl::one< '>' > > {};
    struct structure : pegtl::if_must< pegtl::one< '{' >, whitespaces_any, identifier_saved, pegtl::plus< whitespaces_any, pegtl::if_then_else< code_injection, Flags::build_field_from_asm, field > >, whitespaces_any, pegtl::one< '}' > > {};
    struct directive_let : pegtl::if_must< pegtl::seq< pegtl::string< 'l', 'e', 't' >, whitespaces >, pegtl::plus< whitespaces_any, pegtl::if_then_else< code_injection, Flags::build_field_from_asm, field > > > {};
    struct directive_asm : pegtl::if_must< pegtl::seq< pegtl::string< 'a', 's', 'm' >, whitespaces >, code_injection > {};
    struct directive_return : pegtl::if_must< pegtl::seq< pegtl::string< 'r', 'e', 't', 'u', 'r', 'n' >, whitespaces >, pegtl::string< 'p', 'l', 'a', 'c', 'e', 'm', 'e', 'n', 't' > > {};
    struct directive_set : pegtl::if_must< pegtl::seq< pegtl::string< 's', 'e', 't' >, whitespaces >, value, whitespaces, data > {};
    struct directive_if_cond : pegtl::seq< data, whitespaces, pegtl::string< 't', 'h', 'e', 'n' >, whitespaces, Flags::store_expressions_stack, function_body > {};
    struct directive_if_elif : pegtl::if_must< pegtl::seq< pegtl::string< 'e', 'l', 'i', 'f' >, whitespaces >, data, whitespaces, pegtl::string< 't', 'h', 'e', 'n' >, whitespaces, Flags::store_expressions_stack, function_body > {};
    struct directive_if_else : pegtl::if_must< pegtl::seq< pegtl::string< 'e', 'l', 's', 'e' >, whitespaces >, data, whitespaces, pegtl::string< 't', 'h', 'e', 'n' >, whitespaces, Flags::store_expressions_stack, function_body > {};
    struct directive_if : pegtl::if_must< pegtl::seq< pegtl::string< 'i', 'f' >, whitespaces >, Flags::store_conditions_stack, directive_if_cond, pegtl::star< whitespaces, directive_if_elif >, pegtl::opt< whitespaces, directive_if_else > > {};
    struct directive_while : pegtl::if_must< pegtl::seq< pegtl::string< 'w', 'h', 'i', 'l', 'e' >, whitespaces >, data , whitespaces, pegtl::string< 't', 'h', 'e', 'n' >, whitespaces, Flags::store_expressions_stack, function_body > {};
    struct directives_local : pegtl::if_must< pegtl::one< '/' >, whitespaces_any, pegtl::sor< directive_let, directive_asm, directive_return, directive_set, directive_if, directive_while >, whitespaces_any, pegtl::one< '/' > > {};
    struct data : pegtl::sor< function_call, constant_value, value > {};
    struct function_call_section : pegtl::seq< Flags::store_expressions_stack, pegtl::star< whitespaces_any, data > > {};
    struct function_call_sections : pegtl::list< function_call_section, pegtl::seq< whitespaces_any, pegtl::one< '&' >, whitespaces_any > > {};
    struct function_call : pegtl::if_must< pegtl::one< '(' >, whitespaces_any, identifier_saved, Flags::push_sections_target, pegtl::opt< whitespaces, function_call_sections >, whitespaces_any, pegtl::one< ')' > > {};
    struct function_statement : pegtl::sor< directives_local, function_call, constant_value, value > {};
    struct function_body : pegtl::star< whitespaces_any, function_statement > {};
    struct function_args : pegtl::star< whitespaces_any, field > {};
    struct function_definition : pegtl::if_must< pegtl::one< '[' >, whitespaces_any, identifier_saved, pegtl::if_then_else< pegtl::seq< whitespaces, identifier_saved >, pegtl::success, Flags::push_empty_value >, function_args, function_body, whitespaces_any, pegtl::one< ']' > > {};
    struct directive_strict : pegtl::if_must< pegtl::string< 's', 't', 'r', 'i', 'c', 't' >, pegtl::success > {};
    struct directive_import : pegtl::if_must< pegtl::seq< pegtl::string< 'i', 'm', 'p', 'o', 'r', 't' >, whitespaces >, string > {};
    struct directive_start : pegtl::if_must< pegtl::seq< pegtl::string< 's', 't', 'a', 'r', 't' >, whitespaces >, identifier_saved > {};
    struct directive_jaegerify : pegtl::if_must< pegtl::seq< pegtl::string< 'j', 'a', 'e', 'g', 'e', 'r', 'i', 'f', 'y' >, whitespaces >, code_injection, whitespaces, pegtl::if_then_else< pegtl::seq< pegtl::string< 'a', 's' >, whitespaces >, pegtl::seq< function_definition, whitespaces >, Flags::clear_last_function_definition >, pegtl::string< 'f', 'r', 'o', 'm' >, whitespaces, identifier_saved > {};
    struct directive_marshal_assembly_jaeger : pegtl::if_must< string, whitespaces, pegtl::string< 't', 'o' >, whitespaces, identifier_saved, whitespaces, pegtl::string< 'w', 'i', 't', 'h' >, whitespaces, code_injection > {};
    struct directive_marshal_jaeger_assembly : pegtl::if_must< identifier_saved, whitespaces, pegtl::string< 't', 'o' >, whitespaces, string, whitespaces, pegtl::string< 'w', 'i', 't', 'h' >, whitespaces, code_injection > {};
    struct directive_marshal : pegtl::if_must< pegtl::seq< pegtl::string< 'm', 'a', 'r', 's', 'h', 'a', 'l' >, whitespaces >, pegtl::string< 'f', 'r', 'o', 'm' >, whitespaces, pegtl::sor< directive_marshal_assembly_jaeger, directive_marshal_jaeger_assembly > > {};
    struct directive_asm_global : directive_asm {};
    struct directives_global : pegtl::if_must< pegtl::one< '/' >, whitespaces_any, pegtl::sor< directive_strict, directive_import, directive_start, directive_jaegerify, directive_marshal, directive_asm_global >, whitespaces_any, pegtl::one< '/' > > {};
    struct statement : pegtl::seq< pegtl::sor< directives_global, structure, function_definition >, whitespaces_any > {};
    struct statements : pegtl::star< statement, whitespaces_any > {};
    struct grammar : pegtl::must< pegtl::opt< pegtl::sor< comment, pegtl::shebang >, whitespaces_any >, statements, whitespaces_any, pegtl::eof > {};
    struct grammar_function_definition : function_definition {};
}

#endif
