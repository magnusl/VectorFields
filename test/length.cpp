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
/*                                      Length                               */
/*****************************************************************************/

/**
 * Vector2
 */
TEST(Program, Vector2_Length_Reg)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2        v;"
        "out float      len;"
        "void main()"
        "{"
        "   len = length(v);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_LENGTH_VECTOR2_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program, Vector2_Length_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec2   u;"
        "out float      len;"
        "void main()"
        "{"
        "   len = length(u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_LENGTH_VECTOR2_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Vector3
 */
TEST(Program, Vector3_Length_Reg)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3        v;"
        "out float      len;"
        "void main()"
        "{"
        "   len = length(v);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_LENGTH_VECTOR3_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program, Vector3_Length_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec3   u;"
        "out float      len;"
        "void main()"
        "{"
        "   len = length(u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_LENGTH_VECTOR3_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Vector4
 */
TEST(Program, Vector4_Length_Reg)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4        v;"
        "out float      len;"
        "void main()"
        "{"
        "   len = length(v);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_LENGTH_VECTOR4_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program, Vector4_Length_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec4   u;"
        "out float      len;"
        "void main()"
        "{"
        "   len = length(u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_LENGTH_VECTOR4_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}