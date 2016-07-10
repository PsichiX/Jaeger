#include "../include/program.h"
#include "../include/builder.h"
#include "../include/std_extension.h"
#include <iostream>
#include <algorithm>

Program::Field::Field( const std::string& i, const std::string& t )
: id( i )
, type( t )
{
}

Program::Field::Field( const std::string& a )
: assembly( a )
{
}

Program::Field::~Field()
{
}

void Program::Field::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "Field" << std::endl;
    output << lvl << ":id " << id << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":assembly \"" << assembly << "\"" << std::endl;
}

void Program::Field::validate( Builder* builder, Program* program )
{
    if( isValidated )
        return;
    isValidated = true;
    // TODO: compile assembly?
    if( !type.empty() && !program->structures.count( type ) )
        throw std::runtime_error( "Cannot find type: " + type + " of field: " + id );
}

void Program::Field::assemble( std::ostream& output )
{
    if( assembly.empty() )
        output << id << ":*" << type;
    else
        output << assembly;
}

Program::Structure::Structure( const std::string& i )
: id( i )
{
}

Program::Structure::~Structure()
{
}

void Program::Structure::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "Structure" << std::endl;
    output << lvl << ":id " << id << std::endl;
    for( auto& f : fields )
        f->write( output, level + 1 );
}

void Program::Structure::validate( Builder* builder, Program* program )
{
    if( isValidated )
        return;
    isValidated = true;
    try
    {
        for( auto& f : fields )
            f->validate( builder, program );
    }
    catch( std::exception& ex )
    {
        std::stringstream ss;
        ss << ex.what() << std::endl << "Error in structure: " << id;
        throw std::runtime_error( ss.str() );
    }
}

void Program::Structure::assemble( std::ostream& output )
{
    output << "struct " << id << " { ";
    for( auto& f : fields )
    {
        f->assemble( output );
        output << "; ";
    }
    output << "___refcount___:i32; };" << std::endl;
    output << std::endl;
    output << "routine " << id << "_new():*" << id << std::endl;
    output << "<this:*" << id << ">" << std::endl;
    output << "{" << std::endl;
    output << "new " << id << " 1:i32 => $this;" << std::endl;
    for( auto& f : fields )
        if( f->assembly.empty() )
            output << "mov void null => $this->$" << f->id << ";" << std::endl;
    output << "mov void 0:i32 => $this->$___refcount___;" << std::endl;
    output << "ret $this;" << std::endl;
    output << "};" << std::endl;
    output << std::endl;
    output << "routine " << id << "_Ref(this:*" << id << "):" << std::endl;
    output << "<isnil:i8>" << std::endl;
    output << "{" << std::endl;
    output << "nil $this => $isnil;" << std::endl;
    output << "jif $isnil %failure %test;" << std::endl;
    output << "test:" << std::endl;
    output << "jif $this->$___refcount___ %success %failure;" << std::endl;
    output << "success:" << std::endl;
    output << "add void $this->$___refcount___ 1:i32 => $this->$___refcount___;" << std::endl;
    output << "failure:" << std::endl;
    output << "};" << std::endl;
    output << std::endl;
    output << "routine " << id << "_Unref(this:*" << id << ", stack:i32):*" << id << std::endl;
    output << "<isnil:i8>" << std::endl;
    output << "{" << std::endl;
    output << "nil $this => $isnil;" << std::endl;
    output << "jif $isnil %failure %test;" << std::endl;
    output << "test:" << std::endl;
    output << "jif $this->$___refcount___ %success %failure;" << std::endl;
    output << "success:" << std::endl;
    output << "sub void $this->$___refcount___ 1:i32 => $this->$___refcount___;" << std::endl;
    output << "jif $this->$___refcount___ %failure %delete;" << std::endl;
    output << "delete:" << std::endl;
    output << "call @" << id << "_Ref($this);" << std::endl;
    output << "cpush $stack void $this;" << std::endl;
    output << "call @" << id << "_Dispose__" << id << "($stack);" << std::endl;
    for( auto& f : fields )
        if( f->assembly.empty() )
            output << "call @" << f->type << "_Unref($this->$" << f->id << ", $stack) => $this->$" << f->id << ";" << std::endl;
    output << "del $this;" << std::endl;
    output << "ret null;" << std::endl;
    output << "failure:" << std::endl;
    output << "ret $this;" << std::endl;
    output << "};" << std::endl;
    output << std::endl;
    output << "routine " << id << "_drop(stack:i32):" << std::endl;
    output << "<this:*" << id << ">" << std::endl;
    output << "{" << std::endl;
    output << "cpop $stack void => $this;" << std::endl;
    output << "call @" << id << "_Unref($this, $stack) => $this;" << std::endl;
    output << "};" << std::endl;
    output << std::endl;
    output << std::endl;
}

Program::Expression::Expression( Type t )
: type( t )
{
}

Program::Expression::Expression( Type t, const std::string& rt )
: type( t )
, m_resultType( rt )
{
}

Program::Expression::~Expression()
{
}

Program::ConstantInteger::ConstantInteger( I4::i32 v )
: Expression( T_CONSTANT_INTEGER, "Int" )
, value( v )
{
}

Program::ConstantInteger::~ConstantInteger()
{
}

void Program::ConstantInteger::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "ConstantInteger" << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":value " << value << std::endl;
}

void Program::ConstantInteger::assemble( std::ostream& output, bool dropValue )
{
    if( dropValue )
        return;
    output << "call @___MakeIntConstant___($stack, " << value << ":i32);" << std::endl;
}

Program::ConstantFloat::ConstantFloat( I4::f32 v )
: Expression( T_CONSTANT_FLOAT, "Float" )
, value( v )
{
}

Program::ConstantFloat::~ConstantFloat()
{
}

void Program::ConstantFloat::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "ConstantFloat" << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":value " << value << std::endl;
}

void Program::ConstantFloat::assemble( std::ostream& output, bool dropValue )
{
    if( dropValue )
        return;
    output << "call @___MakeFloatConstant___($stack, " << value << ":f32);" << std::endl;
}

Program::ConstantString::ConstantString( const std::string& v )
: Expression( T_CONSTANT_STRING, "String" )
, value( v )
{
}

Program::ConstantString::~ConstantString()
{
}

void Program::ConstantString::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "ConstantString" << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":value \"" << value << "\"" << std::endl;
}

void Program::ConstantString::assemble( std::ostream& output, bool dropValue )
{
    if( dropValue )
        return;
    output << "call @___MakeStringConstant___($stack, \"" << value << "\\0\":*i8, " << value.length() << ":i32);" << std::endl;
}

Program::ConstantNull::ConstantNull()
: Expression( T_CONSTANT_NULL, "null" )
{
}

Program::ConstantNull::~ConstantNull()
{
}

void Program::ConstantNull::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "ConstantNull" << std::endl;
    output << lvl << ":type " << type << std::endl;
}

void Program::ConstantNull::assemble( std::ostream& output, bool dropValue )
{
    if( dropValue )
        return;
    output << "cpush $stack void null;" << std::endl;
}

Program::ConstantBool::ConstantBool( bool v )
: Expression( T_CONSTANT_BOOL, "Bool" )
, value( v )
{
}

Program::ConstantBool::~ConstantBool()
{
}

void Program::ConstantBool::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "ConstantBool" << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":value " << value << std::endl;
}

void Program::ConstantBool::assemble( std::ostream& output, bool dropValue )
{
    if( dropValue )
        return;
    output << "call @___MakeBoolConstant___($stack, " << (std::size_t)value << ":i8);" << std::endl;
}

Program::Value::Value( const std::vector<std::string>& i )
: Expression( T_VALUE )
, ids( i )
{
}

Program::Value::~Value()
{
}

void Program::Value::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "Value" << std::endl;
    output << lvl << ":type " << type << std::endl;
    std::size_t i = 1;
    for( auto& id : ids )
    {
        output << lvl << "-Id" << std::endl;
        output << lvl << "-:id " << id << std::endl;
        ++i;
    }
}

void Program::Value::validate( Builder* builder, Program* program, Function* func )
{
    if( isValidated )
        return;
    isValidated = true;
    Structure* s = program->findValueStructure( ids, func );
    if( !s )
    {
        std::stringstream ss( ids[0] );
        for( std::size_t i = 1, c = ids.size(); i < c; ++i )
            ss << "." << ids[i];
        throw std::runtime_error( "Cannot find structure type of: " + ss.str() );
    }
    setResultType( s->id );
}

void Program::Value::assemble( std::ostream& output, bool dropValue )
{
    if( dropValue )
        return;
    std::stringstream ss;
    ss << "$" << ids[0];
    for( std::size_t i = 1, c = ids.size(); i < c; ++i )
        ss << "->$" << ids[i];
    output << "call @" << getResultType() << "_Ref(" << ss.str() << ");" << std::endl;
    output << "cpush $stack void " << ss.str() << ";" << std::endl;
}

Program::FunctionCall::FunctionCall( const std::string& i, const std::vector< ExpressionPtr >& a )
: Expression( T_FUNCTION_CALL )
, id( i )
, args( a )
{
}

Program::FunctionCall::~FunctionCall()
{
}

void Program::FunctionCall::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "FunctionCall" << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":id " << id << std::endl;
    for( auto& a : args )
        a->write( output, level + 1 );
}

void Program::FunctionCall::validate( Builder* builder, Program* program, Function* func )
{
    if( isValidated )
        return;
    isValidated = true;
    std::stringstream ss;
    ss << id << "_";
    for( auto& a : args )
    {
        a->validate( builder, program, func );
        ss << "_" << a->getResultType();
    }
    if( !program->functions.count( ss.str() ) )
        throw std::runtime_error( "There is no function with signature: " + ss.str() );
    m_uid = ss.str();
    Function* f = program->functions[ss.str()].get();
    setResultType( f->type );
    if( program->isStrict() && getResultType().empty() )
        throw std::runtime_error( "In strict mode cannot call to function without return type!" );
}

void Program::FunctionCall::assemble( std::ostream& output, bool dropValue )
{
    for( auto& a : args )
        a->assemble( output, false );
    output << "call @" << m_uid << "($stack);" << std::endl;
    if( dropValue && !getResultType().empty() )
        output << "call @" << getResultType() << "_drop($stack);" << std::endl;
}

Program::Asm::Asm( const std::string& c )
: Expression( T_ASM )
, content( c )
{
}

Program::Asm::~Asm()
{
}

void Program::Asm::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "Asm" << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":content \"" << content << "\"" << std::endl;
}

void Program::Asm::validate( Builder* builder, Program* program, Function* func )
{
    if( isValidated )
        return;
    isValidated = true;
    if( program->isStrict() )
        throw std::runtime_error( "In strict mode asm expressions are not allowed!" );
}

void Program::Asm::assemble( std::ostream& output, bool dropValue )
{
    output << content << std::endl;
}

Program::Set::Set( const ExpressionPtr& v, const ExpressionPtr& d )
: Expression( T_SET )
, value( v )
, data( d )
{
}

Program::Set::~Set()
{
}

void Program::Set::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "Set" << std::endl;
    output << lvl << ":type " << type << std::endl;
    value->write( output, level + 1 );
    data->write( output, level + 1 );
}

void Program::Set::validate( Builder* builder, Program* program, Function* func )
{
    if( isValidated )
        return;
    isValidated = true;
    if( program->isStrict() )
        throw std::runtime_error( "In strict mode set expressions are not allowed!" );
    if( value->type != T_VALUE )
        throw std::runtime_error( "Trying to perform set operation on non-variable!" );
    value->validate( builder, program, func );
    data->validate( builder, program, func );
    if( data->getResultType().empty() )
        throw std::runtime_error( "Trying to perform set operation with void-expression!" );
}

void Program::Set::assemble( std::ostream& output, bool dropValue )
{
    data->assemble( output, false );
    Value* v = (Value*)value.get();
    std::stringstream ss;
    ss << "$" << v->ids[0];
    for( std::size_t i = 1, c = v->ids.size(); i < c; ++i )
        ss << "->$" << v->ids[i];
    output << "cpop $stack void => " << ss.str() << ";" << std::endl;
}

std::size_t Program::Condition::s_testUID = 0;

Program::Condition::Condition( const std::vector< ConditionData >& c )
: Expression( T_CONDITION )
, conditions( c )
{
    m_uid = ++s_testUID;
}

Program::Condition::~Condition()
{
}

void Program::Condition::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "Condition" << std::endl;
    output << lvl << ":type " << type << std::endl;
    for( auto& c : conditions )
    {
        output << lvl << "-Stage" << std::endl;
        if( c.condition )
        {
            output << lvl << "--Test" << std::endl;
            c.condition->write( output, level + 3 );
        }
        output << lvl << "--Expressions" << std::endl;
        for( auto& e : c.expressions )
            e->write( output, level + 4 );
    }
}

void Program::Condition::validate( Builder* builder, Program* program, Function* func )
{
    if( isValidated )
        return;
    isValidated = true;
    if( program->isStrict() )
        throw std::runtime_error( "In strict mode conditions are not allowed!" );
    if( conditions.empty() )
        throw std::runtime_error( "Condition without body detected!" );
    for( auto c = conditions.begin(); c != conditions.end(); ++c )
    {
        if( c->condition )
        {
            c->condition->validate( builder, program, func );
            if( c->condition->getResultType() != "Bool" )
                throw std::runtime_error( "Condition test accepts only boolean values!" );
            if( c->condition->type == T_CONSTANT_BOOL )
            {
                ConstantBool* b = (ConstantBool*)c->condition.get();
                if( b->value )
                {
                    c->condition.reset();
                    conditions.erase( c + 1, conditions.end() );
                    for( auto& e : c->expressions )
                        e->validate( builder, program, func );
                    break;
                }
                else
                {
                    c = conditions.erase( c );
                    for( auto& e : c->expressions )
                        e->validate( builder, program, func );
                    continue;
                }
            }
        }
        for( auto& e : c->expressions )
            e->validate( builder, program, func );
    }
}

void Program::Condition::assemble( std::ostream& output, bool dropValue )
{
    for( auto& c : conditions )
    {
        if( c.condition )
        {
            c.condition->assemble( output, false );
            output << "call @___TestBool___($stack) => $test;" << std::endl;
            output << "jif $test %__conditionSuccess" << c.uid << " %__conditionFailure" << c.uid << ";" << std::endl;
            output << "__conditionSuccess" << c.uid << ":" << std::endl;
            for( auto& e : c.expressions )
                e->assemble( output, true );
            output << "goto %__conditionDone" << m_uid << ";" << std::endl;
            output << "__conditionFailure" << c.uid << ":" << std::endl;
        }
        else
        {
            for( auto& e : c.expressions )
                e->assemble( output, true );
        }
    }
    output << "__conditionDone" << m_uid << ":" << std::endl;
}

Program::Function::Function( const std::string& i, const std::string& t )
: id( i )
, type( t )
{
}

Program::Function::~Function()
{
}

void Program::Function::write( std::ostream& output, std::size_t level )
{
    std::string lvl( level, '-' );
    output << lvl << "Function" << std::endl;
    output << lvl << ":id " << id << std::endl;
    output << lvl << ":type " << type << std::endl;
    output << lvl << ":returnPlacement " << returnPlacement << std::endl;
    output << lvl << "-Arguments" << std::endl;
    for( auto& a : args )
        a->write( output, level + 2 );
    output << lvl << "-Variables" << std::endl;
    for( auto& v : vars )
        v->write( output, level + 2 );
    output << lvl << "-Body" << std::endl;
    for( auto& e : expressions )
        e->write( output, level + 2 );
}

void Program::Function::validate( Builder* builder, Program* program )
{
    if( isValidated )
        return;
    isValidated = true;
    if( program->isStrict() )
    {
        if( type.empty() )
            throw std::runtime_error( "In strict mode function: " + id + " must have a return type!" );
        if( expressions.size() != 1 )
            throw std::runtime_error( "In strict mode function: " + id + " must have only one body expression!" );
        if( !vars.empty() )
            throw std::runtime_error( "In strict mode function: " + id + " cannot have any local variable!" );
    }
    if( !type.empty() && !program->structures.count( type ) )
        throw std::runtime_error( "Cannot find function: " + id + " return type: " + type );
    for( auto& a : args )
        a->validate( builder, program );
    for( auto& v : vars )
        v->validate( builder, program );
    for( auto& e : expressions)
        e->validate( builder, program, this );
    if( !returnPlacement && !constructor && !disposal && jaegerifiedId.empty() && !type.empty() && expressions.empty() )
        throw std::runtime_error( "Function: " + id + " must return value!" );
    if( !type.empty() && !expressions.empty() && expressions.back()->type != Expression::T_CONSTANT_NULL && expressions.back()->getResultType() != type )
    {
        std::string t = expressions.back()->getResultType();
        throw std::runtime_error( "Function: " + id + " last expression type: " + (t.empty() ? "void" : t) + " does not match function type: " + type );
    }
}

void Program::Function::assemble( std::ostream& output, Program* program )
{
    if( constructor )
    {
        output << "routine " << makeUID() << "(stack:i32):" << std::endl;
        output << "<this:*" << constructor->id;
        for( auto& a : args )
        {
            output << ", ";
            a->assemble( output );
        }
        output << ">" << std::endl;
        output << "{" << std::endl;
        for( std::vector< FieldPtr >::reverse_iterator it = args.rbegin(); it != args.rend(); ++it )
            output << "cpop $stack void => $" << it->get()->id << ";" << std::endl;
        output << "call @" << constructor->id << "_new() => $this;" << std::endl;
        output << "call @" << constructor->id << "_Ref($this);" << std::endl;
        output << "cpush $stack void $this;" << std::endl;
        output << "};" << std::endl;
        output << std::endl;
    }
    else if( disposal )
    {
        output << "routine " << makeUID() << "(stack:i32):" << std::endl;
        output << "<this:*" << disposal->id << ">" << std::endl;
        output << "{" << std::endl;
        output << "cpop $stack void => $this;" << std::endl;
        output << "call @" << disposal->id << "_Unref($this, $stack) => $this;" << std::endl;
        output << "};" << std::endl;
        output << std::endl;
    }
    else if( !jaegerifiedId.empty() )
    {
        if( !program->jaegerified.count( jaegerifiedId ) )
            throw std::runtime_error( "Cannot assemble jaegerified function! There is no jaegerified meta information for: " + jaegerifiedId );
        JaegerifyData& data = program->jaegerified[jaegerifiedId];
        const auto& nm = program->getNativeModule( jaegerifiedId );
        if( !nm )
            throw std::runtime_error( "Cannot assemble jaegerified function! There is no jaegerified compiled routine for: " + jaegerifiedId );
        output << "routine " << makeUID() << "(stack:i32):" << std::endl;
        std::size_t i = 0;
        std::size_t j = 0;
        output << "<test:i8";
        if( !type.empty() )
            output << ", result:*" << type << ", " << program->marshallField( "result", type, nm->type->makeUID() );
        for( auto& a : args )
        {
            output << ", ";
            a->assemble( output );
            output << ", " << program->marshallField( a, nm->args[j]->type->makeUID() );
        }
        output << ">" << std::endl;
        output << "{" << std::endl;
        j = 0;
        for( std::vector< FieldPtr >::reverse_iterator it = args.rbegin(); it != args.rend(); ++it )
        {
            output << "cpop $stack void => $" << (*it)->id << ";" << std::endl;
            program->marshallValue( output, *it, nm->args[j]->type->makeUID() );
        }
        output << "call @" << data.method << "(";
        i = args.size();
        for( auto& a : args )
        {
            output << "$_" << a->id;
            if( --i )
                output << ", ";
        }
        if( type.empty() )
            output << ");" << std::endl;
        else
        {
            output << ") => $_result;" << std::endl;
            output << "call @" << type << "_($stack);" << std::endl;
            output << "cpop $stack void => $result;" << std::endl;
            program->marshallObject( output, "result", type, nm->type->makeUID() );
            output << "cpush $stack void $result;" << std::endl;
        }
        output << "}[{" << std::endl;
        for( auto& a : args )
            output << "call @" << a->type << "_Unref($" << a->id << ", $stack) => $" << a->id << ";" << std::endl;
        output << "}];" << std::endl;
        output << std::endl;
    }
    else
    {
        output << "routine " << makeUID() << "(stack:i32):" << std::endl;
        std::size_t i = 0;
        output << "<test:i8";
        for( auto& a : args )
        {
            output << ", ";
            a->assemble( output );
        }
        for( auto& v : vars )
        {
            output << ", ";
            v->assemble( output );
        }
        output << ">" << std::endl;
        output << "{" << std::endl;
        for( std::vector< FieldPtr >::reverse_iterator it = args.rbegin(); it != args.rend(); ++it )
            output << "cpop $stack void => $" << it->get()->id << ";" << std::endl;
        for( auto& v : vars )
            if( v->assembly.empty() )
                output << "mov void null => $" + v->id << ";" << std::endl;
        i = 0;
        if( type.empty() )
        {
            for( auto& e : expressions )
                e->assemble( output, true );
        }
        else
        {
            for( auto& e : expressions )
                e->assemble( output, ++i < expressions.size() && e->type != Expression::T_SET );
        }
        output << "}[{" << std::endl;
        for( auto& a : args )
            output << "call @" << a->type << "_Unref($" << a->id << ", $stack) => $" << a->id << ";" << std::endl;
        for( auto& v : vars )
            if( v->assembly.empty() )
                output << "call @" + v->type << "_Unref($" << v->id << ", $stack) => $" << v->id << ";" << std::endl;
        output << "}];" << std::endl;
        output << std::endl;
    }
}

std::string Program::Function::makeUID()
{
    if( !m_uid.empty() )
        return m_uid;
    std::stringstream ss;
    ss << id << "_";
    for( auto& a : args )
        ss << "_" << a->type;
    m_uid = ss.str();
    return m_uid;
}

Program::Program()
: m_strict( false )
, m_returnPlacement( false )
{
}

Program::~Program()
{
}

void Program::write( std::ostream& output )
{
    output << "Program" << std::endl;
    output << ":startFunction " << startFunction << std::endl;
    for( auto& i : imports )
    {
        output << "-Import" << std::endl;
        output << "-:path \"" << i << "\"" << std::endl;
    }
    for( auto& a : globalAsm )
    {
        output << "-GlobalAsm" << std::endl;
        output << "-:assembly \"" << a->content << "\"" << std::endl;
    }
    for( auto& m : marshallingFrom )
    {
        output << "-MarshalFrom" << std::endl;
        output << "-:asmType " << m.asmType << std::endl;
        output << "-:type " << m.type << std::endl;
        output << "-:assembly \"" << m.assembly << "\"" << std::endl;
    }
    for( auto& m : marshallingTo )
    {
        output << "-MarshalTo" << std::endl;
        output << "-:asmType " << m.asmType << std::endl;
        output << "-:type " << m.type << std::endl;
        output << "-:assembly \"" << m.assembly << "\"" << std::endl;
    }
    for( auto& j : jaegerified )
    {
        std::string f = j.first;
        output << "-Jaegerify" << std::endl;
        output << "-:module " << j.second.module << std::endl;
        output << "-:routine \"" << j.second.routine << "\"" << std::endl;
        if( j.second.func )
            j.second.func->write( output, 2 );
    }
    for( auto& s : structures )
        s.second->write( output, 1 );
    for( auto& f : functions )
        f.second->write( output, 1 );
}

void Program::validate( Builder* builder )
{
    std::string temp;
    FunctionPtr f;
    I4::CompilationState::RoutinePtr r;
    std::string tr;
    std::string tf;
    I4::CompilationState::Type* pt;
    for( auto& j : jaegerified )
    {
        JaegerifyData& d = j.second;
        I4::VM vm;
        r = vm.buildRoutineHeader( d.routine, std::cout );
        if( !r )
            throw std::runtime_error( "Cannot build routine: " + j.first + " from native module: " + d.module );
        r->nativeModulePackage = d.module;
        r->nativeModuleMethod = r->id;
        m_nativeModules[j.first] = r;
        f = d.func;
        if( f )
        {
            pt = r->type.get();
            tr = pt ? pt->makeUID() : "";
            tf = f->type;
            if( pt && !tf.empty() )
            {
                if( !verifyMarshalling( pt, tf ) )
                    throw std::runtime_error( "Cannot build routine: " + j.first + " from native module: " + d.module + "! Routine return type does not match function type marshalling! Trying to marshall: " + tf + " to: " + tr );
            }
            else if( !pt && !tf.empty() )
                throw std::runtime_error( "Cannot build routine: " + j.first + " from native module: " + d.module + "! Routine and function have different types! Trying to marshall: " + tf + " to: " + tr );
            if( f->args.size() != r->args.size() )
                throw std::runtime_error( "Cannot build routine: " + j.first + " from native module: " + d.module + "! Routine and function have different number of arguments!" );
            functions[f->makeUID()] = f;
            for( std::size_t i = 0, c = r->args.size(); i < c; ++i )
            {
                pt = r->args[i]->type.get();
                tr = pt->makeUID();
                tf = f->args[i]->type;
                if( !verifyMarshalling( pt, tf ) )
                    throw std::runtime_error( "Cannot build routine: " + j.first + " from native module: " + d.module + "! Routine argument #" + std::to_string( i + 1 ) + " does not match function type marshalling! Trying to marshall: " + tf + " to: " + tr );
            }
        }
    }
    Structure* ps = nullptr;
    for( auto& s : structures )
    {
        ps = s.second.get();
        ps->validate( builder, this );
        if( !functions.count( ps->id ) )
        {
            f = builder->makeFunction( "[" + ps->id + " " + ps->id + "]" );
            if( !f )
                throw std::runtime_error( "Cannot create constructor function of type: " + ps->id );
            f->constructor = s.second;
            functions[f->makeUID()] = f;
        }
        if( !functions.count( ps->id + "_Dispose__" + ps->id ) )
        {
            f = builder->makeFunction( "[" + ps->id + "_Dispose <this " + ps->id + ">]" );
            if( !f )
                throw std::runtime_error( "Cannot create disposal function of type: " + ps->id );
            f->disposal = s.second;
            functions[f->makeUID()] = f;
        }
    }
    for( auto& f : functions )
        f.second->validate( builder, this );
    if( !startFunction.empty() )
    {
        std::string sf = startFunction + "_";
        if( !functions.count( sf ) )
            throw std::runtime_error( "Cannot find startup function: " + startFunction );
        if( functions[sf]->type != "Int" )
            throw std::runtime_error( "Startup function does not return Int value: " + startFunction );
    }
}

I4::CompilationStatePtr Program::assemble( Builder* builder, std::size_t stackSize )
{
    std::stringstream ss;
    ss << "#!/usr/bin/env i4s" << std::endl;
    ss << std::endl;
    ss << "#intuicio 4.0;" << std::endl;
    ss << "#stack " << stackSize << ";" << std::endl;
    ss << "#pointersize 32;" << std::endl;
    ss << std::endl;
    ss << "routine ___MakeIntConstant___(stack:i32, value:i32):" << std::endl;
    ss << "<this:*Int>" << std::endl;
    ss << "{" << std::endl;
    ss << "call @Int_($stack);" << std::endl;
    ss << "cpop $stack void => $this;" << std::endl;
    ss << "mov void $value => $this->$value;" << std::endl;
    ss << "cpush $stack void $this;" << std::endl;
    ss << "};" << std::endl;
    ss << std::endl;
    ss << "routine ___MakeFloatConstant___(stack:i32, value:f32):" << std::endl;
    ss << "<this:*Float>" << std::endl;
    ss << "{" << std::endl;
    ss << "call @Float_($stack);" << std::endl;
    ss << "cpop $stack void => $this;" << std::endl;
    ss << "mov void $value => $this->$value;" << std::endl;
    ss << "cpush $stack void $this;" << std::endl;
    ss << "};" << std::endl;
    ss << std::endl;
    ss << "routine ___MakeStringConstant___(stack:i32, value:*i8, length:i32):" << std::endl;
    ss << "<this:*String>" << std::endl;
    ss << "{" << std::endl;
    ss << "call @String_($stack);" << std::endl;
    ss << "cpop $stack void => $this;" << std::endl;
    ss << "mov void $value => $this->$buffer;" << std::endl;
    ss << "mov void $length => $this->$length;" << std::endl;
    ss << "cpush $stack void $this;" << std::endl;
    ss << "};" << std::endl;
    ss << std::endl;
    ss << "routine ___MakeBoolConstant___(stack:i32, value:i8):" << std::endl;
    ss << "<this:*Bool>" << std::endl;
    ss << "{" << std::endl;
    ss << "call @Bool_($stack);" << std::endl;
    ss << "cpop $stack void => $this;" << std::endl;
    ss << "mov void $value => $this->$value;" << std::endl;
    ss << "cpush $stack void $this;" << std::endl;
    ss << "};" << std::endl;
    ss << std::endl;
    ss << "routine ___TestBool___(stack:i32):i8" << std::endl;
    ss << "<this:*Bool>" << std::endl;
    ss << "{" << std::endl;
    ss << "cpop $stack void => $this;" << std::endl;
    ss << "ret $this->$value;" << std::endl;
    ss << "}" << std::endl;
    ss << "[{" << std::endl;
    ss << "call @Bool_Unref($this, $stack) => $this;" << std::endl;
    ss << "}];" << std::endl;
    ss << std::endl;
    ss << "routine ___CalculateStringLength___(value:*i8):i32" << std::endl;
    ss << "<pos:i32, char:i8>" << std::endl;
    ss << "{" << std::endl;
    ss << "mov void -1:i32 => $pos;" << std::endl;
    ss << "iterate:" << std::endl;
    ss << "add void 1:i32 $pos => $pos;" << std::endl;
    ss << "val $value => $char;" << std::endl;
    ss << "add void $value 1:i32 => $value;" << std::endl;
    ss << "jif $char %iterate %done;" << std::endl;
    ss << "done:" << std::endl;
    ss << "ret $pos;" << std::endl;
    ss << "};" << std::endl;
    ss << std::endl;
    for( auto& a : globalAsm )
        ss << a->content << std::endl;
    ss << std::endl;
    for( auto& j : jaegerified )
        ss << "#import from [" << j.second.module << ":" << j.second.method << "] routine " << j.second.routine << ";" << std::endl;
    ss << std::endl;
    for( auto& s : structures )
        s.second->assemble( ss );
    for( auto& f : functions )
        f.second->assemble( ss, this );
    if( !startFunction.empty() )
    {
        ss << "#entry @___JAEGER_MAIN___;" << std::endl;
        ss << "routine ___JAEGER_MAIN___():i32" << std::endl;
        ss << "<stack:i32, result:*Int, isnil:i8>" << std::endl;
        ss << "{" << std::endl;
        ss << "ctxc => $stack;" << std::endl;
        ss << "call @" << startFunction << "_($stack);" << std::endl;
        ss << "cpop $stack void => $result;" << std::endl;
        ss << "nil $result => $isnil;" << std::endl;
        ss << "jif $isnil %failure %success;" << std::endl;
        ss << "success:" << std::endl;
        ss << "ret $result->$value;" << std::endl;
        ss << "failure:" << std::endl;
        ss << "ret -1:i32;" << std::endl;
        ss << "}[{" << std::endl;
        ss << "call @Int_Unref($result, $stack) => $result;" << std::endl;
        ss << "ctxd $stack;" << std::endl;
        ss << "}];" << std::endl;
    }
    #ifdef DEBUG_PRINT
    std::cout << ss.str() << std::endl;
    #endif
    if( builder->canWriteAsmToFile() )
        builder->writeAsmToFile( ss.str() );
    I4::VM vm;
    std::stringstream err;
    auto state = vm.buildCompilationStateContent( ss.str(), err );
    if( !state || !state->isValidated )
    {
        std::cout << "Intuicio 4 assembly compilation error:" << std::endl << err.str();
        return nullptr;
    }
    return state;
}

bool Program::verifyMarshalling( I4::CompilationState::Type* fromType, const std::string& toType )
{
    if( !fromType || toType.empty() )
        return false;
    std::string t = fromType->makeUID();
    for( auto& m : marshallingFrom )
    {
        if( m.asmType == t && m.type == toType )
        {
            for( auto& m : marshallingTo )
                if( m.asmType == t && m.type == toType )
                    return true;
            break;
        }
    }
    if( t == "*i8" || t == "*u8" )
        return toType == "String";
    else if( t == "i8" || t == "u8" )
        return toType == "Bool";
    else if( t == "i16" || t == "u16" || t == "i32" || t == "u32" || t == "i64" || t == "u64" )
        return toType == "Int";
    else if( t == "f32" || t == "f64" )
        return toType == "Float";
    else if( fromType->mode == I4::CompilationState::Type::M_POINTER && toType == "Handle" )
        return true;
    else
        return false;
}

std::string Program::marshallField( const std::string& id, const std::string& type, const std::string& asmType )
{
    std::stringstream ss;
    ss << "_" << id << ":" << asmType;
    return ss.str();
}

std::string Program::marshallField( const FieldPtr& field, const std::string& asmType )
{
    std::stringstream ss;
    ss << "_" << field->id << ":" << asmType;
    return ss.str();
}

void Program::marshallValue( std::ostream& output, const FieldPtr& field, const std::string& asmType )
{
    for( auto& m : marshallingTo )
    {
        if( m.asmType == asmType && m.type == field->type )
        {
            std::string a = m.assembly;
            a = std::string_replace( a, "$from", "$" + field->id );
            a = std::string_replace( a, "$to", "$_" + field->id );
            output << a << std::endl;
            return;
        }
    }
    if( field->type == "Int" && ( asmType == "i16" || asmType == "u16" || asmType == "i32" || asmType == "u32" || asmType == "i64" || asmType == "u64" ) )
    {
        output << "conv $" << field->id << "->$value => $_" << field->id << ";" << std::endl;
        return;
    }
    else if( field->type == "Float" && ( asmType == "f32" || asmType == "f64" ) )
    {
        output << "conv $" << field->id << "->$value => $_" << field->id << ";" << std::endl;
        return;
    }
    else if( field->type == "Bool" && ( asmType == "i8" || asmType == "u8" ) )
    {
        output << "conv $" << field->id << "->$value => $_" << field->id << ";" << std::endl;
        return;
    }
    else if( field->type == "String" && ( asmType == "*i8" || asmType == "*u8" ) )
    {
        output << "cast $" << field->id << "->$buffer => $_" << field->id << ";" << std::endl;
        return;
    }
    else if( field->type == "Handle" && ( !asmType.empty() && asmType[0] == '*' ) )
    {
        output << "conv $" << field->id << "->$value => $_" << field->id << ";" << std::endl;
        return;
    }
    throw std::runtime_error( "There is no marshalling definition from value type: " + field->type + " to assembly type: " + asmType );
}

void Program::marshallObject( std::ostream& output, const std::string& id, const std::string& type, const std::string& asmType )
{
    for( auto& m : marshallingFrom )
    {
        if( m.asmType == asmType && m.type == type )
        {
            std::string a = m.assembly;
            a = std::string_replace( a, "$from", "$_" + id );
            a = std::string_replace( a, "$to", "$" + id );
            output << a << std::endl;
            return;
        }
    }
    if( type == "Int" && ( asmType == "i16" || asmType == "u16" || asmType == "i32" || asmType == "u32" || asmType == "i64" || asmType == "u64" ) )
    {
        output << "conv $_" << id << " => $" << id << "->$value;" << std::endl;
        return;
    }
    else if( type == "Float" && ( asmType == "f32" || asmType == "f64" ) )
    {
        output << "conv $_" << id << " => $" << id << "->$value;" << std::endl;
        return;
    }
    else if( type == "Bool" && ( asmType == "i8" || asmType == "u8" ) )
    {
        output << "conv $_" << id << " => $" << id << "->$value;" << std::endl;
        return;
    }
    else if( type == "String" && ( asmType == "*i8" || asmType == "*u8" ) )
    {
        output << "conv $_" << id << " => $" << id << "->$buffer;" << std::endl;
        output << "call @___CalculateStringLength___($" << id << "->$buffer) => $" << id << "->$length;" << std::endl;
        return;
    }
    else if( type == "Handle" && ( !asmType.empty() && asmType[0] == '*' ) )
    {
        output << "conv $_" << id << " => $" << id << "->$value;" << std::endl;
        return;
    }
    throw std::runtime_error( "There is no marshalling definition from assembly type: " + asmType + " to value type: " + type );
}

Program::Structure* Program::findValueStructure( const std::vector< std::string >& ids, Function* func )
{
    const std::string& id = ids.front();
    for( auto& v : func->vars )
        if( id == v->id )
            return findValueStructure( ids, v->type, 1 );
    for( auto& a : func->args )
        if( id == a->id )
            return findValueStructure( ids, a->type, 1 );
    return nullptr;
}

Program::Structure* Program::findValueStructure( const std::vector< std::string >& ids, const std::string& type, std::size_t index )
{
    if( !structures.count( type ) )
        return nullptr;
    else if( index >= ids.size() )
        return structures[type].get();
    else
    {
        Structure* s = structures[type].get();
        const std::string& id = ids[index];
        for( auto& f : s->fields )
            if( id == f->id )
                return findValueStructure( ids, f->type, index + 1 );
        return nullptr;
    }
}

void Program::loadImports( Builder* builder )
{
    for( auto& path : imports )
    {
        ProgramPtr p = builder->buildProgram( path, true );
        if( !p )
            throw std::runtime_error( "Cannot build imported program: " + path );
        linkProgram( p.get() );
    }
}

void Program::linkProgram( Program* program )
{
    for( auto& j : program->jaegerified )
    {
        if( jaegerified.count( j.first ) )
        {
            auto& d = jaegerified[j.first];
            auto& _d = program->jaegerified[j.first];
            if( d.routine != _d.routine )
                throw std::runtime_error( "Cannot merge jaegerified: " + j.first + "! There is already existing jaegerify with different routine: " + d.routine );
            if( d.func != _d.func )
                throw std::runtime_error( "Cannot merge jaegerified: " + j.first + "! There is already existing jaegerify with different function!" );
            if( d.module != _d.module )
                throw std::runtime_error( "Cannot merge jaegerified: " + j.first + "! There is already existing jaegerify with different module: " + d.module);
        }
        jaegerified[j.first] = program->jaegerified[j.first];
    }
    for( auto& s : program->structures )
    {
        if( structures.count( s.first ) )
            throw std::runtime_error( "Cannot merge structure! There is already existing structure: " + s.first );
        structures[s.first] = s.second;
    }
    for( auto& f : program->functions )
    {
        if( functions.count( f.first ) )
            throw std::runtime_error( "Cannot merge function! There is already existing function: " + f.first );
        functions[f.first] = f.second;
    }
}

void Program::save( const std::string& v )
{
    m_stack.push( v );
}

std::string Program::load()
{
    if( m_stack.empty() )
        throw std::runtime_error( "Cannot load value from stack - stack is empty!" );
    std::string v = m_stack.top();
    m_stack.pop();
    return v;
}

void Program::discard()
{
    if( m_stack.empty() )
        throw std::runtime_error( "Cannot discard value from stack - stack is empty!" );
    m_stack.pop();
}

void Program::store()
{
    m_stackPositions.push( m_stack.size() );
}

std::size_t Program::restore()
{
    if( m_stackPositions.empty() )
        throw std::runtime_error( "Cannot restore stack position - positions stack is empty!" );
    std::size_t p = m_stackPositions.top();
    m_stackPositions.pop();
    return p;
}

void Program::storeExpressions()
{
    m_expressionsPositions.push( m_builtExpressions.size() );
}

std::size_t Program::restoreExpressions()
{
    if( m_expressionsPositions.empty() )
        throw std::runtime_error( "Cannot restore expressions position - expressions positions stack is empty!" );
    std::size_t p = m_expressionsPositions.top();
    m_expressionsPositions.pop();
    return p;
}

void Program::storeConditions()
{
    m_conditionsPositions.push( m_conditions.size() );
}

std::size_t Program::restoreConditions()
{
    if( m_conditionsPositions.empty() )
        throw std::runtime_error( "Cannot restore conditions position - conditions positions stack is empty!" );
    std::size_t p = m_conditionsPositions.top();
    m_conditionsPositions.pop();
    return p;
}

void Program::buildStructure()
{
    std::string id = load();
    if( structures.count( id ) )
        throw std::runtime_error( "There is already existing structure: " + id );
    Structure* s = new Structure( id );
    s->fields = m_builtFields;
    m_builtFields.clear();
    structures[ id ] = StructurePtr( s );
}

void Program::buildField()
{
    std::string type = load();
    std::string id = load();
    Field* f = new Field( id, type );
    m_builtFields.push_back( FieldPtr( f ) );
}

void Program::buildFieldFromAsm()
{
    std::string a = load();
    Field* f = new Field( a );
    m_builtFields.push_back( FieldPtr( f ) );
}

void Program::buildFunctionVars()
{
    for( auto& f : m_builtFields )
        m_builtVars.push_back( f );
    m_builtFields.clear();
}

void Program::buildFunctionArgs()
{
    m_builtArgs = m_builtFields;
    m_builtFields.clear();
}

void Program::buildFunction()
{
    std::string type = load();
    std::string id = load();
    Function* f = new Function( id, type );
    f->args = m_builtArgs;
    m_builtArgs.clear();
    if( functions.count( f->makeUID() ) )
        throw std::runtime_error( "There is already existing function with signature: " + f->makeUID() );
    f->vars = m_builtVars;
    m_builtVars.clear();
    f->expressions = m_builtExpressions;
    m_builtExpressions.clear();
    while( !m_expressionsPositions.empty() )
        m_expressionsPositions.pop();
    f->returnPlacement = m_returnPlacement;
    m_returnPlacement = false;
    std::string uid = f->makeUID();
    functions[uid] = FunctionPtr( f );
    m_lastFunctionDefinition = uid;
}

void Program::buildConstantInteger( I4::i32 v )
{
    m_builtExpressions.push_back( ExpressionPtr( new ConstantInteger( v ) ) );
}

void Program::buildConstantFloat( I4::f32 v )
{
    m_builtExpressions.push_back( ExpressionPtr( new ConstantFloat( v ) ) );
}

void Program::buildConstantString( const std::string& v )
{
    m_builtExpressions.push_back( ExpressionPtr( new ConstantString( v ) ) );
}

void Program::buildConstantNull()
{
    m_builtExpressions.push_back( ExpressionPtr( new ConstantNull() ) );
}

void Program::buildConstantBool( bool v )
{
    m_builtExpressions.push_back( ExpressionPtr( new ConstantBool( v ) ) );
}

void Program::buildValue( const std::vector< std::string >& ids )
{
    m_builtExpressions.push_back( ExpressionPtr( new Value( ids ) ) );
}

void Program::buildFunctionCallSection()
{
    std::vector< ExpressionPtr > exp;
    std::size_t p = restoreExpressions();
    for( std::size_t i = p; i < m_builtExpressions.size(); ++i )
        exp.push_back( m_builtExpressions[i] );
    while( m_builtExpressions.size() > p )
        m_builtExpressions.pop_back();
    if( m_sections.empty() )
        throw std::runtime_error( "Function call sections target is empty!" );
    m_sections.top().push( exp );
}

void Program::buildFunctionCall()
{
    if( m_sections.empty() )
        throw std::runtime_error( "Function call sections target is empty!" );
    auto& q = m_sections.top();
    std::string id = load();
    FunctionCall* fc;
    if( q.empty() )
    {
        fc = new FunctionCall( id, {} );
        m_builtExpressions.push_back( ExpressionPtr( fc ) );
    }
    else
    {
        while( !q.empty() )
        {
            fc = new FunctionCall( id, q.front() );
            q.pop();
            m_builtExpressions.push_back( ExpressionPtr( fc ) );
        }
    }
    m_sections.pop();
}

void Program::buildAsm( bool isGlobal )
{
    std::string c = load();
    if( isGlobal )
        globalAsm.push_back( AsmPtr( new Asm( c ) ) );
    else
        m_builtExpressions.push_back( ExpressionPtr( new Asm( c ) ) );
}

void Program::markReturnPlacement()
{
    m_returnPlacement = true;
}

void Program::buildImport()
{
    std::string path = load();
    if( std::find( imports.begin(), imports.end(), path ) == imports.end() )
        imports.push_back( path );
}

void Program::buildJaegerify()
{
    if( m_lastFunctionDefinition.empty() )
    {
        std::string m = load();
        std::string r = load();
        if( r.find( "routine" ) != 0 )
            throw std::runtime_error( "Assembly routine need to start with `routine` keyword: " + r );
        r = std::string_trim( r.substr( 7 ) );
        JaegerifyData& data = jaegerified[r];
        data.routine = r;
        data.func = nullptr;
        data.module = m;
        std::size_t found = r.find( "(" );
        if( found == std::string::npos )
            throw std::runtime_error( "Assembly routine does not have a name: " + r );
        data.method = std::string_trim( r.substr( 0, found ) );
    }
    else
    {
        if( jaegerified.count( m_lastFunctionDefinition ) )
            throw std::runtime_error( "Cannot jaegerify function! Function is already jaegerified: " + m_lastFunctionDefinition );
        std::string m = load();
        if( !functions.count( m_lastFunctionDefinition ) )
            throw std::runtime_error( "Cannot jaegerify function! Function not found: " + m_lastFunctionDefinition );
        FunctionPtr f = functions[m_lastFunctionDefinition];
        if( !f->expressions.empty() )
            throw std::runtime_error( "Cannot jaegerify function that have body expressions: " + m_lastFunctionDefinition );
        functions.erase( m_lastFunctionDefinition );
        std::string r = load();
        if( r.find( "routine" ) != 0 )
            throw std::runtime_error( "Assembly routine need to start with `routine` keyword: " + r );
        r = std::string_trim( r.substr( 7 ) );
        JaegerifyData& data = jaegerified[m_lastFunctionDefinition];
        data.routine = r;
        data.func = f;
        data.module = m;
        std::size_t found = r.find( "(" );
        if( found == std::string::npos )
            throw std::runtime_error( "Assembly routine does not have a name: " + r );
        data.method = std::string_trim( r.substr( 0, found ) );
        f->jaegerifiedId = m_lastFunctionDefinition;
    }
}

void Program::buildMarshalFromAssembly()
{
    std::string a = load();
    std::string t = load();
    std::string f = load();
    for( auto& m : marshallingFrom )
    {
        if( m.asmType == f && m.type == t )
        {
            if( m.assembly != a )
                throw std::runtime_error( "Cannot define marshalling! There is already existing marshalling but with different definition! From: " + f + " to: " + t );
            return;
        }
    }
    MarshalData data;
    data.asmType = f;
    data.type = t;
    data.assembly = a;
    marshallingFrom.push_back( data );
}

void Program::buildMarshalToAssembly()
{
    std::string a = load();
    std::string t = load();
    std::string f = load();
    for( auto& m : marshallingTo )
    {
        if( m.asmType == t && m.type == f )
        {
            if( m.assembly != a )
                throw std::runtime_error( "Cannot define marshalling! There is already existing marshalling but with different definition! From: " + f + " to: " + t );
            return;
        }
    }
    MarshalData data;
    data.asmType = t;
    data.type = f;
    data.assembly = a;
    marshallingTo.push_back( data );
}

void Program::buildVariableSet()
{
    if( m_builtExpressions.size() < 2 )
        throw std::runtime_error( "Cannot obtain variable and it's value!" );
    ExpressionPtr d = m_builtExpressions.back();
    m_builtExpressions.pop_back();
    ExpressionPtr v = m_builtExpressions.back();
    m_builtExpressions.pop_back();
    m_builtExpressions.push_back( ExpressionPtr( new Set( v, d ) ) );
}

void Program::buildIf()
{
    std::vector< ConditionData > cond;
    std::size_t p = restoreConditions();
    for( std::size_t i = p; i < m_conditions.size(); ++i )
        cond.push_back( m_conditions[i] );
    while( m_conditions.size() > p )
        m_conditions.pop_back();
    m_builtExpressions.push_back( ExpressionPtr( new Condition( cond ) ) );
}

void Program::buildCond()
{
    std::vector< ExpressionPtr > exp;
    std::size_t p = restoreExpressions();
    for( std::size_t i = p; i < m_builtExpressions.size(); ++i )
        exp.push_back( m_builtExpressions[i] );
    while( m_builtExpressions.size() > p )
        m_builtExpressions.pop_back();
    if( m_builtExpressions.size() < 1 )
        throw std::runtime_error( "Cannot obtain condition test value!" );
    ConditionData data;
    data.uid = ++Condition::s_testUID;
    data.condition = m_builtExpressions.back();
    m_builtExpressions.pop_back();
    data.expressions = exp;
    m_conditions.push_back( data );
}

void Program::buildElif()
{
    std::vector< ExpressionPtr > exp;
    std::size_t p = restoreExpressions();
    for( std::size_t i = p; i < m_builtExpressions.size(); ++i )
        exp.push_back( m_builtExpressions[i] );
    while( m_builtExpressions.size() > p )
        m_builtExpressions.pop_back();
    if( m_builtExpressions.size() < 1 )
        throw std::runtime_error( "Cannot obtain condition test value!" );
    ConditionData data;
    data.uid = ++Condition::s_testUID;
    data.condition = m_builtExpressions.back();
    m_builtExpressions.pop_back();
    data.expressions = exp;
    m_conditions.push_back( data );
}

void Program::buildElse()
{
    std::vector< ExpressionPtr > exp;
    std::size_t p = restoreExpressions();
    for( std::size_t i = p; i < m_builtExpressions.size(); ++i )
        exp.push_back( m_builtExpressions[i] );
    while( m_builtExpressions.size() > p )
        m_builtExpressions.pop_back();
    ConditionData data;
    data.uid = ++Condition::s_testUID;
    data.expressions = exp;
    m_conditions.push_back( data );
}
