#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <i4/types.h>
#include <i4/compilation_state.h>
#include <memory>
#include <vector>
#include <stack>
#include <map>
#include <queue>
#include <ostream>

//#define DEBUG_PRINT

class Builder;

class Program
{
public:
    class Function;

    class Validated
    {
    public:
        Validated() : isValidated( false ) {}
        ~Validated() {}

        bool isValidated;
    };

    class Field : public Validated
    {
    public:
        Field( const std::string& i, const std::string& t );
        Field( const std::string& a );
        ~Field();

        void write( std::ostream& output, std::size_t level );
        void validate( Builder* builder, Program* program );
        void assemble( std::ostream& output );

        std::string id;
        std::string type;
        std::string assembly;
    };

    typedef std::shared_ptr< Field > FieldPtr;

    class Structure : public Validated
    {
    public:
        Structure( const std::string& i );
        ~Structure();

        void write( std::ostream& output, std::size_t level );
        void validate( Builder* builder, Program* program );
        void assemble( std::ostream& output );

        std::string id;
        std::vector< FieldPtr > fields;
    };

    typedef std::shared_ptr< Structure > StructurePtr;

    class Expression : public Validated
    {
    public:
        enum Type
        {
            T_CONSTANT_INTEGER,
            T_CONSTANT_FLOAT,
            T_CONSTANT_STRING,
            T_CONSTANT_NULL,
            T_CONSTANT_BOOL,
            T_VALUE,
            T_FUNCTION_CALL,
            T_ASM,
            T_SET
        };

        Expression( Type t );
        Expression( Type t, const std::string& rt );
        virtual ~Expression();

        std::string getResultType() { return m_resultType; }
        void setResultType( const std::string& v ) { m_resultType = v; }

        virtual void write( std::ostream& output, std::size_t level ) = 0;
        virtual void validate( Builder* builder, Program* program, Function* func ) = 0;
        virtual void assemble( std::ostream& output, bool dropValue ) = 0;

        Type type;

    private:
        std::string m_resultType;
    };

    typedef std::shared_ptr< Expression > ExpressionPtr;

    class ConstantInteger : public Expression
    {
    public:
        ConstantInteger( I4::i32 v );
        virtual ~ConstantInteger();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func ) {}
        virtual void assemble( std::ostream& output, bool dropValue );

        I4::i32 value;
    };

    typedef std::shared_ptr< ConstantInteger > ConstantIntegerPtr;

    class ConstantFloat : public Expression
    {
    public:
        ConstantFloat( I4::f32 v );
        virtual ~ConstantFloat();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func ) {}
        virtual void assemble( std::ostream& output, bool dropValue );

        I4::f32 value;
    };

    typedef std::shared_ptr< ConstantFloat > ConstantFloatPtr;

    class ConstantString : public Expression
    {
    public:
        ConstantString( const std::string& v );
        virtual ~ConstantString();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func ) {}
        virtual void assemble( std::ostream& output, bool dropValue );

        std::string value;
    };

    typedef std::shared_ptr< ConstantString > ConstantStringPtr;

    class ConstantNull : public Expression
    {
    public:
        ConstantNull();
        virtual ~ConstantNull();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func ) {}
        virtual void assemble( std::ostream& output, bool dropValue );
    };

    typedef std::shared_ptr< ConstantNull > ConstantNullPtr;

    class ConstantBool : public Expression
    {
    public:
        ConstantBool( bool v );
        virtual ~ConstantBool();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func ) {}
        virtual void assemble( std::ostream& output, bool dropValue );

        bool value;
    };

    typedef std::shared_ptr< ConstantBool > ConstantBoolPtr;

    class Value : public Expression
    {
    public:
        Value( const std::vector< std::string >& i );
        virtual ~Value();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func );
        virtual void assemble( std::ostream& output, bool dropValue );

        std::vector< std::string > ids;
    };

    typedef std::shared_ptr< Value > ValuePtr;

    class FunctionCall : public Expression
    {
    public:
        FunctionCall( const std::string& i, const std::vector< ExpressionPtr >& a );
        virtual ~FunctionCall();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func );
        virtual void assemble( std::ostream& output, bool dropValue );

        std::string id;
        std::vector< ExpressionPtr > args;

    private:
        std::string m_uid;
    };

    typedef std::shared_ptr< FunctionCall > FunctionCallPtr;

    class Asm : public Expression
    {
    public:
        Asm( const std::string& c );
        virtual ~Asm();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func );
        virtual void assemble( std::ostream& output, bool dropValue );

        std::string content;
    };

    typedef std::shared_ptr< Asm > AsmPtr;

    class Set : public Expression
    {
    public:
        Set( const ExpressionPtr& v, const ExpressionPtr& d );
        virtual ~Set();

        virtual void write( std::ostream& output, std::size_t level );
        virtual void validate( Builder* builder, Program* program, Function* func );
        virtual void assemble( std::ostream& output, bool dropValue );

        ExpressionPtr value;
        ExpressionPtr data;
    };

    typedef std::shared_ptr< Set > SetPtr;

    class Function : public Validated
    {
    public:
        Function( const std::string& i, const std::string& t );
        ~Function();

        void write( std::ostream& output, std::size_t level );
        void validate( Builder* builder, Program* program );
        void assemble( std::ostream& output, Program* program );
        std::string makeUID();

        std::string id;
        std::string type;
        std::vector< FieldPtr > args;
        std::vector< FieldPtr > vars;
        std::vector< ExpressionPtr > expressions;
        bool returnPlacement;
        std::string nativeModulePackage;
        StructurePtr constructor;
        StructurePtr disposal;
        std::string jaegerifiedId;

    private:
        std::string m_uid;
    };

    typedef std::shared_ptr< Function > FunctionPtr;

    struct JaegerifyData
    {
        std::string routine;
        FunctionPtr func;
        std::string module;
        std::string method;
    };

    struct MarshalData
    {
        std::string asmType;
        std::string type;
        std::string assembly;
    };

    Program();
    ~Program();

    std::size_t getStackSize() { return m_stack.size(); }
    std::string getLastFunction() { return m_lastFunctionDefinition; }
    void clearLastFunctionDefinition() { m_lastFunctionDefinition.clear(); }
    void pushSectionsTarget() { m_sections.push( std::queue< std::vector< ExpressionPtr > >() ); }
    void popSectionsTarget() { m_sections.pop(); }
    void markStrict() { m_strict = true; }
    bool isStrict() { return m_strict; }
    I4::CompilationState::RoutinePtr getNativeModule( const std::string& id ) { return m_nativeModules.count( id ) ? m_nativeModules[id] : nullptr; }

    void write( std::ostream& output );
    void validate( Builder* builder );
    I4::CompilationStatePtr assemble( Builder* builder, std::size_t stackSize );
    bool verifyMarshalling( I4::CompilationState::Type* fromType, const std::string& toType );
    std::string marshallField( const std::string& id, const std::string& type, const std::string& asmType );
    std::string marshallField( const FieldPtr& field, const std::string& asmType );
    void marshallValue( std::ostream& output, const FieldPtr& field, const std::string& asmType );
    void marshallObject( std::ostream& output, const std::string& id, const std::string& type, const std::string& asmType );
    Structure* findValueStructure( const std::vector< std::string >& ids, Function* func );
    Structure* findValueStructure( const std::vector< std::string >& ids, const std::string& type, std::size_t index );
    void loadImports( Builder* builder );
    void linkProgram( Program* program );
    void save( const std::string& v );
    std::string load();
    void discard();
    void store();
    std::size_t restore();
    void storeExpressions();
    std::size_t restoreExpressions();
    void buildStructure();
    void buildField();
    void buildFieldFromAsm();
    void buildFunctionArgs();
    void buildFunctionVars();
    void buildFunction();
    void buildConstantInteger( I4::i32 v );
    void buildConstantFloat( I4::f32 v );
    void buildConstantString( const std::string& v );
    void buildConstantNull();
    void buildConstantBool( bool v );
    void buildValue( const std::vector< std::string >& ids );
    void buildFunctionCallSection();
    void buildFunctionCall();
    void buildAsm( bool isGlobal );
    void markReturnPlacement();
    void buildImport();
    void buildJaegerify();
    void buildMarshalFromAssembly();
    void buildMarshalToAssembly();
    void buildVariableSet();

    std::string startFunction;
    std::vector< std::string > imports;
    std::vector< MarshalData > marshallingFrom;
    std::vector< MarshalData > marshallingTo;
    std::vector< AsmPtr > globalAsm;
    std::map< std::string, JaegerifyData > jaegerified;
    std::map< std::string, StructurePtr > structures;
    std::map< std::string, FunctionPtr > functions;

private:
    bool m_strict;
    std::stack< std::string > m_stack;
    std::stack< std::size_t > m_stackPositions;
    std::vector< FieldPtr > m_builtFields;
    std::vector< ExpressionPtr > m_builtExpressions;
    std::stack< std::queue< std::vector< ExpressionPtr > > > m_sections;
    std::stack< std::size_t > m_expressionsPositions;
    std::vector< FieldPtr > m_builtArgs;
    std::vector< FieldPtr > m_builtVars;
    bool m_returnPlacement;
    std::string m_lastFunctionDefinition;
    std::map< std::string, I4::CompilationState::RoutinePtr > m_nativeModules;
};

typedef std::shared_ptr< Program > ProgramPtr;

#endif
