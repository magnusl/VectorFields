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
/*                      Vector2 operations - Register/Register               */
/*****************************************************************************/
/**
 * Test:    Vector addition between a constant and a register.
 */
TEST(Program_Vector2_Arithmetic, Addition_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2    src1;"
        "in vec2    src2;"
        "out vec2   dst;"
        "void main()"
        "{"
        "   dst = src1 + src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/**
 * Test:    Vector addition between a constant and a register.
 */
TEST(Program_Vector2_Arithmetic, Subtraction_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2    src1;"
        "in vec2    src2;"
        "out vec2   dst;"
        "void main()"
        "{"
        "   dst = src1 - src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_SUB_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/**
 * Test:    Vector multiplication between a register and constant
 */
TEST(Program_Vector2_Arithmetic, Multiplication_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2    src1;"
        "out vec2   dst;"
        "void main()"
        "{"
        "   dst = src1 * 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );

    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

/**
 * Test:    Vector multiplication between a register and constant
 */
TEST(Program_Vector2_Arithmetic, Division_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2    src1;"
        "out vec2   dst;"
        "void main()"
        "{"
        "   dst = src1 / 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );

    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

/**
 * Test:    Vector multiplication between a register and uniform
 */
TEST(Program_Vector2_Arithmetic, Multiplication_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2            src1;"
        "uniform float      dt;"
        "out vec2           dst;"
        "void main()"
        "{"
        "   dst = src1 * dt;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Test:    Vector multiplication between a register and a uniform
 */
TEST(Program_Vector2_Arithmetic, Division_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec2            src1;"
        "uniform float      dt;"
        "out vec2           dst;"
        "void main()"
        "{"
        "   dst = src1 / dt;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR2_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                      Vector3 operations - Register/Register               */
/*****************************************************************************/
/**
 * Test:    Vector addition between a constant and a register.
 */
TEST(Program_Vector3_Arithmetic, Addition_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3    src1;"
        "in vec3    src2;"
        "out vec3   dst;"
        "void main()"
        "{"
        "   dst = src1 + src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/**
 * Test:    Vector addition between a constant and a register.
 */
TEST(Program_Vector3_Arithmetic, Subtraction_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3    src1;"
        "in vec3    src2;"
        "out vec3   dst;"
        "void main()"
        "{"
        "   dst = src1 - src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_SUB_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/**
 * Test:    Vector multiplication between a register and constant
 */
TEST(Program_Vector3_Arithmetic, Multiplication_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3    src1;"
        "out vec3   dst;"
        "void main()"
        "{"
        "   dst = src1 * 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );

    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

/**
 * Test:    Vector multiplication between a register and constant
 */
TEST(Program_Vector3_Arithmetic, Division_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3    src1;"
        "out vec3   dst;"
        "void main()"
        "{"
        "   dst = src1 / 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );

    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

/**
 * Test:    Vector multiplication between a register and uniform
 */
TEST(Program_Vector3_Arithmetic, Multiplication_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3            src1;"
        "uniform float      dt;"
        "out vec3           dst;"
        "void main()"
        "{"
        "   dst = src1 * dt;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Test:    Vector multiplication between a register and a uniform
 */
TEST(Program_Vector3_Arithmetic, Division_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec3            src1;"
        "uniform float      dt;"
        "out vec3           dst;"
        "void main()"
        "{"
        "   dst = src1 / dt;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR3_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/*****************************************************************************/
/*                      Vector4 operations - Register/Register               */
/*****************************************************************************/
/**
 * Test:    Vector addition between a constant and a register.
 */
TEST(Program_Vector4_Arithmetic, Addition_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4    src1;"
        "in vec4    src2;"
        "out vec4   dst;"
        "void main()"
        "{"
        "   dst = src1 + src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/**
 * Test:    Vector addition between a constant and a register.
 */
TEST(Program_Vector4_Arithmetic, Subtraction_RegisterRegister)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4    src1;"
        "in vec4    src2;"
        "out vec4   dst;"
        "void main()"
        "{"
        "   dst = src1 - src2;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SUB_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,0)),
        code[0]
        );
}

/**
 * Test:    Vector multiplication between a register and constant
 */
TEST(Program_Vector4_Arithmetic, Multiplication_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4    src1;"
        "out vec4   dst;"
        "void main()"
        "{"
        "   dst = src1 * 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );

    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

/**
 * Test:    Vector multiplication between a register and constant
 */
TEST(Program_Vector4_Arithmetic, Division_RegisterConstant)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4    src1;"
        "out vec4   dst;"
        "void main()"
        "{"
        "   dst = src1 / 2.0;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 2);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(0xff),
        code[0]
        );

    EXPECT_FLOAT_EQ(2.0f,*((float *)&code[1]));
}

/**
 * Test:    Vector multiplication between a register and uniform
 */
TEST(Program_Vector4_Arithmetic, Multiplication_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "uniform float      dt;"
        "out vec4           dst;"
        "void main()"
        "{"
        "   dst = src1 * dt;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_MUL_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Test:    Vector multiplication between a register and a uniform
 */
TEST(Program_Vector4_Arithmetic, Division_RegisterUniform)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "uniform float      dt;"
        "out vec4           dst;"
        "void main()"
        "{"
        "   dst = src1 / dt;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_VECTOR4_SCALAR_DIV_RC)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,0)),
        code[0]
        );
}

/**
 * Test:    
 */
TEST(Program_Vector4_Arithmetic, Addition_Reg_XY)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "out float          dst;"
        "void main()"
        "{"
        "   dst = src1.x + src1.y;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_ADD_RR)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(0,1)),
        code[0]
        );
}

/**
 * Test:    
 */
TEST(Program_Vector4_Arithmetic, Addition_Reg_ZW)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "out float          dst;"
        "void main()"
        "{"
        "   dst = src1.z + src1.w;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_ADD_RR)|Make_Destination(Make_Register(1,0))|Make_FirstOperand(Make_Register(0,2))|Make_SecondOperand(Make_Register(0,3)),
        code[0]
        );
}

/**
 * Test:    
 */
TEST(Program_Vector4_Arithmetic, Addition_RegX_RegY)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "in vec4            src2;"
        "out float          dst;"
        "void main()"
        "{"
        "   dst = src1.x + src2.y;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,1)),
        code[0]
        );
}

/**
 * Test:    
 */
TEST(Program_Vector4_Arithmetic, Addition_MixedTypes_Members_XY)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in vec4            src1;"
        "in vec2            src2;"
        "out float          dst;"
        "void main()"
        "{"
        "   dst = src1.x + src2.y;"
        "}";

    auto bytecode = Compile(pSource);
    ASSERT_NE(bytecode, nullptr);
    ASSERT_TRUE(GetMethodSource(bytecode, code));
    ASSERT_EQ(code.size(), 1);

    EXPECT_EQ(
        Make_Opcode(OP_SCALAR_ADD_RR)|Make_Destination(Make_Register(2,0))|Make_FirstOperand(Make_Register(0,0))|Make_SecondOperand(Make_Register(1,1)),
        code[0]
        );
}

/**
 * Test:    
 */
TEST(Program_Vector4_Arithmetic, Addition_Scalar_Vector4_Member)
{
    std::vector<uint32_t> code;
    const char * pSource =
        "in float           scalar;"
        "in vec2            src1;"
        "out float          dst;"
        "void main()"
        "{"
        "   dst = scalar + src1.x;"
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