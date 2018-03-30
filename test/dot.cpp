#include <intermediate.hpp>
#include <gtest\gtest.h>
#include <memory>

using namespace vf;

/*****************************************************************************/
/*                                      Utility                              */
/*****************************************************************************/

static std::shared_ptr<vf::ByteCode> Compile(const char * pSource)
{
    std::shared_ptr<vf::Program>    pProgram;
    EXPECT_NO_THROW(pProgram = std::make_shared<vf::Program>(pSource));
    return pProgram->Compile();
}

static bool GetMethodSource(std::shared_ptr<vf::ByteCode> bc, std::vector<uint32_t> & code)
{
    const std::vector<std::shared_ptr<vf::ByteCode_Method> > & methods = bc->GetMethods();
    if (methods.empty())
        return false;

    const std::shared_ptr<vf::ByteCode_Method> method = methods[0];
    code = method->GetCode();
    return true;
}

/*****************************************************************************/
/*                              Dot product - Vector 2                       */
/*****************************************************************************/

TEST(Program_Dotproduct, Vector2_RegReg)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2            src1;"
        "in vec2            src2;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(src1, src2);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR2_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

TEST(Program_Dotproduct, Vector2_RegUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2            src1;"
        "uniform vec2       u;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(src1, u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR2_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program_Dotproduct, Vector2_UniformUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec2       u1;"
        "uniform vec2       u2;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(u1, u2);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR2_CC)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                          Dot product - Vector 3                           */
/*****************************************************************************/

TEST(Program_Dotproduct, Vector3_RegReg)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3            src1;"
        "in vec3            src2;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(src1, src2);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR3_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

TEST(Program_Dotproduct, Vector3_RegUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3            src1;"
        "uniform vec3       u;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(src1, u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR3_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program_Dotproduct, Vector3_UniformUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec3       u1;"
        "uniform vec3       u2;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(u1, u2);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR3_CC)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                              Dot product - Vector 4                       */
/*****************************************************************************/

TEST(Program_Dotproduct, Vector4_RegReg)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "in vec4            src2;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(src1, src2);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR4_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

TEST(Program_Dotproduct, Vector4_RegUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "uniform vec4       u;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(src1, u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR4_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program_Dotproduct, Vector4_UniformUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec4       u1;"
        "uniform vec4       u2;"
        "out float          result;"
        "void main()"
        "{"
        "   result = dot(u1, u2);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_DOT_VECTOR4_CC)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}