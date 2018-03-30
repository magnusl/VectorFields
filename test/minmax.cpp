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
/*                                      Execution                            */
/*****************************************************************************/

class MinMax : public ::testing::Test 
{
protected:

    MinMax() : m_Stream(m_instructions)
    {
    }

    virtual void SetUp() 
    {
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

/*****************************************************************************/
/*                                      Scalar                               */
/*****************************************************************************/

/**
 * Tests min()
 */
TEST_F(MinMax, Min_Scalar)
{
    float a[]       = {1.0, 2.0, 3.0, 4.0, 1.0, 2.0, 3.0, 4.0, 1.0, 2.0};
    float b[]       = {0.5, 2.5, 1.5, 4.5, 0.5, 2.5, 4.0, 5.0, 0.5, 1.5};
    float c_min[]   = {0.5, 2.0, 1.5, 4.0, 0.5, 2.0, 3.0, 4.0, 0.5, 1.5};
    float c_max[]   = {1.0, 2.5, 3.0, 4.5, 1.0, 2.5, 4.0, 5.0, 1.0, 2.0};

    vf::Vector4  src1[10], src2[10], result[10], expected[10];
    for(size_t i = 0; i < 10; i++) {
        src1[i].x       = a[i];
        src2[i].x       = b[i];
        expected[i].x   = c_min[i];
    }

    memset(result, 0, sizeof(result));

    const char * pSource =
        "in float   src1;"
        "in float   src2;"
        "out float  result;"
        ""
        "void main()"
        "{"
        "   result = min(src1, src2);"
        "}";

    std::shared_ptr<vf::ByteCode> bc = Compile(pSource);
    ASSERT_NE(bc, nullptr);

    uint8_t mem[2048];
    vf::ByteCode_Execution be(bc, mem, sizeof(mem));
    be.SetRegisterPointer(0, (float *)src1);
    be.SetRegisterPointer(1, (float *)src2);
    be.SetRegisterPointer(2, (float *)result);

    ASSERT_EQ(be.Execute(0, 10), vf::Err_Success);

    for(size_t i = 0; i < 10; ++i) {
        EXPECT_FLOAT_EQ(expected[i].x, result[i].x);
    }
}

/**
 * Tests max()
 */
TEST_F(MinMax, Max_Scalar)
{
    float a[]       = {1.0, 2.0, 3.0, 4.0, 1.0, 2.0, 3.0, 4.0, 1.0, 2.0};
    float b[]       = {0.5, 2.5, 1.5, 4.5, 0.5, 2.5, 4.0, 5.0, 0.5, 1.5};
    float c_min[]   = {0.5, 2.0, 1.5, 4.0, 0.5, 2.0, 3.0, 4.0, 0.5, 1.5};
    float c_max[]   = {1.0, 2.5, 3.0, 4.5, 1.0, 2.5, 4.0, 5.0, 1.0, 2.0};

    vf::Vector4  src1[10], src2[10], result[10], expected[10];
    for(size_t i = 0; i < 10; i++) {
        src1[i].x       = a[i];
        src2[i].x       = b[i];
        expected[i].x   = c_max[i];
    }

    memset(result, 0, sizeof(result));

    const char * pSource =
        "in float   src1;"
        "in float   src2;"
        "out float  result;"
        ""
        "void main()"
        "{"
        "   result = max(src1, src2);"
        "}";

    std::shared_ptr<vf::ByteCode> bc = Compile(pSource);
    ASSERT_NE(bc, nullptr);

    uint8_t mem[2048];
    vf::ByteCode_Execution be(bc, mem, sizeof(mem));
    be.SetRegisterPointer(0, (float *)src1);
    be.SetRegisterPointer(1, (float *)src2);
    be.SetRegisterPointer(2, (float *)result);

    ASSERT_EQ(be.Execute(0, 10), vf::Err_Success);

    for(size_t i = 0; i < 10; ++i) {
        EXPECT_FLOAT_EQ(expected[i].x, result[i].x);
    }
}

/*****************************************************************************/
/*                                      Vector4                              */
/*****************************************************************************/

/**
 * Tests min()
 */
TEST_F(MinMax, Min_Vector4)
{
    float a[] = {
        1.0, 2.0, 3.0, 4.0,
        0.1, 1.5, 0.6, 5.0,
        0.5, 3.0, 2.0, 1.0,
        1.0, 6.0, 2.0, 0.1,
        8.0, 1.0, 6.0, 0.2
    };

    float b[] = {
        0.5, 2.5, 3.1, 3.9,
        0.1, 1.6, 0.2, 5.1,
        0.5, 3.2, 2.9, 0.2,
        1.1, 6.1, 2.1, 0.0,
        8.2, 0.0, 1.0, 0.1
    };

    float c[] = {
        0.5, 2.0, 3.0, 3.9,
        0.1, 1.5, 0.2, 5.0,
        0.5, 3.0, 2.0, 0.2,
        1.0, 6.0, 2.0, 0.0,
        8.0, 0.0, 1.0, 0.1
    };

    float result[20];

    const char * pSource =
        "in vec4    src1;"
        "in vec4    src2;"
        "out vec4   result;"
        ""
        "void main()"
        "{"
        "   result = min(src1, src2);"
        "}";

    std::shared_ptr<vf::ByteCode> bc = Compile(pSource);
    ASSERT_NE(bc, nullptr);

    uint8_t mem[2048];
    vf::ByteCode_Execution be(bc, mem, sizeof(mem));
    be.SetRegisterPointer(0, (float *)a);
    be.SetRegisterPointer(1, (float *)b);
    be.SetRegisterPointer(2, (float *)result);

    ASSERT_EQ(be.Execute(0, 5), vf::Err_Success);

    for(size_t i = 0; i < 20; ++i) {
        EXPECT_FLOAT_EQ(c[i], result[i]);
    }
}

/**
 * Tests max() on vec4
 */
TEST_F(MinMax, Max_Vector4)
{
    float a[] = {
        1.0, 2.0, 3.0, 4.0,
        0.1, 1.5, 0.6, 5.0,
        0.5, 3.0, 2.0, 1.0,
        1.0, 6.0, 2.0, 0.1,
        8.0, 1.0, 6.0, 0.2
    };

    float b[] = {
        0.5, 2.5, 3.1, 3.9,
        0.1, 1.6, 0.2, 5.1,
        0.5, 3.2, 2.9, 0.2,
        1.1, 6.1, 2.1, 0.0,
        8.2, 0.0, 1.0, 0.1
    };

    float c[] = {
        1.0, 2.5, 3.1, 4.0,
        0.1, 1.6, 0.6, 5.1,
        0.5, 3.2, 2.9, 1.0,
        1.1, 6.1, 2.1, 0.1,
        8.2, 1.0, 6.0, 0.2
    };

    float result[20];

    const char * pSource =
        "in vec4    src1;"
        "in vec4    src2;"
        "out vec4   result;"
        ""
        "void main()"
        "{"
        "   result = max(src1, src2);"
        "}";

    std::shared_ptr<vf::ByteCode> bc = Compile(pSource);
    ASSERT_NE(bc, nullptr);

    uint8_t mem[2048];
    vf::ByteCode_Execution be(bc, mem, sizeof(mem));
    be.SetRegisterPointer(0, (float *)a);
    be.SetRegisterPointer(1, (float *)b);
    be.SetRegisterPointer(2, (float *)result);

    ASSERT_EQ(be.Execute(0, 5), vf::Err_Success);

    for(size_t i = 0; i < 20; ++i) {
        EXPECT_FLOAT_EQ(c[i], result[i]);
    }

}