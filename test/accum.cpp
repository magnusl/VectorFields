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
/*                                      Accumulate                           */
/*****************************************************************************/
TEST(Simple, Accumulate_Vector4Constant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "const vec4             c = {1.0, 2.0, 3.0, 4.0};"
        "out accumulate vec4    v;"
        ""
        "void main()"
        "{"
        "   v = c;"
        "}";
    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);

    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 5);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_ADD_RC)|Make_Destination(Make_Register(0,0))|Make_FirstOperand(Make_Register(0,0)) |
        Make_SecondOperand(0xff),
        code[0]
    );
    EXPECT_FLOAT_EQ(1.0, *((float *)&code[1]));
    EXPECT_FLOAT_EQ(2.0, *((float *)&code[2]));
    EXPECT_FLOAT_EQ(3.0, *((float *)&code[3]));
    EXPECT_FLOAT_EQ(4.0, *((float *)&code[4]));
}


TEST(Simple, Accumulate_Vector4Addition)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4                x;"
        "in vec4                y;"
        "out accumulate vec4    v;"
        ""
        "void main()"
        "{"
        "   v = x + y;"
        "}";
    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);

    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_ADD_RR)|Make_Destination(Make_Register(3,0))|Make_FirstOperand(Make_Register(0,0)) |
        Make_SecondOperand(Make_Register(1, 0)),
        code[0]
    );
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(2,0)) |
        Make_SecondOperand(Make_Register(3, 0)),
        code[1]
    );
}

TEST(Simple, Accumulate_Vector4Subtraction)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4                x;"
        "in vec4                y;"
        "out accumulate vec4    v;"
        ""
        "void main()"
        "{"
        "   v = x - y;"
        "}";
    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);

    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SUB_RR)|Make_Destination(Make_Register(3,0))|Make_FirstOperand(Make_Register(0,0)) |
        Make_SecondOperand(Make_Register(1, 0)),
        code[0]
    );
    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(2,0)) |
        Make_SecondOperand(Make_Register(3, 0)),
        code[1]
    );
}