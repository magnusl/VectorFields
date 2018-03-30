#include <gtest/gtest.h>
#include <token.hpp>
#include <sstream>

/*****************************************************************************/
/*                          Single character tokens                          */
/*****************************************************************************/
TEST(Tokenizer, Token_Semicolon)
{
    std::stringstream ss(";");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_SEMICOLON, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Colon)
{
    std::stringstream ss(":");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_COLON, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Dot)
{
    std::stringstream ss(".");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_DOT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Comma)
{
    std::stringstream ss(",");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_COMMA, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_LeftParen)
{
    std::stringstream ss("(");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_LEFT_PAREN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_RightParen)
{
    std::stringstream ss(")");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_RIGHT_PAREN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_LeftCurlyBracket)
{
    std::stringstream ss("{");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_LEFT_CURLY_BRACKET, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_RightCurlyBracket)
{
    std::stringstream ss("}");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_RIGHT_CURLY_BRACKET, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_LeftSquareBracket)
{
    std::stringstream ss("[");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_LEFT_SQUARE_BRACKET, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_RightSquareBracket)
{
    std::stringstream ss("]");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_RIGHT_SQUARE_BRACKET, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

/*****************************************************************************/
/*                                  Operators                                */
/*****************************************************************************/
TEST(Tokenizer, Token_Add)
{
    std::stringstream ss("+");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_ADD, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Sub)
{
    std::stringstream ss("-");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_SUB, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Mul)
{
    std::stringstream ss("*");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_MUL, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Div)
{
    std::stringstream ss("/");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_DIV, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

/*****************************************************************************/
/*                                  Keywords                                 */
/*****************************************************************************/
TEST(Tokenizer, Token_In)
{
    std::stringstream ss("in");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_IN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Out)
{
    std::stringstream ss("out");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_OUT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Inout)
{
    std::stringstream ss("inout");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_INOUT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Uniform)
{
    std::stringstream ss("uniform");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_UNIFORM, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Const)
{
    std::stringstream ss("const");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_CONST, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Accumulate)
{
    std::stringstream ss("accumulate");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_ACCUM, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Sine)
{
    std::stringstream ss("sin");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_SIN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Cosine)
{
    std::stringstream ss("cos");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_COS, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Tangent)
{
    std::stringstream ss("tan");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_TAN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_ArcSine)
{
    std::stringstream ss("asin");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_ASIN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_ArcCosine)
{
    std::stringstream ss("acos");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_ACOS, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_ArcTangent)
{
    std::stringstream ss("atan");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_ATAN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Sqrt)
{
    std::stringstream ss("sqrt");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_SQRT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_InvSqrt)
{
    std::stringstream ss("invsqrt");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_INVSQRT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Normalize)
{
    std::stringstream ss("normalize");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_NORMALIZE, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Min)
{
    std::stringstream ss("min");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_MIN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Max)
{
    std::stringstream ss("max");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_MAX, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Floor)
{
    std::stringstream ss("floor");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_FLOOR, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Ceil)
{
    std::stringstream ss("ceil");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_CEIL, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Sampler)
{
    std::stringstream ss("sampler");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_SAMPLER, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Sample1D)
{
    std::stringstream ss("sample1D");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_SAMPLE1D, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Sample2D)
{
    std::stringstream ss("sample2D");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_SAMPLE2D, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Sample3D)
{
    std::stringstream ss("sample3D");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_SAMPLE3D, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}



/*****************************************************************************/
/*                                      Types                                */
/*****************************************************************************/
TEST(Tokenizer, Token_Void)
{
    std::stringstream ss("void");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_TYPE_VOID, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Float)
{
    std::stringstream ss("float");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_TYPE_FLOAT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Vec2)
{
    std::stringstream ss("vec2");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_TYPE_VEC2, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Vec3)
{
    std::stringstream ss("vec3");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_TYPE_VEC3, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Vec4)
{
    std::stringstream ss("vec4");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_TYPE_VEC4, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

/*****************************************************************************/
/*                                      Identifier                           */
/*****************************************************************************/
TEST(Tokenizer, Token_SingleCharacterIdent)
{
    std::stringstream ss("x");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Identifier_xy)
{
    std::stringstream ss("xy");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_Identifier_with_underscore)
{
    std::stringstream ss("user_name");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Token_NumberNotAIdentifier)
{
    std::stringstream ss("1234");
    vf::Tokenizer token(ss);
    EXPECT_NE(vf::T_IDENT, token.GetSym());
}

/*****************************************************************************/
/*                                      Integer                              */
/*****************************************************************************/

TEST(Tokenizer, Integer_SingleDigit)
{
    std::stringstream ss("1");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_INTEGER, token.GetSym());
    EXPECT_EQ(1, token.IntValue());
}

TEST(Tokenizer, Integer_Short)
{
    std::stringstream ss("1234");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_INTEGER, token.GetSym());
    EXPECT_EQ(1234, token.IntValue());
}

TEST(Tokenizer, Integer_Long)
{
    std::stringstream ss("123456789");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_INTEGER, token.GetSym());
    EXPECT_EQ(123456789, token.IntValue());
}

TEST(Tokenizer, Integer_ToLong)
{
    std::stringstream ss("1234567890123456789");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_FAILURE, token.GetSym());
}

/*****************************************************************************/
/*                                      Real                                 */
/*****************************************************************************/
TEST(Tokenizer, Integer_Simple1_NoF)
{
    std::stringstream ss("1.0");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_REAL, token.GetSym());
    EXPECT_EQ(1.0, token.RealValue());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Integer_Simple2_NoF)
{
    std::stringstream ss("1.25");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_REAL, token.GetSym());
    EXPECT_EQ(1.25, token.RealValue());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Integer_Simple3_NoF)
{
    std::stringstream ss("10.0");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_REAL, token.GetSym());
    EXPECT_EQ(10.0, token.RealValue());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, Read_WithF)
{
    std::stringstream ss("1.0f");
    vf::Tokenizer token(ss);
    EXPECT_EQ(vf::T_REAL, token.GetSym());
    EXPECT_EQ(1.0, token.RealValue());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

/*****************************************************************************/
/*                                      Assignments                          */
/*****************************************************************************/
TEST(Tokenizer, SimpleAssignment)
{
    std::stringstream ss("x = y;");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // x 
    EXPECT_EQ(vf::T_ASSIGN, token.GetSym());            // =
    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // y
    EXPECT_EQ(vf::T_SEMICOLON, token.GetSym());     // ;
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, SimpleAssignment_WithNewline)
{
    std::stringstream ss("x =\ny;");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // x 
    EXPECT_EQ(vf::T_ASSIGN, token.GetSym());            // =
    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // y
    EXPECT_EQ(vf::T_SEMICOLON, token.GetSym());     // ;
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

/*****************************************************************************/
/*                                  Expressions                              */
/*****************************************************************************/

TEST(Tokenizer, SimpleAddition)
{
    std::stringstream ss("x + y");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // x 
    EXPECT_EQ(vf::T_ADD, token.GetSym());           // =
    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // y
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, SimpleSubtraction)
{
    std::stringstream ss("x - y");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // x 
    EXPECT_EQ(vf::T_SUB, token.GetSym());           // =
    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // y
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, SimpleMultiplication)
{
    std::stringstream ss("x * y");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // x 
    EXPECT_EQ(vf::T_MUL, token.GetSym());           // =
    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // y
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, SimpleExpression_WithParenthesis)
{
    std::stringstream ss("(x + y)");
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_LEFT_PAREN, token.GetSym());
    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // x 
    EXPECT_EQ(vf::T_ADD, token.GetSym());           // =
    EXPECT_EQ(vf::T_IDENT, token.GetSym());         // y
    EXPECT_EQ(vf::T_RIGHT_PAREN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

/*****************************************************************************/
/*                                  Declarations                             */
/*****************************************************************************/
TEST(Tokenizer, InputVariableDeclaration)
{
    const char * code = "in vec4 position;";  

    std::stringstream ss(code);
    vf::Tokenizer token(ss);

    /** in vec4 position; */
    EXPECT_EQ(vf::T_KEYWORD_IN, token.GetSym());
    EXPECT_EQ(vf::T_TYPE_VEC4, token.GetSym());
    EXPECT_EQ(vf::T_IDENT, token.GetSym());
    EXPECT_EQ(vf::T_SEMICOLON, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, OutputVariableDeclaration)
{
    const char * code = "out vec4 position;";  

    std::stringstream ss(code);
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_KEYWORD_OUT, token.GetSym());
    EXPECT_EQ(vf::T_TYPE_VEC4, token.GetSym());
    EXPECT_EQ(vf::T_IDENT, token.GetSym());
    EXPECT_EQ(vf::T_SEMICOLON, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}

TEST(Tokenizer, FunctionDeclaration_VoidNoArguments)
{
    const char * code = "void main()";
    std::stringstream ss(code);
    vf::Tokenizer token(ss);

    EXPECT_EQ(vf::T_TYPE_VOID, token.GetSym());
    EXPECT_EQ(vf::T_IDENT, token.GetSym());
    EXPECT_EQ(vf::T_LEFT_PAREN, token.GetSym());
    EXPECT_EQ(vf::T_RIGHT_PAREN, token.GetSym());
    EXPECT_EQ(vf::T_EOF, token.GetSym());
}
