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
/*                                  Assignment                               */
/*****************************************************************************/

/**
 * Assigns a output vector2 the value of a input variable.
 */
TEST(Codegen, Assignment_Vector2_Register)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2        u;"
        "out vec2       v;"
        "void main()"
        "{"
        "   v = u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR2_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Assigns a output vector2 the value of a uniform variable.
 */
TEST(Codegen, Assignment_Vector2_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec2   u;"
        "out vec2       v;"
        "void main()"
        "{"
        "   v = u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR2_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Assigns a output vector3 the value of a input variable.
 */
TEST(Codegen, Assignment_Vector3_Register)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3        u;"
        "out vec3       v;"
        "void main()"
        "{"
        "   v = u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR3_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Assigns a output vector3 the value of a uniform variable.
 */
TEST(Codegen, Assignment_Vector3_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec3   u;"
        "out vec3       v;"
        "void main()"
        "{"
        "   v = u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR3_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Assigns a output vector4 the value of a input variable.
 */
TEST(Codegen, Assignment_Vector4_Register)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4        u;"
        "out vec4       v;"
        "void main()"
        "{"
        "   v = u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR4_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Assigns a output vector4 the value of a uniform variable.
 */
TEST(Codegen, Assignment_Vector4_Uniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec4   u;"
        "out vec4       v;"
        "void main()"
        "{"
        "   v = u;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR4_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Simple, Assign_Vector2Constant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "const vec2 c = {1.0, 2.0};"
        "out vec2   y;"     // register 4
        "void main()"
        "{"
        "   y = c;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 3);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR2_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[2]));
}

TEST(Simple, Assign_Vector3Constant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "const vec3 c = {1.0, 2.0, 3.0};"
        "out vec3   y;"
        "void main()"
        "{"
        "   y = c;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 4);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR3_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[2]));
    EXPECT_FLOAT_EQ(3.0, *((float *)&code[3]));
}

TEST(Simple, Assign_Vector4Constant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "const vec4 c = {1.0, 2.0, 3.0, 4.0};"
        "out vec4   y;"
        "void main()"
        "{"
        "   y = c;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 5);

    EXPECT_EQ(
        Make_Opcode(OP_ASSIGN_VECTOR4_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[2]));
    EXPECT_FLOAT_EQ(3.0, *((float *)&code[3]));
    EXPECT_FLOAT_EQ(4.0, *((float *)&code[4]));
}

TEST(Simple, Assign_Vector4Paren)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float a;"
        "in float b;"
        "out float   c;"
        "void main()"
        "{"
        "   c = (a + b) < c ? a : b;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
}

TEST(Simple, Assign_Vector4ParenEntire)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float a;"
        "in float b;"
        "out float   c;"
        "void main()"
        "{"
        "   c = (((a + b) < c) ? a : b);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
}