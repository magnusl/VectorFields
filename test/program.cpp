#include <intermediate.hpp>
#include <gtest\gtest.h>
#include <memory>

using namespace vf;

std::shared_ptr<vf::ByteCode> Compile(const char * pSource)
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

/**
 * Tests compiling a simple program with a single method without any statements.
 */
TEST(Program, Parse_SimpleProgram)
{
    const char * pSource = "void main() {}";
    std::shared_ptr<vf::Program>    pProgram;
    EXPECT_NO_THROW(pProgram = std::make_shared<vf::Program>(pSource));
}

/*****************************************************************************/
/*                                  Mixed types                              */
/*****************************************************************************/

/*****************************************************************************/
/*                                  Simple program                           */
/*****************************************************************************/
TEST(Simple, Test1)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4    x;"     // register 0
        "in vec4    y;"     // register 1
        "in vec4    z;"     // register 2
        "out vec4   w;"     // register 3
        "void main()"
        "{"
        "   w = ((x + y) - z) / 2.0;" 
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 4);

    // t1 (r4) = r0 + r1
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_ADD_RR)|Make_Destination(Make_Register(4,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
    );
    // r4 = r4 - r2
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SUB_RR)|Make_Destination(Make_Register(4,0))|Make_FirstOperand(Make_Register(4,0))|Make_SecondOperand(Make_Register(2,0)),
        code[1]
    );
    // r3 = r4 / 2.0
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_DIV_RC)|Make_Destination(Make_Register(3,0))|Make_FirstOperand(Make_Register(4,0))|Make_SecondOperand(0xff),
        code[2]
    );
    // 2.0
    EXPECT_FLOAT_EQ(2.0f, *((float *)&code[3]));
}

TEST(Simple, Test2_MultipleOutputs)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4    x;"     // register 0
        "in vec4    y;"     // register 1
        "in vec4    z;"     // register 2
        "out vec4   w;"     // register 3
        "out vec4   j;"     // register 4
        "void main()"
        "{"
        "   w = ((x + y) - z) / 2.0;"
        "   j = w * 3.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 6);

    // t1 (r5) = r0 + r1
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_ADD_RR)|Make_Destination(Make_Register(5,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
    );
    // r4 = r5 - r2
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SUB_RR)|Make_Destination(Make_Register(5,0))|Make_FirstOperand(Make_Register(5,0))|Make_SecondOperand(Make_Register(2,0)),
        code[1]
    );
    // r3 = r5 / 2.0
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_DIV_RC)|Make_Destination(Make_Register(3,0))|Make_FirstOperand(Make_Register(5,0))|Make_SecondOperand(0xff),
        code[2]
    );
    // 2.0
    EXPECT_FLOAT_EQ(2.0f, *((float *)&code[3]));

    // r4 = r3 * 2.0
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_MUL_RC)|Make_Destination(Make_Register(4,0))|Make_FirstOperand(Make_Register(3,0))|Make_SecondOperand(0xff),
        code[4]
    );
    // 3.0
    EXPECT_FLOAT_EQ(3.0f, *((float *)&code[5]));
}

TEST(Simple, Test2_Normalize)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4    x;"     // register 0
        "out vec4   y;"     // register 4
        "void main()"
        "{"
        "   float len = length(x);"
        "   y = x / len;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    // r2 = length(r0)
    EXPECT_EQ(Make_Opcode(OP_LENGTH_VECTOR4_R)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0)),code[0]);
    // r1 = r0 / r2.x
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_DIV_RR)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|
        Make_SecondOperand(Make_Register(2,0)),
        code[1]);
}