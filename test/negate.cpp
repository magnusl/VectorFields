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
/*                                      Negation                             */
/*****************************************************************************/
TEST(Codegen, Negation_Scalar_Register)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       u;"
        "out float      v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_NEGATE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, Negation_Vector2_Register)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2        u;"
        "out vec2       v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_NEGATE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, Negation_Vector3_Register)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3        u;"
        "out vec3       v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_NEGATE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, Negation_Vector4_Register)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4        u;"
        "out vec4       v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_NEGATE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/** 
 *
 */
TEST(Codegen, Negation_Scalar_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float      u;"
        "out float          v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_NEGATE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, Negation_Vector2_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec2   u;"
        "out vec2       v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_NEGATE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, Negation_Vector3_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec3   u;"
        "out vec3       v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_NEGATE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, Negation_Vector4_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec4   u;"
        "out vec4       v;"
        "void main()"
        "{"
        "   v = -u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_NEGATE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}