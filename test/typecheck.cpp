#include <gtest/gtest.h>
#include <parser.hpp>

#if  0
inline void declare(const char * name, vf::Parser & parser, vf::Tokenizer & token, vf::DataType type)
{
    auto pos = token.SymbolTable().Insert(name);
    parser.CurrentEnv()->DeclareVariable(pos, type, vf::ATTRIBUTE_LOCAL);
}

TEST(TypeCheck, Addition_Undeclared)
{
    std::stringstream ss("x + y");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    vf::Node_Expression * exp = parser.VF_Expression();
    EXPECT_EQ(nullptr, exp);
}

TEST(TypeCheck, Addition_Vec4)
{
    std::stringstream ss("x + y");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    declare("x", parser, token, vf::Type_Vec4);
    declare("y", parser, token, vf::Type_Vec4);

    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(nullptr, exp);
    EXPECT_EQ(vf::Type_Vec4, exp->m_expType);
}

TEST(TypeCheck, Addition_DifferentTypes)
{
    std::stringstream ss("x + y");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    declare("x", parser, token, vf::Type_Vec3);
    declare("y", parser, token, vf::Type_Vec4);

    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_EQ(nullptr, exp);
}

TEST(TypeCheck, Vec2_x)
{
    std::stringstream ss("var.x");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec2);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec2_y)
{
    std::stringstream ss("var.y");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec2);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec2_xy)
{
    std::stringstream ss("var.xy");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec2);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Vec2);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

/*****************************************************************************/
/*                                  Vector3                                  */
/*****************************************************************************/

/**
 * Tests that it's possible to access the 'x' member of a vec3.
 */
TEST(TypeCheck, Vec3_x)
{
    std::stringstream ss("var.x");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec3);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

/**
 * Tests that it's possible to access the 'y' member of a vec3.
 */
TEST(TypeCheck, Vec3_y)
{
    std::stringstream ss("var.y");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec3);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

/**
 * Tests that it's possible to access the 'z' member of a vec3.
 */
TEST(TypeCheck, Vec3_z)
{
    std::stringstream ss("var.z");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec3);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

/**
 * Tests that it's possible to access the 'xy' member of a vec3.
 */
TEST(TypeCheck, Vec3_xy)
{
    std::stringstream ss("var.xy");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec3);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Vec2);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

/**
 * Tests that it's possible to access the 'xyz' member of a vec3.
 */
TEST(TypeCheck, Vec3_xyz)
{
    std::stringstream ss("var.xyz");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec3);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Vec3);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

/*****************************************************************************/
/*                                  Vector 4                                 */
/*****************************************************************************/

TEST(TypeCheck, Vec4_x)
{
    std::stringstream ss("var.x");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec4);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec4_y)
{
    std::stringstream ss("var.y");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec4);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec4_z)
{
    std::stringstream ss("var.z");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec4);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec4_w)
{
    std::stringstream ss("var.w");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec4);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Float);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec4_xy)
{
    std::stringstream ss("var.xy");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec4);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Vec2);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec4_xyz)
{
    std::stringstream ss("var.xyz");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec4);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Vec3);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

TEST(TypeCheck, Vec4_xyzw)
{
    std::stringstream ss("var.xyzw");
    vf::Tokenizer token(ss);
    vf::Parser parser(token);

    // declare a variable of type 'vec2'
    declare("var", parser, token, vf::Type_Vec4);
    // try to acces the 'x' member of the 'var' variable, which should be of type float.
    vf::Node_Expression * exp = parser.VF_Expression();
    ASSERT_NE(exp, nullptr);
    // 'y' should be of type flpat
    EXPECT_EQ(exp->m_expType, vf::Type_Vec4);
    ASSERT_EQ(exp->Type(), vf::Node_Expression::EXP_MEMBER_REFERENCE);
    vf::Node_MemberReference * ref = reinterpret_cast<vf::Node_MemberReference*>(exp);
}

#endif