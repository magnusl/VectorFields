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
/*                                  Conditional                              */
/*****************************************************************************/
TEST(Simple, Conditional_Greater)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       x;"
        "out float      v;"
        ""
        "void main()"
        "{"
        "   v = x > 1.0 ? 2.0 : x;"
        "}";
    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 4);

    EXPECT_EQ(
        Make_Opcode(vf::OP_CMP_GRT_RC) | Make_FirstOperand(Make_Register(0,0)) | Make_SecondOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_EQ(
        Make_Opcode(vf::OP_COND_SCALAR_CR) | Make_Destination(Make_Register(1, 0)) | Make_FirstOperand(0xff) | 
        Make_SecondOperand(Make_Register(0,0)),
        code[2]
    );
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[3]));
}

TEST(Simple, Conditional_Less)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       x;"
        "out float      v;"
        ""
        "void main()"
        "{"
        "   v = x < 1.0 ? 2.0 : x;"
        "}";
    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 4);

    EXPECT_EQ(
        Make_Opcode(vf::OP_CMP_LE_RC) | Make_FirstOperand(Make_Register(0,0)) | Make_SecondOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_EQ(
        Make_Opcode(vf::OP_COND_SCALAR_CR) | Make_Destination(Make_Register(1, 0)) | Make_FirstOperand(0xff) | 
        Make_SecondOperand(Make_Register(0,0)),
        code[2]
    );
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[3]));
}

TEST(Simple, Conditional_GreaterEqual)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       x;"
        "out float      v;"
        ""
        "void main()"
        "{"
        "   v = x >= 1.0 ? 2.0 : x;"
        "}";
    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 4);

    EXPECT_EQ(
        Make_Opcode(vf::OP_CMP_GEQ_RC) | Make_FirstOperand(Make_Register(0,0)) | Make_SecondOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_EQ(
        Make_Opcode(vf::OP_COND_SCALAR_CR) | Make_Destination(Make_Register(1, 0)) | Make_FirstOperand(0xff) | 
        Make_SecondOperand(Make_Register(0,0)),
        code[2]
    );
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[3]));
}

TEST(Simple, Conditional_LessEqual)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float       x;"
        "out float      v;"
        ""
        "void main()"
        "{"
        "   v = x <= 1.0 ? 2.0 : x;"
        "}";
    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 4);

    EXPECT_EQ(
        Make_Opcode(vf::OP_CMP_LEQ_RC) | Make_FirstOperand(Make_Register(0,0)) | Make_SecondOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_EQ(
        Make_Opcode(vf::OP_COND_SCALAR_CR) | Make_Destination(Make_Register(1, 0)) | Make_FirstOperand(0xff) | 
        Make_SecondOperand(Make_Register(0,0)),
        code[2]
    );
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[3]));
}

/*****************************************************************************/
/*                                      Execution                            */
/*****************************************************************************/

class Conditional : public ::testing::Test 
{
protected:

    Conditional() : m_Stream(m_instructions)
    {
    }

    virtual void SetUp() 
    {
    }

protected:
    std::vector<uint32_t>                   m_instructions;
    vf::InstructionStream                   m_Stream;
};
