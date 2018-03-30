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

/*****************************************************************************/
/*                                      sin(x)                               */
/*****************************************************************************/
TEST(Sine, ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = sin(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Sine, ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = sin(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SINE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Sine, Vector2RegisterMemberX)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2        v;"
        "out float      result;"
        "void main()"
        "{"
        "   result = sin(v.x);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Sine, Vector2RegisterMemberY)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2        v;"
        "out float      result;"
        "void main()"
        "{"
        "   result = sin(v.y);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,1)),
        code[0]
        );
}

/*****************************************************************************/
/*                                      cos(x)                               */
/*****************************************************************************/
TEST(Cosine, ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = cos(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_COSINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Cosine, ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = cos(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_COSINE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Cosine, Vector2RegisterMemberX)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2        v;"
        "out float      result;"
        "void main()"
        "{"
        "   result = cos(v.x);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_COSINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Cosine, Vector2RegisterMemberY)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2        v;"
        "out float      result;"
        "void main()"
        "{"
        "   result = cos(v.y);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_COSINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,1)),
        code[0]
        );
}

/*****************************************************************************/
/*                                      tan(x)                               */
/*****************************************************************************/
TEST(Tangent, ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = tan(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_TANGENT_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Tangent, ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = tan(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_TANGENT_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                                      asin(x)                              */
/*****************************************************************************/
TEST(ArcSine, ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = asin(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ARCSINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(ArcSine, ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = asin(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ARCSINE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                                  acos(x)                                  */
/*****************************************************************************/
TEST(ArcCosine, ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = acos(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ARCCOSINE_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(ArcCosine, ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = acos(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ARCCOSINE_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                                      atan(x)                              */
/*****************************************************************************/
TEST(ArcTangent, ScalarRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = atan(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ARCTANGENT_R)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(ArcTangent, ScalarUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform float  f;"
        "out float      result;"
        "void main()"
        "{"
        "   result = atan(f);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_ARCTANGENT_C)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                                      Execution                            */
/*****************************************************************************/

class Trig : public ::testing::Test 
{
protected:

    Trig() : m_Stream(m_instructions)
    {
    }

    virtual void SetUp() 
    {
        //m_bc = std::make_shared<vf::ByteCode>(2);
        //m_exec = std::make_shared<vf::ByteCode_Execution>(m_bc, nullptr, 0);
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
 * Tests sin()
 */
TEST_F(Trig, Err_Scalar_Sine)
{
    vf::Vector4 values[100], expected[100], result[100];
    for(size_t i = 0; i < 100; i++) {
        values[i].x     = float(i);
        expected[i].x   = sinf(float(i));
    }

    memset(result, 0, sizeof(result));

    const char * pSource =
        "in float   value;"
        "out float  result;"
        ""
        "void main()"
        "{"
        "   result = sin(value);"
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
 * Tests cos()
 */
TEST_F(Trig, Err_Scalar_Cosine)
{
    vf::Vector4 values[100], expected[100], result[100];
    for(size_t i = 0; i < 100; i++) {
        values[i].x     = float(i);
        expected[i].x   = cosf(float(i));
    }

    memset(result, 0, sizeof(result));

    const char * pSource =
        "in float   value;"
        "out float  result;"
        ""
        "void main()"
        "{"
        "   result = cos(value);"
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
 * Tests tan()
 */
TEST_F(Trig, Err_Scalar_Tangent)
{
    vf::Vector4 values[100], expected[100], result[100];
    for(size_t i = 0; i < 100; i++) {
        values[i].x     = float(i);
        expected[i].x   = tanf(float(i));
    }

    memset(result, 0, sizeof(result));

    const char * pSource =
        "in float   value;"
        "out float  result;"
        ""
        "void main()"
        "{"
        "   result = tan(value);"
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