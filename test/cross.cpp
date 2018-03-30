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
/*                                  Cross product                            */
/*****************************************************************************/
TEST(Program_CrossProduct, Vector3_RR)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3        v;"
        "in vec3        u;"
        "out vec3       r;"
        "void main()"
        "{"
        "   r = cross(v, u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_CROSS_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

TEST(Program_CrossProduct, Vector3_RC)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3        v;"
        "uniform vec3   u;"
        "out vec3       r;"
        "void main()"
        "{"
        "   r = cross(v, u);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_CROSS_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program_CrossProduct, Vector3_CR)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3        v;"
        "uniform vec3   u;"
        "out vec3       r;"
        "void main()"
        "{"
        "   r = cross(u, v);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_CROSS_CR)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

TEST(Program_CrossProduct, Vector3_CC)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "uniform vec3   u1;"
        "uniform vec3   u2;"
        "out vec3       r;"
        "void main()"
        "{"
        "   r = cross(u1, u2);"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_CROSS_CC)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}