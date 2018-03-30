#include "token.hpp"
#include <cctype>
#include <string>
#include <memory>
#include <sstream>

#define stringify( name ) #name

namespace vf
{
    using std::string;

    /** Single characters tokens */
    static struct {
        char        c;
        Symbol_t    sym;
    } SingleTokens[] = {
        {';', T_SEMICOLON}, 
        {':', T_COLON}, 
        {'.', T_DOT},
        {'?', T_QUESTION},
        {',', T_COMMA},
        {'(', T_LEFT_PAREN}, 
        {')', T_RIGHT_PAREN}, 
        {'{', T_LEFT_CURLY_BRACKET}, 
        {'}', T_RIGHT_CURLY_BRACKET},
        {'[', T_LEFT_SQUARE_BRACKET}, 
        {']', T_RIGHT_SQUARE_BRACKET},
        {'+', T_ADD}, 
        {'-', T_SUB}, 
        {'*', T_MUL}, 
        {'/', T_DIV}
    };

    /** Keywords */
    static struct {
        const char *    keyword;
        Symbol_t        sym;
    } Keywords[] = {
        {"in", T_KEYWORD_IN}, 
        {"out", T_KEYWORD_OUT}, 
        {"inout", T_KEYWORD_INOUT}, 
        {"const", T_KEYWORD_CONST}, 
        {"uniform", T_KEYWORD_UNIFORM},
        {"void", T_TYPE_VOID}, 
        {"float", T_TYPE_FLOAT}, 
        {"vec2", T_TYPE_VEC2}, 
        {"vec3", T_TYPE_VEC3}, 
        {"vec4", T_TYPE_VEC4},
        {"return", T_KEYWORD_RETURN}, 
        {"dot", T_KEYWORD_DOT}, 
        {"cross", T_KEYWORD_CROSS}, 
        {"length", T_KEYWORD_LENGTH},
        {"sin", T_KEYWORD_SIN},
        {"asin", T_KEYWORD_ASIN},
        {"cos", T_KEYWORD_COS},
        {"acos", T_KEYWORD_ACOS},
        {"tan", T_KEYWORD_TAN},
        {"atan", T_KEYWORD_ATAN},
        {"accumulate", T_KEYWORD_ACCUM},
        {"normalize", T_KEYWORD_NORMALIZE},
        {"sqrt", T_KEYWORD_SQRT},
        {"invsqrt", T_KEYWORD_INVSQRT},
        {"max", T_KEYWORD_MAX},
        {"min", T_KEYWORD_MIN},
        {"floor", T_KEYWORD_FLOOR},
        {"ceil", T_KEYWORD_CEIL},
        {"sampler", T_KEYWORD_SAMPLER},
        {"sample1D", T_KEYWORD_SAMPLE1D},
        {"sample2D", T_KEYWORD_SAMPLE2D},
        {"sample3D", T_KEYWORD_SAMPLE3D},
    };

    bool Tokenizer::GetChar( char & c ) 
    {
        if (!m_Stream.get(c)) {
            return false;
        }

        /** Update the position */
        switch(c) {
        case '\n':      ++m_Position.Row; m_Position.Col = 0; break;
        case '\t':      m_Position.Col += 4; break;
        default:        ++m_Position.Col; break;
        }

        return true;
    }

    bool Tokenizer::Peek( char & c ) 
    {
        c = m_Stream.peek();
        if (c == EOF) {
            return false;
        }
        return true;
    }

    /**
     * Peek at the next symbol without consuming it.
     */
    Symbol_t Tokenizer::Peek()
    {
        if (!m_HasPeeked) {
            m_NextSym   = GetSym();
            m_HasPeeked = true;
        }
        return m_NextSym;
    }

    Symbol_t Tokenizer::GetSym()
    {
        m_State = TOK_INITIAL;
        if (m_HasPeeked) {
            m_HasPeeked = false;
            return m_NextSym;
        }
        char c;
        string value;
        if (!GetChar(c)) {
            return T_EOF;   /** end-of-file */
        }
        for(;;) {
            switch(m_State)
            {
            case TOK_INITIAL:    /** handle single character tokens */
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                    if (!GetChar(c)) {
                        return T_EOF;
                    }
                    continue;
                }
                for(size_t i = 0; i < sizeof(SingleTokens)/sizeof(SingleTokens[0]); i++) {
                    if (SingleTokens[i].c == c) {
                        return SingleTokens[i].sym;
                    }
                }
                if (c == '=') {
                    /** either assign or equal */
                    return (Peek(c) ? ((c == '=') ? (GetChar(c), T_EQUAL) : T_ASSIGN) : T_ASSIGN);
                } else if (c == '<') {
                    /** either T_LESS or T_LEQ */
                    return (Peek(c) ? ((c == '=') ? (GetChar(c), T_LEQ) : T_LESS) : T_LESS);
                } else if(c == '>') {
                    /** either T_GRT or T_GRT */
                    return (Peek(c) ? ((c == '=') ? (GetChar(c), T_GEQ) : T_GRT) : T_GRT);
                } else if (isdigit(c)) {
                    m_State = TOK_NUMERIC;
                    value += c;
                    continue;
                } else if(std::isalpha(c)) {
                    m_State = TOK_IDENT;
                    value += c;
                    continue;
                } else {
                    /** error */
                }
            case TOK_IDENT:             
                while(Peek(c) && (std::isalnum(c) || (c == '_'))) 
                {
                    GetChar(c);
                    value += c;
                }
                /** match it against known keywords */
                for(size_t i = 0; i < sizeof(Keywords)/sizeof(Keywords[0]); i++) {
                    if (value == Keywords[i].keyword) {
                        return Keywords[i].sym;
                    }
                }
                u.m_SymbolIndex = m_SymbolTable.Insert(value.c_str());
                return T_IDENT;
            case TOK_NUMERIC:   /** integer or floating point numbr */
                while(Peek(c)) {
                    if (std::isdigit(c)) {
                        GetChar(c);
                        value += c;
                    } else if (c == '.') {
                        GetChar(c);
                        m_State = TOK_FLOAT;
                        value += c;
                        break;
                    } else {
                        break;  /** return T_INTEGER */
                    }
                }
                if (m_State == TOK_NUMERIC) {
                    std::stringstream ss;
                    ss << value;
                    if (!(ss >> u.m_IntValue)) {
                        return T_FAILURE;
                    }
                    return T_INTEGER;
                }
                break;
            case TOK_FLOAT:
                while(Peek(c)) {
                    if (std::isdigit(c)) {
                        GetChar(c);
                        value += c;
                    } else if (c == 'f') {
                        GetChar(c);
                        break;
                    } else {
                        break;
                    }
                }
                /** convert to float */
                {
                    std::stringstream ss;
                    ss << value;
                    if (!(ss >> u.m_RealValue)) {
                        return T_FAILURE;
                    }
                    return T_REAL;
                }
                break;
            }
        }
    }

    const char * Tokenizer::GetTokenString(Symbol_t sym)
    {
        switch(sym) {
        case T_IDENT:                   return stringify(T_IDENT);
        case T_KEYWORD_IN:              return stringify(T_KEYWORD_IN);
        case T_KEYWORD_OUT:             return stringify(T_KEYWORD_OUT);
        case T_KEYWORD_INOUT:           return stringify(T_KEYWORD_INOUT);
        case T_KEYWORD_CONST:           return stringify(T_KEYWORD_CONST);
        case T_KEYWORD_ACCUM:           return stringify(T_KEYWORD_ACCUM);
        case T_KEYWORD_UNIFORM:         return stringify(T_KEYWORD_UNIFORM);
        case T_KEYWORD_RETURN:          return stringify(T_KEYWORD_RETURN);
        case T_KEYWORD_DOT:             return stringify(T_KEYWORD_DOT);
        case T_KEYWORD_CROSS:           return stringify(T_KEYWORD_CROSS);
        case T_KEYWORD_LENGTH:          return stringify(T_KEYWORD_LENGTH);
        case T_KEYWORD_SIN:             return stringify(T_KEYWORD_SIN);
        case T_KEYWORD_COS:             return stringify(T_KEYWORD_COS);
        case T_KEYWORD_ASIN:            return stringify(T_KEYWORD_ASIN);
        case T_KEYWORD_ACOS:            return stringify(T_KEYWORD_ACOS);   
        case T_KEYWORD_TAN:             return stringify(T_KEYWORD_TAN);
        case T_KEYWORD_ATAN:            return stringify(T_KEYWORD_ATAN);
        case T_KEYWORD_NORMALIZE:       return stringify(T_KEYWORD_NORMALIZE);
        case T_KEYWORD_SQRT:            return stringify(T_KEYWORD_SQRT);
        case T_KEYWORD_INVSQRT:         return stringify(T_KEYWORD_INSQRT);
        case T_KEYWORD_MIN:             return stringify(T_KEYWORD_MIN);
        case T_KEYWORD_MAX:             return stringify(T_KEYWORD_MAX);
        case T_KEYWORD_FLOOR:           return stringify(T_KEYWORD_FLOOR);
        case T_KEYWORD_CEIL:            return stringify(T_KEYWORD_CEIl);
        case T_KEYWORD_SAMPLER:         return stringify(T_KEYWORD_SAMPLER);
        case T_KEYWORD_SAMPLE1D:        return stringify(T_KEYWORD_SAMPLE1D);
        case T_KEYWORD_SAMPLE2D:        return stringify(T_KEYWORD_SAMPLE2D);
        case T_KEYWORD_SAMPLE3D:        return stringify(T_KEYWORD_SAMPLE3D);
        case T_TYPE_VOID:               return stringify(T_TYPE_VOID);
        case T_TYPE_FLOAT:              return stringify(T_TYPE_FLOAT);
        case T_TYPE_VEC2:               return stringify(T_TYPE_VEC2);
        case T_TYPE_VEC3:               return stringify(T_TYPE_VEC3);
        case T_TYPE_VEC4:               return stringify(T_TYPE_VEC4);
        case T_INTEGER:                 return stringify(T_TYPE_INTEGER);
        case T_REAL:                    return stringify(T_REAL);
        case T_ASSIGN:                  return stringify(T_ASSIGN);
        case T_QUESTION:                return stringify(T_QUESTION);
        case T_COMMA:                   return stringify(T_COMMA);
        case T_SEMICOLON:               return stringify(T_SEMICOLON);
        case T_COLON:                   return stringify(T_COLON);
        case T_DOT:                     return stringify(T_DOT);
        case T_LEFT_PAREN:              return stringify(T_LEFT_PAREN);
        case T_RIGHT_PAREN:             return stringify(T_RIGHT_PAREN);
        case T_LEFT_SQUARE_BRACKET:     return stringify(T_LEFT_SQUARE_BRACKET);
        case T_RIGHT_SQUARE_BRACKET:    return stringify(T_RIGHT_SQUARE_BRACKET);
        case T_LEFT_CURLY_BRACKET:      return stringify(T_LEFT_CURLY_BRACKET);
        case T_RIGHT_CURLY_BRACKET:     return stringify(T_RIGHT_CURLY_BRACKET);
        case T_ADD:                     return stringify(T_ADD);
        case T_SUB:                     return stringify(T_SUB);
        case T_MUL:                     return stringify(T_MUL);
        case T_DIV:                     return stringify(T_DIV);
        case T_EQUAL:                   return stringify(T_EQUAL);
        case T_LESS:                    return stringify(T_ASSIGN);
        case T_GRT:                     return stringify(T_GRT);
        case T_LEQ:                     return stringify(T_LEQ);
        case T_GEQ:                     return stringify(T_GEQ);
        case T_FAILURE:                 return stringify(T_FAILURE);
        case T_EOF:                     return stringify(T_EOF);
        default:                        return 0;
        }
    }
}