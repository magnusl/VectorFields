#include "parser.hpp"
#include <sstream>

namespace vf
{
    using namespace std;

    std::ostream & operator << (std::ostream & os, const PositionInfo & pos) 
    {
        os << "Ln: " << pos.Row << ", Col: " << pos.Col;
        return os;
    }

    std::ostream & operator << (std::ostream & os, const DataType dt)
    {
        switch(dt) {
        case Type_Float:    os << "float";
        case Type_Vec2:     os << "vec2";
        case Type_Vec3:     os << "vec3";
        case Type_Vec4:     os << "vec4";
        case Type_Void:     os << "void";
        case Type_Sampler:  os << "sampler";
        case Type_Boolean:  os << "bool";
        case Type_Invalid:  os << "invalid";
        }
        return os;
    }

    /*************************************************************************/
    /*                              Warnings                                 */
    /*************************************************************************/
    void Parser::Warn_Shadow(const vf::PositionInfo &, vf::SymbolTable::SymIndex)
    {
    }

    /*************************************************************************/
    /*                          Error reporting                              */
    /*************************************************************************/
    void Parser::Err_Expected(const vf::PositionInfo & position, 
        vf::Symbol_t expected,
        vf::Symbol_t actual)
    {
        const char * pStrExpected = vf::Tokenizer::GetTokenString(expected);
        const char * pStrActual = vf::Tokenizer::GetTokenString(actual);

        stringstream ss;
        if (pStrExpected && pStrActual) {
            ss << "Expected token '" << pStrExpected << "' at " << position << " but encountered '" << pStrActual << "'.";
            SetErrorString(ss.str());
        }
    }

    void Parser::Err_AlreadyDeclared(const vf::Tokenizer & tokenizer, 
        const vf::PositionInfo & position, 
        vf::SymbolTable::SymIndex sym)
    {
        if (const char * pName = tokenizer.Lookup(sym)) {
            stringstream ss;
            ss << "Redefinition of variable " << pName << " at " << position << ".";
            SetErrorString(ss.str());
        }
    }

    void Parser::Err_Unexpected(const vf::PositionInfo & position, vf::Symbol_t symbol)
    {
        stringstream ss;
        if (const char * pName = Tokenizer::GetTokenString(symbol)) {
            ss << "Unexpected token " << pName << " at " << position << ".";
            SetErrorString(ss.str());
        }
    }

    void Parser::Err_Undeclared(const vf::Tokenizer & tokenizer, const vf::PositionInfo & position, vf::SymbolTable::SymIndex sym)
    {
        if (const char * pName = tokenizer.Lookup(sym)) {
            stringstream ss;
            ss << "Undeclared variable '" << pName << "' at " << position << ".";
            SetErrorString(ss.str());
        }
    }

    void Parser::Err_Memory(const char *)
    {
    }

    void Parser::Err_Message(const vf::PositionInfo & pos, const char * pMessage)
    {
        if (pMessage) {
            stringstream ss;
            ss << pMessage << " at " << pos << ".";
            SetErrorString(ss.str());
        }
    }

    void Parser::Err_Internal(const char *)
    {
    }

    void Parser::Err_InvalidMember(const vf::Tokenizer & tokenizer, 
        const vf::PositionInfo & position, 
        vf::DataType type, 
        vf::SymbolTable::SymIndex varName)
    {
        if (const char * pName = tokenizer.Lookup(varName)) {
            stringstream ss;
            ss << "'" << pName << "' is not a member of type '" << type << "'.";
            SetErrorString(ss.str());
        }
    }

    void Parser::Err_TypeExpected(const vf::Tokenizer &, const vf::PositionInfo & position)
    {
        stringstream ss;
        ss << "Expected a type declaration at " << position << ".";
        SetErrorString(ss.str());
    }
}