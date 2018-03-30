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
/*                                      sqrt(x)                              */
/*****************************************************************************/
TEST(Codegen, Sqrt_ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = sqrt(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SQRT_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, Sqrt_ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = sqrt(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SQRT_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                                  invsqrt(x)                               */
/*****************************************************************************/
TEST(Codegen, InvSqrt_ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = invsqrt(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_INVSQRT_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Codegen, InvSqrt_ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = invsqrt(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_INVSQRT_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}
/*****************************************************************************/
/*                                      Execution                            */
/*****************************************************************************/

class Sqrt : public ::testing::Test 
{
protected:

    Sqrt() : m_Stream(m_instructions)
    {
    }

    virtual void SetUp() 
    {
//      m_bc = std::make_shared<vf::ByteCode>(2);
//      m_exec = std::make_shared<vf::ByteCode_Execution>(m_bc, nullptr, 0);
    }

protected:
    std::vector<vf::Vector4>                m_Src1;
    std::vector<vf::Vector4>                m_Src2;
    std::vector<vf::Vector4>                m_Dst;
    std::shared_ptr<vf::ByteCode>           m_bc;
    std::shared_ptr<vf::ByteCode_Execution> m_exec;
    std::vector<uint32_t>                   m_instructions;
    vf::InstructionStream                   m_Stream;
};

/**
 * Tests sqrt() - square root.
 */
TEST_F(Sqrt, Err_Scalar)
{
    vf::Vector4 values[100], expected[100], result[100];
    for(size_t i = 0; i < 100; i++) {
        values[i].x     = float(i);
        expected[i].x   = sqrtf(float(i));
    }

    memset(result, 0, sizeof(result));

    const char * pSource =
        "in float   value;"
        "out float  root;"
        ""
        "void main()"
        "{"
        "   root = sqrt(value);"
        "}";

    std::shared_ptr<vf::ByteCode> bc = Compile(pSource);
    ASSERT_NE(bc, nullptr);

    uint8_t mem[2048];
    vf::ByteCode_Execution be(bc, mem, sizeof(mem));
    be.SetRegisterPointer(0, (float *)values);
    be.SetRegisterPointer(1, (float *)result);

    ASSERT_EQ(be.Execute(0, 100), vf::Err_Success);

    for(size_t i = 0; i < 100; ++i) {
        EXPECT_FLOAT_EQ(expected[i].x, result[i].x);
    }
}

/**
 * Tests invsqrt(), inverse square root
 */
TEST_F(Sqrt, Err_InvScalar)
{
    vf::Vector4 values[100], expected[100], result[100];
    for(size_t i = 0; i < 100; i++) {
        values[i].x     = float(i+1);
        expected[i].x   = 1.0f / sqrtf(float(i+1));
    }

    memset(result, 0, sizeof(result));

    const char * pSource =
        "in float   value;"
        "out float  root;"
        ""
        "void main()"
        "{"
        "   root = invsqrt(value);"
        "}";

    std::shared_ptr<vf::ByteCode> bc = Compile(pSource);
    ASSERT_NE(bc, nullptr);

    uint8_t mem[2048];
    vf::ByteCode_Execution be(bc, mem, sizeof(mem));
    be.SetRegisterPointer(0, (float *)values);
    be.SetRegisterPointer(1, (float *)result);

    ASSERT_EQ(be.Execute(0, 100), vf::Err_Success);

    for(size_t i = 0; i < 100; ++i) {
        EXPECT_FLOAT_EQ(expected[i].x, result[i].x);
    }
}