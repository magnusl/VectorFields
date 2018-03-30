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
/*                                      Tests                                */
/*****************************************************************************/

/**
 * Test:    Scalar addition between two scalar registers.
 */
TEST(ScalarArithmetic, SAddition_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float src1;"
        "in float src2;"
        "out float dst;"
        "void main()"
        "{"
        "   dst = src1 + src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/**
 * Test:    Scalar subtraction between two scalar registers.
 */
TEST(ScalarArithmetic, Subtraction_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float src1;"
        "in float src2;"
        "out float dst;"
        "void main()"
        "{"
        "   dst = src1 - src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_SUB_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}
/*****************************************************************************/
/*                      Scalar operations - Register/Constant                */
/*****************************************************************************/

/**
 * Test:    Scalar addition between a register and constant.
 */
TEST(ScalarArithmetic, Addition_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float src1;"
        "out float dst;"
        "void main()"
        "{"
        "   dst = src1 + 1.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_ADD_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );
    EXPECT_FLOAT_EQ(1.0f,*((float *)&code[1]));
}

/**
 * Test:    Scalar subtraction between a register and constant.
 */
TEST(ScalarArithmetic, Subtraction_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float src1;"
        "out float dst;"
        "void main()"
        "{"
        "   dst = src1 - 1.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_SUB_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );
    EXPECT_FLOAT_EQ(1.0f,*((float *)&code[1]));
}

TEST(ScalarArithmetic, Multiplication_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float src1;"
        "out float dst;"
        "void main()"
        "{"
        "   dst = src1 * 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );
    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

TEST(ScalarArithmetic, Division_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float src1;"
        "out float dst;"
        "void main()"
        "{"
        "   dst = src1 / 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );
    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

/*****************************************************************************/
/*                      Scalar operations - Constant/Register                */
/*****************************************************************************/

/**
 * Test:    Scalar subtraction between a constant and a register.
 */
TEST(ScalarArithmetic, Subtraction_ConstantRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float src1;"
        "out float dst;"
        "void main()"
        "{"
        "   dst = 1.0 - src1;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_SUB_CR)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(0xff)|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
    EXPECT_FLOAT_EQ(1.0f,*((float *)&code[1]));
}

/*****************************************************************************/
/*                      Scalar operations - Register/Uniform                 */
/*****************************************************************************/

/**
 * Test:    Scalar addition between a register and a uniform
 */
TEST(ScalarArithmetic, Addition_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       src1;"
        "uniform float  du;"
        "out float      dst;"
        "void main()"
        "{"
        "   dst = src1 + du;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_ADD_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Test:    Scalar addition between a register and a uniform
 */
TEST(ScalarArithmetic, Subtraction_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       src1;"
        "uniform float  du;"
        "out float      dst;"
        "void main()"
        "{"
        "   dst = src1 - du;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_SUB_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Test:    Scalar multiplication between a register and a uniform
 */
TEST(ScalarArithmetic, Multiplication_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       src1;"
        "uniform float  du;"
        "out float      dst;"
        "void main()"
        "{"
        "   dst = src1 * du;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Test:    Scalar division between a register and a uniform
 */
TEST(ScalarArithmetic, Division_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       src1;"
        "uniform float  du;"
        "out float      dst;"
        "void main()"
        "{"
        "   dst = src1 / du;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}