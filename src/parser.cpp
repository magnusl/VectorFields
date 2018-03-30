#include "parser.hpp"
#include <memory>
#include <sstream>

namespace vf
{
    using namespace std;

    /*************************************************************************/
    /*                                  Utility                              */
    /*************************************************************************/

    /**
     * Returns true if the symbol represents a type.
     */
    inline bool IsType(vf::Symbol_t sym)
    {
        switch(sym) {
        case vf::T_TYPE_FLOAT:
        case vf::T_TYPE_VEC2:
        case vf::T_TYPE_VEC3:
        case vf::T_TYPE_VEC4:
            return true;
        }
        return false;
    }

    /**
     * Translates a token to a type.
     */
    inline DataType GetType(vf::Symbol_t sym)
    {
        switch(sym) {
        case vf::T_TYPE_FLOAT:  return vf::Type_Float;
        case vf::T_TYPE_VEC2:   return vf::Type_Vec2;
        case vf::T_TYPE_VEC3:   return vf::Type_Vec3;
        case vf::T_TYPE_VEC4:   return vf::Type_Vec4;
        default:                return vf::Type_Invalid;
        }
    }
    
    static bool GetMemberInfo
        (
        vf::DataType    type,               // [in] variable type
        const char *    pName,              // [in] name of the referenced member
        vf::DataType &  referencedType,     // [out] type of the referenced member
        uint8_t &       offset              // [out] offset in register, only used for scalar members.
        )
    {
        size_t size = GetNumMembers(type);
        if (strlen(pName) == 1) {
            int regOffset = GetRegisterOffset(pName);
            if (regOffset < 0) {
                return false;
            }
            offset          = static_cast<uint8_t>(regOffset);
            referencedType  = vf::Type_Float;
        } else if (!strcmp(pName, "xy") && (size >= 2)) {
            offset          = 0;
            referencedType  = vf::Type_Vec2;
        } else if (!strcmp(pName, "xyz") && (size >= 3)) {
            offset          = 0;
            referencedType  = vf::Type_Vec3;
        } else if (!strcmp(pName, "xyzw") && (size >= 4)) {
            offset          = 0;
            referencedType  = vf::Type_Vec4;
        } else {
            return false;
        }
        return true;
    }

    /*************************************************************************/
    /*                          Parser utility functions                     */
    /*************************************************************************/

    /** 
     * Returns true if the next symbol in the symbol stream is the expected symbol.
     */
    bool Parser::Expect(vf::Symbol_t expectedSymbol, vf::Tokenizer & tokenizer)
    {
        vf::Symbol_t sym = tokenizer.GetSym();
        return (sym == expectedSymbol ? true : (Err_Expected(tokenizer.Position(), expectedSymbol, sym), false));
    }

    /**
     * Consumes the next symbol in the stream, or returns false if T_EOF is encountered.
     */
    bool Parser::Consume(vf::Tokenizer & tokenizer)
    {
        vf::Symbol_t sym = tokenizer.GetSym();
        return (sym != vf::T_EOF) ? true : (Err_Unexpected(tokenizer.Position(), sym), false); 
    }

    /**
     * Parses a variable type.
     */
    std::shared_ptr<Node_Type> Parser::VF_Type(vf::Tokenizer & tokenizer)
    {
        vf::Symbol_t sym = tokenizer.GetSym();
        switch(sym) {
        case T_TYPE_VOID:   return std::make_shared<Node_Type>(vf::Type_Void);
        case T_TYPE_FLOAT:  return std::make_shared<Node_Type>(vf::Type_Float);
        case T_TYPE_VEC2:   return std::make_shared<Node_Type>(vf::Type_Vec2);
        case T_TYPE_VEC3:   return std::make_shared<Node_Type>(vf::Type_Vec3);
        case T_TYPE_VEC4:   return std::make_shared<Node_Type>(vf::Type_Vec4);
            return nullptr;
        }
        return nullptr;
    }

    /*************************************************************************/
    /*                              Environment                              */
    /*************************************************************************/

    /**
     * Returns the current environment.
     */
    vf::Environment * Parser::CurrentEnv()
    {
        return m_pCurrentEnv;
    }

    /**
     * Parses the program supplied as a string.
     */
    shared_ptr<Node_Program> Parser::Parse(const char * pCodeStr)
    {
        std::stringstream ss(pCodeStr);
        vf::Tokenizer token(ss);
        return VF_Program(token);
    }

    /**
     * Uses the supplied tokenizer to parse a program.
     */
    shared_ptr<Node_Program> Parser::Parse(vf::Tokenizer & tokenizer)
    {
        return VF_Program(tokenizer);
    }

    /**
     * Uses the supplied tokenizer to parse a program.
     */
    shared_ptr<Node_Program> Parser::VF_Program(vf::Tokenizer & tokenizer)
    {
        shared_ptr<Node_Program> pProgram = make_shared<Node_Program>();
        vf::Symbol_t sym;
        while((sym = tokenizer.Peek()) != T_EOF) {
            switch(sym) {
            case T_KEYWORD_IN:
            case T_KEYWORD_OUT:
            case T_KEYWORD_INOUT:
            case T_KEYWORD_CONST:
            case T_KEYWORD_UNIFORM:
            case T_KEYWORD_SAMPLER:
                if (pProgram->GetFunctions().size()) {
                    return (Err_Message(tokenizer.Position(), "global variables must be declared before any function"), nullptr);
                }
                if (!VF_GlobalVariable(tokenizer)) {
                    return false;
                }
                break;
            default:
                {
                    shared_ptr<Node_Function> pFunction = VF_Function(tokenizer);
                    if (pFunction == nullptr) {
                        return false;
                    }
                    /** add the parsed function to the program */
                    pProgram->AddFunction( pFunction );
                }
            }
        }
        return pProgram;
    }

    /*************************************************************************/
    /*                              Declarations                             */
    /*************************************************************************/

    /** Parses a declaration of a uniform variable */
    bool Parser::VF_UniformVariable(vf::Tokenizer & tokenizer)
    {
        /** uniform keyword */
        if (!Expect(vf::T_KEYWORD_UNIFORM, tokenizer)) {
            return false;
        }
        vf::DataType type = GetType(tokenizer.GetSym());
        if (type == vf::Type_Invalid) {
            return (Err_TypeExpected(tokenizer, tokenizer.Position()), false);
        }
        if (!Expect(vf::T_IDENT, tokenizer)) {
            return false;
        }
        vf::SymbolTable::SymIndex SymIndex = tokenizer.SymIndex();
        if (!Expect(vf::T_SEMICOLON, tokenizer)) {
            return false;
        }
        if (!CurrentEnv()->DeclareVariable(SymIndex, type, vf::ATTRIBUTE_UNIFORM)) {
            return (Err_AlreadyDeclared(tokenizer, tokenizer.Position(), SymIndex), false);
        }
        return true;
    }

    /**
     * Parses a constant value, which can either be a single floating point value, or a 
     * a vector which is enclosed in curly brackets.

     */
    bool Parser::VF_ConstValue(vf::Tokenizer & tokenizer, vf::DataType & type, vf::Vector & value)
    {
        vf::Symbol_t sym = tokenizer.GetSym();
        if (sym == vf::T_REAL) {
            /** Single scalar constant */
            type        = vf::Type_Float;
            value.u.f   = tokenizer.RealValue();
        } else if(sym == vf::T_INTEGER) {
            type        = vf::Type_Float;
            value.u.f   = static_cast<float>(tokenizer.IntValue());
        } else if (sym == vf::T_LEFT_CURLY_BRACKET) {
            /** Vector constant */
            size_t elementIndex = 0;
            if (!Expect(vf::T_REAL, tokenizer)) {
                return false;
            }
            value[elementIndex++] = tokenizer.RealValue();
            while((sym = tokenizer.Peek()) == vf::T_COMMA) {
                if (elementIndex >= 4) {
                    return (Err_Message(tokenizer.Position(), "To many elements in initialization list."), false);
                }
                Consume(tokenizer);
                if (!Expect(vf::T_REAL, tokenizer)) {
                    return false;
                }
                value[elementIndex++] = tokenizer.RealValue();
            }
            if (!Expect(vf::T_RIGHT_CURLY_BRACKET, tokenizer)) {
                return false;
            }
            switch(elementIndex) {
            case 1: type = vf::Type_Float; break;
            case 2: type = vf::Type_Vec2; break;
            case 3: type = vf::Type_Vec3; break;
            case 4: type = vf::Type_Vec4; break;
            }
        } else {
            return (Err_Unexpected(tokenizer.Position(), sym), false);
        }
        return true;
    }

    /** Parses a declaration of a constant variable */
    bool Parser::VF_ConstVariable(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_KEYWORD_CONST, tokenizer)) {
            return false;
        }
        vf::DataType type = GetType(tokenizer.GetSym());
        if (type == vf::Type_Invalid) {
            return (Err_Message(tokenizer.Position(), "Expected a type declaration after T_CONST."), false);
        }
        if (!Expect(vf::T_IDENT, tokenizer)) {
            return false;
        }
        vf::SymbolTable::SymIndex SymIndex = tokenizer.SymIndex();
        
        if (!Expect(vf::T_ASSIGN, tokenizer)) {
            return false;
        }

        /** Parse the constant */
        vf::DataType    constType;
        vf::Vector      value;
        if (!VF_ConstValue(tokenizer, constType, value)) {
            return false;
        }

        if (constType != type) {
            return (Err_Message(tokenizer.Position(), "Type mismatch between variable type and constant value."), false);
        }

        if (!Expect(vf::T_SEMICOLON, tokenizer)) {
            return false;
        }

        if (!CurrentEnv()->DeclareConstant(SymIndex, constType, value)) {
            return (Err_AlreadyDeclared(tokenizer, tokenizer.Position(), SymIndex), false);
        }
        return true;
    }

    /**
     * Parses the declaration of a input/output variable, which is declared with the in, out
     * or the inout keyword.
     */
    bool Parser::VF_IOVariable(vf::Tokenizer & tokenizer)
    {
        Symbol_t sym = tokenizer.GetSym();
        
        VariableAttribute attribute;
        switch(sym) {
        case vf::T_KEYWORD_IN:      attribute = vf::ATTRIBUTE_IN; break;
        case vf::T_KEYWORD_INOUT:   attribute = vf::ATTRIBUTE_INOUT; break;
        case vf::T_KEYWORD_OUT:     attribute = vf::ATTRIBUTE_OUT; break;
        default:
            return (Err_Message(tokenizer.Position(), "Unexpected token, expected in, out or inout attribute."), false);
        }
        sym = tokenizer.Peek();

        bool accumulate = false;
        if (sym == vf::T_KEYWORD_ACCUM) {
            if ((attribute != vf::ATTRIBUTE_INOUT) && (attribute != vf::ATTRIBUTE_OUT)) {
                return (Err_Message(tokenizer.Position(), "Only output variables can have the 'accumulate' attribute."), false);
            }
            tokenizer.GetSym();
            accumulate = true;
        }

        vf::DataType type = GetType(tokenizer.GetSym());
        if (type == vf::Type_Invalid) {
            return (Err_Message(tokenizer.Position(), "Expected a type declaration after attribute."), false);
        }
        if (!Expect(vf::T_IDENT, tokenizer)) {
            return false;
        }
        vf::SymbolTable::SymIndex SymIndex = tokenizer.SymIndex();
        if (!Expect(vf::T_SEMICOLON, tokenizer)) {
            return false;
        }

        if (!CurrentEnv()->DeclareVariable(SymIndex, type, attribute, accumulate)) {
            return (Err_AlreadyDeclared(tokenizer, tokenizer.Position(), SymIndex), false);
        }

        return true;
    }

    /**
     * Declares a sample variable
     */
    bool Parser::VF_SampleVariable(vf::Tokenizer & tokenizer)
    {
        Symbol_t sym                    = tokenizer.GetSym();
        VariableAttribute attr          = vf::ATTRIBUTE_SAMPLER;
        vf::DataType type               = vf::Type_Sampler;

        if (!Expect(T_IDENT, tokenizer)) {
            return false;
        }

        SymbolTable::SymIndex SymIndex  = tokenizer.SymIndex();

        if (!Expect(T_SEMICOLON, tokenizer)) {
            return false;
        }

        if (!CurrentEnv()->DeclareVariable(SymIndex, type, attr)) {
            return (Err_AlreadyDeclared(tokenizer, tokenizer.Position(), SymIndex), false);
        }
        return true;
    }

    /**
     * Parses the definition of a global variable. Each global variable starts with
     * a attribute (in, out, inout, const, uniform)
     */
    bool Parser::VF_GlobalVariable(vf::Tokenizer & tokenizer)
    {
        /** attribute type name e.g. 'in vec4 position;' */
        vf::Symbol_t            sym = tokenizer.Peek();        /** attribute */

        switch(sym) {
        case vf::T_KEYWORD_CONST:
            return VF_ConstVariable(tokenizer);
        case vf::T_KEYWORD_IN:
        case vf::T_KEYWORD_OUT:
        case vf::T_KEYWORD_INOUT:
            return VF_IOVariable(tokenizer);
        case vf::T_KEYWORD_UNIFORM:
            return VF_UniformVariable(tokenizer);
        case vf::T_KEYWORD_SAMPLER:
            return VF_SampleVariable(tokenizer);
        default:
            return (Err_Unexpected(tokenizer.Position(), sym), false);
        }
    }

    /**
     * Parses a function definition.
     *
     * \param   tokenizer   The tokenizer to extract the symbol stream from.
     * \return  The parsed function, or nullptr on error.
     */
    shared_ptr<Node_Function> Parser::VF_Function(vf::Tokenizer & tokenizer)
    {
        vf::Symbol_t sym;
        
        shared_ptr<Node_Type> type        = VF_Type(tokenizer);
        if (!type) {
            return (Err_Message(tokenizer.Position(), "Expected type in function declaration."), nullptr);
        }

        if (!Expect(vf::T_IDENT, tokenizer)) {
            return nullptr;
        }

        vf::SymbolTable::SymIndex name = tokenizer.SymIndex();
        /** enter a new environment, arguments are declared in this one */
        m_pCurrentEnv = CurrentEnv()->Enter(name);
        if (!m_pCurrentEnv) {
            return (Err_Internal("Failed to get the current environment."), nullptr);
        }

        if (!Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }

        shared_ptr<Node_Function> func = make_shared<Node_Function>(name, type);
        /** parse the function body which is enclosed by '{' and '}' */
        if (!Expect(vf::T_LEFT_CURLY_BRACKET, tokenizer)) {
            return nullptr;
        }
        sym = tokenizer.Peek();
        while(sym != vf::T_RIGHT_CURLY_BRACKET) 
        {
            shared_ptr<Node_Statement> pStmt = VF_Statement(tokenizer);
            if (!pStmt) {
                return nullptr;
            }
            func->Append( pStmt );
            sym = tokenizer.Peek();
        }
        if (!Expect(vf::T_RIGHT_CURLY_BRACKET, tokenizer)) {
            return nullptr;
        }

        m_pCurrentEnv = CurrentEnv()->Leave();
        return func;
    }

    /**
     * Parses a variable declaration, which is always on the form 
     * 'type identifier = expression'.
     */
     shared_ptr<Node_Assignment> Parser::VF_VariableDecl(vf::Tokenizer & tokenizer)
     {
         vf::Symbol_t sym = tokenizer.GetSym();
         if (!IsType(sym)) {
             Err_Message(tokenizer.Position(), "Expected a type declaration.");
             return nullptr;
         }

         vf::DataType type          = GetType(sym);

         if (!Expect(vf::T_IDENT, tokenizer)) {
             return nullptr;
         }

         vf::SymbolTable::SymIndex name = tokenizer.SymIndex();
         if (!Expect(vf::T_ASSIGN, tokenizer)) {
             return nullptr;
         }

         shared_ptr<Node_Expression> exp = VF_Expression(tokenizer);
         if (exp == nullptr) {
             return nullptr;
         }

         if (!Expect(T_SEMICOLON, tokenizer)) {
             return nullptr;
         }

         /** Search the current environment for the symbol */
         auto pCurrentEnv         = CurrentEnv();
         vf::Environment * result = pCurrentEnv->Lookup(name);

         if (result == pCurrentEnv) {
             return (Err_AlreadyDeclared(tokenizer, tokenizer.Position(), name), nullptr);
         } else if (result != nullptr) {
             Warn_Shadow(tokenizer.Position(), name);
         }
         /** declare the variable in the environment */
         if (!pCurrentEnv->DeclareVariable(name, type, vf::ATTRIBUTE_LOCAL)) {
             return (Err_AlreadyDeclared(tokenizer, tokenizer.Position(), name), nullptr);
         }
         return make_shared<Node_Assignment>(name, exp);
     }

    /*************************************************************************/
    /*                              Expressions                              */
    /*************************************************************************/

    /**
     * Parses a constant, which can either be scalar or a vector.
     * A vector constant is enclosed in '{ ... }'
     */
    shared_ptr<Node_Constant> Parser::VF_Constant(vf::Tokenizer & tokenizer)
    {
        vf::DataType    type;
        vf::Vector      value;
        if (!VF_ConstValue(tokenizer, type, value)) {
            return nullptr;
        }
        return make_shared<Node_Constant>(type, value);
    }

    /**
     * Parses a identifier which can either be a variable or a reference to a member of a variable.
     *
     * \param   tokenizer   Contains the stream of tokens.
     * \return  The parsed identifier, or nullptr on error.
     */
    shared_ptr<Node_Identifier> Parser::VF_Identifier(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_IDENT, tokenizer)) {
            return nullptr;
        }

        vf::Variable var;
        vf::SymbolTable::SymIndex varName = tokenizer.SymIndex();
        auto pEnv = CurrentEnv();

        /** Peek at the next symbol to determine if it's a identifier or a reference to a member variable */
        vf::Symbol_t sym = tokenizer.Peek();
        if (sym != vf::T_DOT) {
            if (!pEnv->Lookup(varName, var)) {
                return (Err_Undeclared(tokenizer, tokenizer.Position(), varName), nullptr);
            }
            shared_ptr<Node_Identifier> ident = make_shared<Node_Identifier>(varName);
            ident->m_expType        = var.m_Type;
            return ident;
        } else { /* x.y */
            vf::DataType referencedType;
            uint8_t offset;

            if (!pEnv->Lookup(varName, var)) {
                return (Consume(tokenizer), Err_Undeclared(tokenizer, tokenizer.Position(), varName), nullptr);
            }
            if (!isConstructed(var.m_Type)) {
                return (Consume(tokenizer), Err_Message(tokenizer.Position(), 
                    "Scalar variables does not have any member variables."), nullptr);
            }

            if (!Consume(tokenizer) || !Expect(vf::T_IDENT, tokenizer)) {
                return nullptr;
            }
            vf::SymbolTable::SymIndex memberName = tokenizer.SymIndex();
            if (!GetMemberInfo(
                var.m_Type,                                             // the type of the parent,
                tokenizer.SymbolTable().Retrive(memberName),            // the name of the referenced member
                referencedType,                                         // the type of the referenced variable,
                offset                                                  // offset, only used for scalar members
                ))
            {
                /** No such member */
                return (Err_InvalidMember(tokenizer, tokenizer.Position(), var.m_Type, memberName), nullptr);
            }

            shared_ptr<Node_MemberReference> ref    = make_shared<Node_MemberReference>(varName);
            ref->m_expType                          = referencedType;//var.m_Type;
            ref->m_MemberType                       = referencedType;
            ref->m_MemberOffset                     = offset;
            return ref;
        }
    }



    /**
     * Parses a expression.
     * 
     * \param       tokenizer       Stream with tokens.
     * \return      The parsed expression, or nullptr on error.
     */
    shared_ptr<Node_Expression> Parser::VF_Expression(vf::Tokenizer & tokenizer)
    {
        shared_ptr<Node_Expression> exp = VF_Compare(tokenizer);
        if (tokenizer.Peek() != vf::T_QUESTION) {
            return exp;
        }
        if (exp->Type() != vf::Node_Expression::EXP_COMPARE) {
            return shared_ptr<Node_Expression>();
        }
        tokenizer.GetSym();
        shared_ptr<Node_Expression> left = VF_Compare(tokenizer);
        if (!left) {
            return shared_ptr<Node_Expression>();
        }
        if (!Expect(vf::T_COLON, tokenizer)) {
            return shared_ptr<Node_Expression>();
        }
        shared_ptr<Node_Expression> right = VF_Compare(tokenizer);
        if (!right) {
            return shared_ptr<Node_Expression>();
        }

        return make_shared<vf::Node_Conditional>(exp, left, right);
    }

    shared_ptr<Node_Expression> Parser::VF_Compare(vf::Tokenizer & tokenizer)
    {
        shared_ptr<Node_Expression> exp = VF_AddSub(tokenizer);
        if (!exp) {
            return shared_ptr<Node_Expression>();
        }
        vf::Symbol_t sym = tokenizer.Peek();
        if (sym == vf::T_LESS || sym == vf::T_GRT || sym == vf::T_EQUAL || sym == T_LEQ || sym == T_GEQ) {
            sym = tokenizer.GetSym();
            if (exp->m_expType != vf::Type_Float) {
                return shared_ptr<Node_Expression>();
            }
            shared_ptr<Node_Expression> exp2 = VF_AddSub(tokenizer);
            if (!exp2) {
                return shared_ptr<Node_Expression>();
            }
            if (exp2->m_expType != vf::Type_Float) {
                return shared_ptr<Node_Expression>();
            }
            exp = make_shared<Node_Comparison>(sym, exp, exp2);
        }
        
        return exp;
    }

    shared_ptr<Node_Expression> Parser::VF_AddSub(vf::Tokenizer & tokenizer)
    {
        /** multiplication and division takes precedence over '+' and '-' */
       shared_ptr<Node_Expression> exp = VF_Term(tokenizer);
       if (exp == nullptr) {
            return shared_ptr<Node_Expression>();
       }
       vf::Symbol_t sym = tokenizer.Peek();
       while(sym == vf::T_ADD || sym == vf::T_SUB) {
           /** addition and subtraction */
           Consume(tokenizer);
           shared_ptr<Node_Expression> exp2 = VF_Term(tokenizer);
           if (exp2 == nullptr) {
               return shared_ptr<Node_Expression>();
           }
           if (exp->m_expType != exp2->m_expType) {
               return (Err_Message(tokenizer.Position(), "Addition/Subtraction, both expressions must have the same type."), nullptr);
           }
           exp = make_shared<Node_Binary>(sym == vf::T_ADD ? Node_Binary::Op_Add : Node_Binary::Op_Sub, exp, exp2);
           exp->m_expType = exp2->m_expType;
           sym = tokenizer.Peek();
       }
       return exp;
    }

    /**
     * Parses a term
     */
    shared_ptr<Node_Expression> Parser::VF_Term(vf::Tokenizer & tokenizer)
    {
        shared_ptr<Node_Expression> factor = VF_Factor(tokenizer);
        if (factor == nullptr) {
            return nullptr;
        }
        vf::Symbol_t sym = tokenizer.Peek();
        while(sym == vf::T_MUL || sym == vf::T_DIV) {
            Consume(tokenizer);
            shared_ptr<Node_Expression> factor2 = VF_Factor(tokenizer);
            if (factor2 == nullptr) {
                return nullptr;
            }

            vf::DataType resultType;
            /**
             * A vector can be multiplied with a scalar value and be divided with a scalar value.
             */
            if ((factor->m_expType == vf::Type_Float) && (factor2->m_expType == vf::Type_Float)) {
                /** A scalar multiplication or division */
                resultType = vf::Type_Float;
            } else if (factor->m_expType == vf::Type_Float) {   /** left hand side is a float */
                if (sym == vf::T_MUL) { /** OK: float * vector, make sure that the rhs is the float */
                    resultType = factor2->m_expType;
                    std::swap(factor, factor2);
                } else {    /** ERROR: float / vector */
                    return (Err_Message(tokenizer.Position(), "Cannot divide a float with a vector."), nullptr);
                }
            } else if (factor2->m_expType == vf::Type_Float) {
                /** right hand side is a float, which is ok for botk T_MUL and T_DIV */
                resultType = factor->m_expType;
            } else if (factor->m_expType != factor2->m_expType) {
                return (Err_Message (tokenizer.Position(), 
                    "Operands must have the same type for a division/multiplication."), nullptr);
            }

            factor              = make_shared<Node_Binary>(
                sym == T_MUL ? Node_Binary::Op_Mul : Node_Binary::Op_Div, factor, factor2);
            factor->m_expType   = resultType;
            sym                 = tokenizer.Peek();
        }
        return factor;
    }

    /**
     * Parses a factor.
     */
    shared_ptr<Node_Expression> Parser::VF_Factor(vf::Tokenizer & tokenizer)
    {
        vf::Symbol_t sym = tokenizer.Peek();
        if (sym == vf::T_REAL || sym == vf::T_INTEGER) {
            return VF_Constant(tokenizer);
        } else if (sym == vf::T_SUB) {
            return VF_Negate(tokenizer);
        } else if (sym == vf::T_IDENT) {
            return VF_Identifier(tokenizer);
        } else if (sym == vf::T_LEFT_PAREN) {
            Consume(tokenizer);
            shared_ptr<Node_Expression> node = VF_Expression(tokenizer);
            if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
                return nullptr;
            }
            return node;
        } else if (sym == vf::T_KEYWORD_DOT) {
            return VF_DotProduct(tokenizer);
        } else if (sym == vf::T_KEYWORD_CROSS) {
            return VF_CrossProduct(tokenizer);
        } else if (sym == vf::T_KEYWORD_LENGTH) {
            return VF_Length(tokenizer);
        } else if (sym == vf::T_KEYWORD_COS) {
            return VF_Cosine(tokenizer);
        } else if (sym == vf::T_KEYWORD_SIN) {
            return VF_Sine(tokenizer);
        } else if (sym == vf::T_KEYWORD_ACOS) {
            return VF_ArcCosine(tokenizer);
        } else if (sym == vf::T_KEYWORD_ASIN) {
            return VF_ArcSine(tokenizer);
        } else if (sym == vf::T_KEYWORD_TAN) {
            return VF_Tangent(tokenizer);
        } else if (sym == vf::T_KEYWORD_ATAN) {
            return VF_ArcTangent(tokenizer);
        } else if (sym == vf::T_KEYWORD_NORMALIZE) {
            return VF_Normalize(tokenizer);
        } else if (sym == vf::T_KEYWORD_SQRT) {
            return VF_Sqrt(tokenizer);
        } else if (sym == vf::T_KEYWORD_INVSQRT) {
            return VF_InvSqrt(tokenizer);
        } else if ((sym == vf::T_KEYWORD_MIN) || (sym == vf::T_KEYWORD_MAX)) {
            return VF_MinMax(tokenizer);
        } else if ((sym == vf::T_KEYWORD_FLOOR) || (sym == vf::T_KEYWORD_CEIL)) {
            return VF_FloorCeil(tokenizer);
        } else if ((sym == T_KEYWORD_SAMPLE1D) || (sym == T_KEYWORD_SAMPLE2D) || (sym == T_KEYWORD_SAMPLE3D)) {
            return VF_Sampler(tokenizer);
        } else {
            return nullptr;
        }
    }

    /**
     * Vector dot product, dot(v, u)
     */
    shared_ptr<Node_DotProduct> Parser::VF_DotProduct(vf::Tokenizer & tokenizer)
    {
        /** dot product, dot(u, v) */
        if (!Expect(vf::T_KEYWORD_DOT, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (!Expect(vf::T_COMMA, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp2 = VF_Expression(tokenizer);
        if (!exp2) {
            return nullptr;
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }

        /** The expressions must be vectors and of the same type */
        if ((exp1->m_expType != exp2->m_expType) || (exp1->m_expType == vf::Type_Float)) {
            return (Err_Message(tokenizer.Position(), "DOT, operands must be of the same type."), nullptr);
        }

        return std::make_shared<vf::Node_DotProduct>(exp1, exp2);   
    }

    /**
     * Vector cross product, cross(v, u)
     */
    shared_ptr<Node_CrossProduct> Parser::VF_CrossProduct(vf::Tokenizer & tokenizer)
    {
        /** dot product, dot(u, v) */
        if (!Expect(vf::T_KEYWORD_CROSS, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (!Expect(vf::T_COMMA, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp2 = VF_Expression(tokenizer);
        if (!exp2) {
            return nullptr;
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }

        if ((exp1->m_expType != vf::Type_Vec3) || (exp2->m_expType != vf::Type_Vec3)) {
            return (Err_Message(tokenizer.Position(), "CROSS, operands must be vec3."), nullptr);
        }

        return make_shared<Node_CrossProduct>(exp1, exp2);
    }

    /**
     * Vector length, length( u )
     */
    shared_ptr<Node_Length> Parser::VF_Length(vf::Tokenizer & tokenizer)
    {
        /** dot product, dot(u, v) */
        if (!Expect(vf::T_KEYWORD_LENGTH, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp = VF_Expression(tokenizer);
        if (!exp) {
            return nullptr;
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }

        if ((exp->m_expType != vf::Type_Vec2) &&
            (exp->m_expType != vf::Type_Vec3) &&
            (exp->m_expType != vf::Type_Vec4)) 
        {
            /** the length operator can only be applied to vectors */
            return (Err_Message(tokenizer.Position(), "The length operation can only be applied to vectors."), nullptr);
        }

        return make_shared<Node_Length>(exp);
    }

    /**
     * Sine
     */
    shared_ptr<Node_Sine> Parser::VF_Sine(vf::Tokenizer & tokenizer)
    {
        /** dot product, dot(u, v) */
        if (!Expect(vf::T_KEYWORD_SIN, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The sin() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_Sine>(exp1);
    }

    /**
     * ArcSine
     */
    shared_ptr<Node_ArcSine> Parser::VF_ArcSine(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_KEYWORD_ASIN, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The asin() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_ArcSine>(exp1);
    }

    /**
     * Cosine
     */
    shared_ptr<Node_Cosine> Parser::VF_Cosine(vf::Tokenizer & tokenizer)
    {
        /** dot product, dot(u, v) */
        if (!Expect(vf::T_KEYWORD_COS, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The cos() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_Cosine>(exp1);
    }

    /**
     * ArcCosine
     */
    shared_ptr<Node_ArcCosine> Parser::VF_ArcCosine(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_KEYWORD_ACOS, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The acos() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_ArcCosine>(exp1);
    }

    /**
     * Tangent
     */
    shared_ptr<Node_Tangent> Parser::VF_Tangent(vf::Tokenizer & tokenizer)
    {
        /** dot product, dot(u, v) */
        if (!Expect(vf::T_KEYWORD_TAN, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The tan() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_Tangent>(exp1);
    }

    /**
     * ArcCosine
     */
    shared_ptr<Node_ArcTangent> Parser::VF_ArcTangent(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_KEYWORD_ATAN, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The atan() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_ArcTangent>(exp1);
    }

    /**
     * Negation
     */
    shared_ptr<Node_Negate> Parser::VF_Negate(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_SUB, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp = VF_Expression(tokenizer);
        if (!exp) {
            return nullptr;
        }
        return make_shared<Node_Negate>(exp);
    }

    /**
     * Sqrt
     */
    shared_ptr<Node_Sqrt> Parser::VF_Sqrt(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_KEYWORD_SQRT, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The sqrt() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_Sqrt>(exp1);
    }

    /**
     * InvSqrt
     */
    shared_ptr<Node_InvSqrt> Parser::VF_InvSqrt(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_KEYWORD_INVSQRT, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != vf::Type_Float) {
            return (Err_Message(tokenizer.Position(), 
                "The invsqrt() operation can only be applied to scalars"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_InvSqrt>(exp1);
    }

    /**
     * Normalize - Normalize a vector
     */
    shared_ptr<Node_Normalize> Parser::VF_Normalize(vf::Tokenizer & tokenizer)
    {
        if (!Expect(vf::T_KEYWORD_NORMALIZE, tokenizer) || !Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if ((exp1->m_expType != vf::Type_Vec2) && (exp1->m_expType != vf::Type_Vec3) && (exp1->m_expType != vf::Type_Vec4)) {
            return (Err_Message(tokenizer.Position(), 
                "The normalize() operation can only be applied to vectors"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_Normalize>(exp1);
    }

    /**
     * Min/Max
     */
    shared_ptr<Node_MinMax> Parser::VF_MinMax(vf::Tokenizer & tokenizer)
    {
        bool isMin;
        vf::Symbol_t sym = tokenizer.GetSym();
        if ((sym == vf::T_KEYWORD_MIN) || (sym == vf::T_KEYWORD_MAX)) {
            isMin = (sym == vf::T_KEYWORD_MIN);
        } else {
            return nullptr;
        }

        if (!Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (!Expect(vf::T_COMMA, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp2 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (exp1->m_expType != exp2->m_expType) {
            return (Err_Memory("Operands must have the same type"), nullptr);
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_MinMax>(exp1, exp2, isMin);
    }

    /**
     * floor/ceil
     */
    shared_ptr<Node_FloorCeil> Parser::VF_FloorCeil(vf::Tokenizer & tokenizer)
    {
        bool isFloor;
        vf::Symbol_t sym = tokenizer.GetSym();
        if ((sym == vf::T_KEYWORD_FLOOR) || (sym == vf::T_KEYWORD_CEIL)) {
            isFloor = (sym == vf::T_KEYWORD_FLOOR);
        } else {
            return nullptr;
        }

        if (!Expect(vf::T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp1 = VF_Expression(tokenizer);
        if (!exp1) {
            return nullptr;
        }
        if (!Expect(vf::T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_FloorCeil>(exp1, isFloor);
    }

    /**
     * Sampler
     */
    shared_ptr<Node_Sampler> Parser::VF_Sampler(vf::Tokenizer & tokenizer)
    {
        vf::Symbol_t sym = tokenizer.GetSym();

        if (!Expect(T_LEFT_PAREN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Identifier> sampleIdent = VF_Identifier(tokenizer);
        if (!sampleIdent) {
            return nullptr;
        }
        if (sampleIdent->m_expType != vf::Type_Sampler) {
            return (Err_Message(tokenizer.Position(), "Expected a identifier of type 'sampler'."), nullptr);
        }
        if (!Expect(T_COMMA, tokenizer)) {
            return nullptr;
        }
        shared_ptr<vf::Node_Expression> exp = VF_Expression(tokenizer);
        if (!exp) {
            return nullptr;
        }
        if (!Expect(T_RIGHT_PAREN, tokenizer)) {
            return nullptr;
        }

        switch(sym) {
        case vf::T_KEYWORD_SAMPLE1D:    
            if (exp->m_expType != vf::Type_Float) 
                return (Err_Message(tokenizer.Position(), "Expected a expression of type 'float'"), nullptr);
            break;
        case vf::T_KEYWORD_SAMPLE2D:
            if (exp->m_expType != vf::Type_Vec2) 
                return (Err_Message(tokenizer.Position(), "Expected a expression of type 'vec2"), nullptr);
            break;
        case vf::T_KEYWORD_SAMPLE3D:
            if (exp->m_expType != vf::Type_Vec3) 
                return (Err_Message(tokenizer.Position(), "Expected a expression of type 'vec3'"), nullptr);
            break;
        default:
            return nullptr;
        }

        return make_shared<Node_Sampler>(sampleIdent, exp);
    }


    /*************************************************************************/
    /*                              Statements                               */
    /*************************************************************************/

    /**
     * Parses a statement. A statement can either be a assignment or a variable declaration.
     */
    shared_ptr<Node_Statement> Parser::VF_Statement(vf::Tokenizer & tokenizer)
    {
        vf::Symbol_t sym = tokenizer.Peek();
        if (sym == vf::T_IDENT) {
            /** assignment */
            return VF_Assignment(tokenizer);
        } else {
            /** variable declaration */
            return VF_VariableDecl(tokenizer);
        }

    }

    /**
     * Parses a assignment.
     * 
     * \param       tokenizer       Stream with tokens to be parsed.
     * \return      The parsed assignment statement, or nullptr on error.
     */
    shared_ptr<Node_Assignment> Parser::VF_Assignment(vf::Tokenizer & tokenizer)
    {
        shared_ptr<Node_Identifier> ident = VF_Identifier(tokenizer);
        if (!Expect(vf::T_ASSIGN, tokenizer)) {
            return nullptr;
        }
        shared_ptr<Node_Expression> exp = VF_Expression(tokenizer);
        if (!exp) {
            return nullptr;
        }
        if (ident->m_expType != exp->m_expType) {
            return (Err_Message(tokenizer.Position(), "Assign, type mismatch."), nullptr);
        }

        if (!Expect(vf::T_SEMICOLON, tokenizer)) {
            return nullptr;
        }
        return make_shared<Node_Assignment>(ident, exp);
    }
}