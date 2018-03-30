#include "vfvm.h"
#include "vfmath.hpp"

namespace vf
{
    VirtualMachine::VirtualMachine(const vfutil::Bitmap & IoMap, uint8_t NumRegisters, uint8_t NumUniforms, uint8_t NumSamplers)
        : m_IoMap(IoMap)
    {
        m_Registers.resize(NumRegisters);
        m_Samplers.resize(NumSamplers);
        m_Uniforms.resize(NumUniforms);

        BuildCallTable();
    }

    /*************************************************************************/
    /*                                  Modifiers                            */
    /*************************************************************************/

    Status_t VirtualMachine::SetRegisterPointer(size_t index, void * ptrMem)
    {
        m_Registers[index] = ptrMem;
        return Err_Success;
    }
    
    Status_t VirtualMachine::SetUniform(size_t index, float value)
    {
        m_Uniforms[index].u.f = value;
        return Err_Success;
    }

    Status_t VirtualMachine::SetUniform(size_t index, const vf::Vector2 & value)
    {
        m_Uniforms[index].u.v2 = value;
        return Err_Success;
    }

    Status_t VirtualMachine::SetUniform(size_t index, const vf::Vector3 & value)
    {
        m_Uniforms[index].u.v3 = value;
        return Err_Success;
    }

    Status_t VirtualMachine::SetUniform(size_t index, const vf::Vector4 & value)
    {
        m_Uniforms[index].u.v4 = value;
        return Err_Success;
    }

    Status_t VirtualMachine::SetSampler(size_t index, vf::ISampler * sampler)
    {
        m_Samplers[index] = sampler;
        return Err_Success;
    }

    /*************************************************************************/
    /*                  Utility methods used during execution                */
    /*************************************************************************/
    Vector * VirtualMachine::Retrive_Register(uint8_t operand, size_t offset)
    {
        uint8_t reg = Register_Index(operand);
        if (reg > m_Registers.size()) {
            throw std::runtime_error("invalid register index.");
        }
        if (!m_Registers[reg]) {
            throw std::runtime_error("register hasn't been initialized.");
        }
        return ((Vector *)m_Registers[reg]) + (m_IoMap.Get(reg) ? offset : 0);
    }

    float & VirtualMachine::Retrive_Element(uint8_t operand, size_t offset)
    {
        uint8_t idx = Register_Member(operand);
        Vector * ptr = Retrive_Register(operand);
        return ptr->operator[](idx);
    }

    Vector * VirtualMachine::Retrive_Uniform(uint8_t operand)
    {
        uint8_t id = Register_Index(operand);
        if (id > m_Registers.size()) {
            throw std::runtime_error("invalid uniform index.");
        }
        return &m_Uniforms[id];
    }

    float & VirtualMachine::Retrive_UniformElement(uint8_t operand)
    {
        uint8_t id = Register_Index(operand), idx = Register_Member(operand);
        if (id > m_Registers.size()) {
            throw std::runtime_error("invalid register index.");
        }
        return m_Uniforms[id].operator[](idx);
    }

    /** Returns the sampler with the specified index */
    ISampler * VirtualMachine::GetSampler(uint8_t operand)
    {
        if (operand > m_Samplers.size()) {
            throw std::runtime_error("Invalid sampler index.");
        }
        if (m_Samplers[operand] == 0) {
            throw std::runtime_error("Uninitialized sampler.");
        }
        return m_Samplers[operand];
    }

    uint8_t * VirtualMachine::GetFlags()
    {
        return m_Flags;
    }
    /**
     * Sets the memory region that should be used to store the per batch flags.
     */ 
    void VirtualMachine::SetFlagPointer(void * ptr)
    {
        m_Flags = (uint8_t *)ptr;
    }
    /*************************************************************************/
    /*                                  Execution                            */
    /*************************************************************************/
    void VirtualMachine::BuildCallTable()
    {
        m_CallTable.resize(OP_MAX);
        for(size_t i = 0; i < OP_MAX; ++i) {
            if ((i >= OP_SCALAR_ADD_RR) && (i <= OP_VECTOR4_ADD_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Add;
            } else if ((i >= OP_SCALAR_SUB_RR) && (i <= OP_VECTOR4_SUB_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Sub;
            } else if ((i >= OP_SCALAR_MUL_RR) && (i <= OP_VECTOR4_SCALAR_MUL_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Mul;
            } else if ((i >= OP_SCALAR_DIV_RR) && (i <= OP_VECTOR4_SCALAR_DIV_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Div;
            } else if ((i >= OP_SCALAR_NEGATE_R) && (i <= OP_VECTOR4_NEGATE_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Negate;
            } else if ((i >= OP_DOT_VECTOR2_RR) && (i <= OP_DOT_VECTOR4_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Dot;
            } else if ((i >= OP_CROSS_RR) && (i <= OP_CROSS_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Cross;
            } else if ((i >= OP_LENGTH_VECTOR2_R) && (i <= OP_LENGTH_VECTOR4_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Length;
            } else if ((i >= OP_SINE_R) && (i <= OP_ARCTANGENT_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Trigonometric;
            } else if ((i >= OP_SQRT_R) && (i <= OP_INVSQRT_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Sqrt;
            } else if ((i >= OP_VECTOR2_NORMALIZE_R) && (i <= OP_VECTOR4_NORMALIZE_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Normalize;
            } else if ((i >= OP_ASSIGN_SCALAR_R) && (i <= OP_ASSIGN_VECTOR4_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Assignment;
            } else if ((i >= OP_CMP_GRT_RR) && (i <= OP_CMP_LEQ_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Comparison;
            } else if ((i >= OP_COND_SCALAR_RR) && (i <= OP_COND_VECTOR4_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_ConditionalAssignment;
            } else if ((i >= OP_MIN_SCALAR_RR) && (i <= OP_MIN_VECTOR4_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Min;
            } else if ((i >= OP_MAX_SCALAR_RR) && (i <= OP_MAX_VECTOR4_CC)) {
                m_CallTable[i] = &VirtualMachine::Execute_Max;
            } else if ((i >= OP_SAMPLE1D_R) && (i <= OP_SAMPLE3D_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Sampler;
            } else if ((i >= OP_FLOOR_SCALAR_R) && (i <= OP_FLOOR_VECTOR4_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Floor;
            } else if ((i >= OP_CEIL_SCALAR_R) && (i <= OP_CEIL_VECTOR4_C)) {
                m_CallTable[i] = &VirtualMachine::Execute_Ceil;
            }
        }
    }

    /**
     * VirtualMachine::Execute
     * Executes the instruction in the stream on the Virtual Machine (VM).
     */
    Status_t VirtualMachine::Execute(vf::InstructionStream & stream, size_t batchSize, size_t batchOffset)
    {
        vf::Instruction_t instr;
        try {
            while(stream.Decode(instr)) {
                if (instr.Opcode < OP_MAX) {
                    VirtualMachine::pInstrImpl_t methodPtr = m_CallTable[instr.Opcode];
                    Status_t err = (this->*methodPtr)(instr, stream, batchSize, batchOffset);
                    if (err != Err_Success) {
                        return err;
                    }
                } else {
                    return Err_InvalidBytecode;
                }
            }
        } catch(std::runtime_error &) {
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /*************************************************************************/
    /*                                  Instructions                         */
    /*************************************************************************/

    /**
    * Execute_Add
    * Executes a addition bytecode instruction.
    */
    Status_t VirtualMachine::Execute_Add(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode)
        {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_SCALAR_ADD_RR:
            {
                float * pCurrentDst         = &Retrive_Element(ins.Dst, batchOffset);
                const float * pCurrentSrc1  = &Retrive_Element(ins.Src1, batchOffset);
                const float * pCurrentSrc2  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = pCurrentSrc1[i] + pCurrentSrc2[i];
                }
                break;
            }
        case OP_SCALAR_ADD_RC:      /** scalar register + scalar constant/uniform */
            {
                float * pCurrentDst = &Retrive_Element(ins.Dst, batchOffset);
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = pLhs[i] + rhs;
                }
                break;
            }
        case OP_SCALAR_ADD_CR:      /** scalar constant/uniform + scalar register */
            {
                float * pCurrentDst = &Retrive_Element(ins.Dst, batchOffset);
                const float * pRhs  = &Retrive_Element(ins.Src2, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = lhs + pRhs[i];
                }
                break;
            }
        case OP_SCALAR_ADD_CC:      /** scalar constant/uniform + scalar constant/uniform */
            {
                float * pCurrentDst = &Retrive_Element(ins.Dst, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                float result        = lhs + rhs;

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = result;
                }
                break;
            }

            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_VECTOR2_ADD_RR:
            {
                Vector * dst = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                Vector * src2 = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v2, (src1++)->u.v2, (src2++)->u.v2);
                }
                break;
            }
        case OP_VECTOR2_ADD_RC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector2 & rhs = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v2, (src1++)->u.v2, rhs);
                }
            }
            break;
        case OP_VECTOR2_ADD_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src2 = Retrive_Register(ins.Src2, batchOffset);
                const Vector2 & lhs = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v2, lhs, (src2++)->u.v2);
                }
            }
            break;
        case OP_VECTOR2_ADD_CC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & lhs = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                const Vector2 & rhs = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;
                Vector2 result      = lhs + rhs;

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v2 = result;
                }
            }
            break;
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_VECTOR3_ADD_RR:
            {
                Vector * dst = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                Vector * src2 = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v3, (src1++)->u.v3, (src2++)->u.v3);
                }
                break;
            }
        case OP_VECTOR3_ADD_RC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector3 & rhs = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v3, (src1++)->u.v3, rhs);
                }
            }
            break;
        case OP_VECTOR3_ADD_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src2 = Retrive_Register(ins.Src2, batchOffset);
                const Vector3 & lhs = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v3, lhs, (src2++)->u.v3);
                }
            }
            break;
        case OP_VECTOR3_ADD_CC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & lhs = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                const Vector3 & rhs = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;
                Vector3 result      = lhs + rhs;

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v3 = result;
                }
            }
            break;
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_VECTOR4_ADD_RR:
            {
                Vector * dst = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                Vector * src2 = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v4, (src1++)->u.v4, (src2++)->u.v4);
                }
                break;
            }
        case OP_VECTOR4_ADD_RC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector4 & rhs = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v4, (src1++)->u.v4, rhs);
                }
            }
            break;
        case OP_VECTOR4_ADD_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src2 = Retrive_Register(ins.Src2, batchOffset);
                const Vector4 & lhs = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Add((dst++)->u.v4, lhs, (src2++)->u.v4);
                }
            }
            break;
        case OP_VECTOR4_ADD_CC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & lhs = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                const Vector4 & rhs = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;
                Vector4 result      = lhs + rhs;

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v4 = result;
                }
            }
            break;
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
    * Execute_Sub
    * Executes a subtraction bytecode instruction.
    */
    Status_t VirtualMachine::Execute_Sub(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode)
        {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_SCALAR_SUB_RR:
            {
                float * pCurrentDst         = &Retrive_Element(ins.Dst, batchOffset);
                const float * pCurrentSrc1  = &Retrive_Element(ins.Src1, batchOffset);
                const float * pCurrentSrc2  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = pCurrentSrc1[i] - pCurrentSrc2[i];
                }
                break;
            }
        case OP_SCALAR_SUB_RC:
            {
                float * pCurrentDst = &Retrive_Element(ins.Dst, batchOffset);
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = pLhs[i] - rhs;
                }
                break;
            }
        case OP_SCALAR_SUB_CR:      /** scalar constant/uniform + scalar register */
            {
                float * pCurrentDst = &Retrive_Element(ins.Dst, batchOffset);
                const float * pRhs  = &Retrive_Element(ins.Src2, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = lhs - pRhs[i];
                }
                break;
            }
        case OP_SCALAR_SUB_CC:      /** scalar constant/uniform + scalar constant/uniform */
            {
                float * pCurrentDst = &Retrive_Element(ins.Dst, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                float result        = lhs - rhs;

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pCurrentDst[i] = result;
                }
                break;
            }

            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_VECTOR2_SUB_RR:
            {
                Vector * dst = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                Vector * src2 = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v2, (src1++)->u.v2, (src2++)->u.v2);
                }
                break;
            }
        case OP_VECTOR2_SUB_RC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector2 & rhs = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v2, (src1++)->u.v2, rhs);
                }
            }
            break;
        case OP_VECTOR2_SUB_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src2 = Retrive_Register(ins.Src2, batchOffset);
                const Vector2 & lhs = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v2, lhs, (src2++)->u.v2);
                }
            }
            break;
        case OP_VECTOR2_SUB_CC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & lhs = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                const Vector2 & rhs = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;
                Vector2 result      = lhs - rhs;

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v2 = result;
                }
            }
            break;
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_VECTOR3_SUB_RR:
            {
                Vector * dst = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                Vector * src2 = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v3, (src1++)->u.v3, (src2++)->u.v3);
                }
                break;
            }
        case OP_VECTOR3_SUB_RC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector3 & rhs = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v3, (src1++)->u.v3, rhs);
                }
            }
            break;
        case OP_VECTOR3_SUB_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src2 = Retrive_Register(ins.Src2, batchOffset);
                const Vector3 & lhs = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v3, lhs, (src2++)->u.v3);
                }
            }
            break;
        case OP_VECTOR3_SUB_CC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & lhs = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                const Vector3 & rhs = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;
                Vector3 result      = lhs - rhs;

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v3 = result;
                }
            }
            break;
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_VECTOR4_SUB_RR:
            {
                Vector * dst = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                Vector * src2 = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v4, (src1++)->u.v4, (src2++)->u.v4);
                }
                break;
            }
        case OP_VECTOR4_SUB_RC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector4 & rhs = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v4, (src1++)->u.v4, rhs);
                }
            }
            break;
        case OP_VECTOR4_SUB_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src2 = Retrive_Register(ins.Src2, batchOffset);
                const Vector4 & lhs = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Sub((dst++)->u.v4, lhs, (src2++)->u.v4);
                }
            }
            break;
        case OP_VECTOR4_SUB_CC:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & lhs = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                const Vector4 & rhs = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;
                Vector4 result      = lhs - rhs;

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v4 = result;
                }
            }
            break;
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
    * Execute_Mul
    * Executes a multiplication bytecode instruction.
    */
    Status_t VirtualMachine::Execute_Mul(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode)
        {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_SCALAR_MUL_RR:
            {
                float * pDst        = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc1 = &Retrive_Element(ins.Src1, batchOffset);
                const float * pSrc2 = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = pSrc1[i] * pSrc2[i];
                }
                break;
            }
        case OP_SCALAR_MUL_RC:      /** scalar register - scalar constant/uniform */
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = pSrc1[i] * rhs;
                }
                break;
            }
        case OP_SCALAR_MUL_CR:      /** scalar constant/uniform - scalar register */
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc2     = &Retrive_Element(ins.Src2, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = lhs * pSrc2[i];
                }
                break;
            }
        case OP_SCALAR_MUL_CC:      /** scalar constant/uniform - scalar constant/uniform */
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                float result            = lhs * rhs;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    pDst[i] = result;
                }
                break;
            }
            /*****************************************************************/
            /*                  Vector2 multiplied with a scalar             */
            /*****************************************************************/
        case OP_VECTOR2_SCALAR_MUL_RR:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float * fValue  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    vf::Mul((dst++)->u.v2, (src1++)->u.v2, fValue[i]);
                }
                break;
            }
        case OP_VECTOR2_SCALAR_MUL_RC:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float rhs       = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    vf::Mul((dst++)->u.v2, (src1++)->u.v2, rhs);
                }
                break;
            }
        case OP_VECTOR2_SCALAR_MUL_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                float * pRhs        = &Retrive_Element(ins.Src2, batchOffset);
                const Vector2 & lhs = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    vf::Mul((dst++)->u.v2, lhs, pRhs[i]);
                }
                break;
            }
        case OP_VECTOR2_SCALAR_MUL_CC:
            {
                Vector * dst         = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & lhs  = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                float rhs            = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                Vector2 result       = lhs * rhs;

                for(size_t i = 0; i < batchSize; i++) {
                    dst->u.v2 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                  Vector3 multiplied with a scalar             */
            /*****************************************************************/
        case OP_VECTOR3_SCALAR_MUL_RR:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float * fValue  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    vf::Mul((dst++)->u.v3, (src1++)->u.v3, fValue[i]);
                }
                break;
            }
        case OP_VECTOR3_SCALAR_MUL_RC:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float rhs       = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    vf::Mul((dst++)->u.v3, (src1++)->u.v3, rhs);
                }
                break;
            }
        case OP_VECTOR3_SCALAR_MUL_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                float * pRhs        = &Retrive_Element(ins.Src2, batchOffset);
                const Vector3 & lhs = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    vf::Mul((dst++)->u.v3, lhs, pRhs[i]);
                }
                break;
            }
        case OP_VECTOR3_SCALAR_MUL_CC:
            {
                Vector * dst         = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & lhs  = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                float rhs            = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                Vector3 result       = lhs * rhs;

                for(size_t i = 0; i < batchSize; i++) {
                    dst->u.v3 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                  Vector4 multiplied with a scalar             */
            /*****************************************************************/
        case OP_VECTOR4_SCALAR_MUL_RR:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float * fValue  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    vf::Mul((dst++)->u.v4, (src1++)->u.v4, fValue[i]);
                }
                break;
            }
        case OP_VECTOR4_SCALAR_MUL_RC:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float rhs       = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    vf::Mul((dst++)->u.v4, (src1++)->u.v4, rhs);
                }
                break;
            }
        case OP_VECTOR4_SCALAR_MUL_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                float * pRhs        = &Retrive_Element(ins.Src2, batchOffset);
                const Vector4 & lhs = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    vf::Mul((dst++)->u.v4, lhs, pRhs[i]);
                }
                break;
            }
        case OP_VECTOR4_SCALAR_MUL_CC:
            {
                Vector * dst         = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & lhs  = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                float rhs            = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                Vector4 result       = lhs * rhs;

                for(size_t i = 0; i < batchSize; i++) {
                    dst->u.v4 = result;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
    * Execute_Div
    * Executes a division bytecode instruction.
    */
    Status_t VirtualMachine::Execute_Div(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode)
        {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_SCALAR_DIV_RR:
            {
                float * pDst        = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc1 = &Retrive_Element(ins.Src1, batchOffset);
                const float * pSrc2 = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = pSrc1[i] / pSrc2[i];
                }
                break;
            }
        case OP_SCALAR_DIV_RC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = pSrc1[i] / rhs;
                }
                break;
            }
        case OP_SCALAR_DIV_CR:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc2     = &Retrive_Element(ins.Src2, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = lhs / pSrc2[i];
                }
                break;
            }
        case OP_SCALAR_DIV_CC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                float result            = lhs / rhs;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    pDst[i] = result;
                }
                break;
            }
            /*****************************************************************/
            /*                  Vector2 divided with a scalar                */
            /*****************************************************************/
        case OP_VECTOR2_SCALAR_DIV_RR:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float * fValue  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    vf::Div((dst++)->u.v2, (src1++)->u.v2, fValue[i]);
                }
                break;
            }
        case OP_VECTOR2_SCALAR_DIV_RC:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float rhs       = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    vf::Div((dst++)->u.v2, (src1++)->u.v2, rhs);
                }
                break;
            }
        case OP_VECTOR2_SCALAR_DIV_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                float * pRhs        = &Retrive_Element(ins.Src2, batchOffset);
                const Vector2 & lhs = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    vf::Div((dst++)->u.v2, lhs, pRhs[i]);
                }
                break;
            }
        case OP_VECTOR2_SCALAR_DIV_CC:
            {
                Vector * dst         = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & lhs  = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                float rhs            = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                Vector2 result       = lhs / rhs;

                for(size_t i = 0; i < batchSize; i++) {
                    dst->u.v2 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                  Vector3 divided with a scalar                */
            /*****************************************************************/
        case OP_VECTOR3_SCALAR_DIV_RR:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float * fValue  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    vf::Div((dst++)->u.v3, (src1++)->u.v3, fValue[i]);
                }
                break;
            }
        case OP_VECTOR3_SCALAR_DIV_RC:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float rhs       = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    vf::Div((dst++)->u.v3, (src1++)->u.v3, rhs);
                }
                break;
            }
        case OP_VECTOR3_SCALAR_DIV_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                float * pRhs        = &Retrive_Element(ins.Src2, batchOffset);
                const Vector3 & lhs = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    vf::Div((dst++)->u.v3, lhs, pRhs[i]);
                }
                break;
            }
        case OP_VECTOR3_SCALAR_DIV_CC:
            {
                Vector * dst         = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & lhs  = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                float rhs            = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                Vector3 result       = lhs / rhs;

                for(size_t i = 0; i < batchSize; i++) {
                    dst->u.v3 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                  Vector4 divided with a scalar                */
            /*****************************************************************/
        case OP_VECTOR4_SCALAR_DIV_RR:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float * fValue  = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    vf::Div((dst++)->u.v4, (src1++)->u.v4, fValue[i]);
                }
                break;
            }
        case OP_VECTOR4_SCALAR_DIV_RC:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector * src1   = Retrive_Register(ins.Src1, batchOffset);
                float rhs       = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    vf::Div((dst++)->u.v4, (src1++)->u.v4, rhs);
                }
                break;
            }
        case OP_VECTOR4_SCALAR_DIV_CR:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                float * pRhs        = &Retrive_Element(ins.Src2, batchOffset);
                const Vector4 & lhs = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    vf::Div((dst++)->u.v4, lhs, pRhs[i]);
                }
                break;
            }
        case OP_VECTOR4_SCALAR_DIV_CC:
            {
                Vector * dst         = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & lhs  = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                float rhs            = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                Vector4 result       = lhs / rhs;

                for(size_t i = 0; i < batchSize; i++) {
                    dst->u.v4 = result;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
    * Execute_Negate
    * Executes a negation bytecode instruction.
    */
    Status_t VirtualMachine::Execute_Negate(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) 
        {
        case OP_SCALAR_NEGATE_R:
            {
                float * pDst        = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc  = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = -pSrc[i];
                }
                break;
            }
        case OP_SCALAR_NEGATE_C:
            {
                float * pDst = &Retrive_Element(ins.Dst, batchOffset);
                float value  = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = -value;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_VECTOR2_NEGATE_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Negate((pDst++)->u.v2, (pSrc++)->u.v2);
                }
                break;
            }
        case OP_VECTOR2_NEGATE_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & w   = Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Negate((pDst++)->u.v2, w);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_VECTOR3_NEGATE_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Negate((pDst++)->u.v3, (pSrc++)->u.v3);
                }
                break;
            }
        case OP_VECTOR3_NEGATE_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & w   = Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Negate((pDst++)->u.v3, w);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_VECTOR4_NEGATE_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Negate((pDst++)->u.v4, (pSrc++)->u.v4);
                }
                break;
            }
        case OP_VECTOR4_NEGATE_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & w   = Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Negate((pDst++)->u.v4, w);
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
    * Execute_Negate
    * Executes a negation bytecode instruction.
    */
    Status_t VirtualMachine::Execute_Trigonometric(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode)
        {
            /*****************************************************************/
            /*                              Sine                             */
            /*****************************************************************/
        case OP_SINE_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i+= 4) {
                    dst[i] = sinf(src[i]);
                }
                break;
            }
        case OP_SINE_C:
            {
                float fValue    = (ins.Src1 == 0xff) ? sinf(is.DecodeScalar()) : sinf(Retrive_UniformElement(ins.Src1));
                float * dst     = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
            /*****************************************************************/
            /*                              Cosine                           */
            /*****************************************************************/
        case OP_COSINE_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);
                
                for(size_t i = 0; i < (batchSize * 4); i+= 4) {
                    dst[i] = cosf(src[i]);
                }
                break;
            }
        case OP_COSINE_C:
            {
                float fValue    = (ins.Src1 == 0xff) ? cosf(is.DecodeScalar()) : cosf(Retrive_UniformElement(ins.Src1));
                float * dst     = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
            /*****************************************************************/
            /*                              Tangent                          */
            /*****************************************************************/
        case OP_TANGENT_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i+= 4) {
                    dst[i] = tanf(src[i]);
                }
                break;
            }
        case OP_TANGENT_C:
            {
                float fValue    = (ins.Src1 == 0xff) ? tanf(is.DecodeScalar()) : tanf(Retrive_UniformElement(ins.Src1));
                float * dst     = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
            /*****************************************************************/
            /*                              ArcSine                          */
            /*****************************************************************/
        case OP_ARCSINE_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i+= 4) {
                    dst[i] = asinf(src[i]);
                }
                break;
            }
        case OP_ARCSINE_C:
            {
                float fValue    = (ins.Src1 == 0xff) ? asinf(is.DecodeScalar()) : asinf(Retrive_UniformElement(ins.Src1));
                float * dst     = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
            /*****************************************************************/
            /*                              ArcCosine                        */
            /*****************************************************************/
        case OP_ARCCOSINE_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i+= 4) {
                    dst[i] = acosf(src[i]);
                }
                break;
            }
        case OP_ARCCOSINE_C:
            {
                float fValue    = (ins.Src1 == 0xff) ? acosf(is.DecodeScalar()) : acosf(Retrive_UniformElement(ins.Src1));
                float * dst     = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
            /*****************************************************************/
            /*                              ArcTangent                       */
            /*****************************************************************/
        case OP_ARCTANGENT_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i+= 4) {
                    dst[i] = atanf(src[i]);
                }
                break;
            }
        case OP_ARCTANGENT_C:
            {
                float fValue    = (ins.Src1 == 0xff) ? atanf(is.DecodeScalar()) : atanf(Retrive_UniformElement(ins.Src1));
                float * dst     = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Execute_Length
     * Executes a length bytecode instruction.
     */
    Status_t VirtualMachine::Execute_Length(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) {
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_LENGTH_VECTOR2_R:
            {
                float * pDst    = &Retrive_Element(ins.Dst, batchOffset);
                Vector * src    = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = (src++)->u.v2.length();
                }
                break;
            }
        case OP_LENGTH_VECTOR2_C:
            {
                float * pDst    = &Retrive_Element(ins.Dst, batchOffset);
                float fLength   = (ins.Src1 == 0xff) ? is.Decode2()->length() : Retrive_Uniform(ins.Src1)->u.v2.length();

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = fLength;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_LENGTH_VECTOR3_R:
            {
                float * pDst    = &Retrive_Element(ins.Dst, batchOffset);
                Vector * src    = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = (src++)->u.v3.length();
                }
                break;
            }
        case OP_LENGTH_VECTOR3_C:
            {
                float * pDst    = &Retrive_Element(ins.Dst, batchOffset);
                float fLength   = (ins.Src1 == 0xff) ? is.Decode3()->length() : Retrive_Uniform(ins.Src1)->u.v3.length();

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = fLength;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_LENGTH_VECTOR4_R:
            {
                float * pDst    = &Retrive_Element(ins.Dst, batchOffset);
                Vector * src    = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = (src++)->u.v3.length();
                }
                break;
            }
        case OP_LENGTH_VECTOR4_C:
            {
                float * pDst    = &Retrive_Element(ins.Dst, batchOffset);
                float fLength   = (ins.Src1 == 0xff) ? is.Decode4()->length() : Retrive_Uniform(ins.Src1)->u.v4.length();

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = fLength;
                }
                break;
            }

        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Execute_Sqrt
     * Executes a sqrt or invsqrt instruction.
     */
    Status_t VirtualMachine::Execute_Sqrt(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) {
            /*****************************************************************/
            /*                              SQRT                             */
            /*****************************************************************/
        case OP_SQRT_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    dst[i] = sqrtf(src[i]);
                }
                break;
            }
        case OP_SQRT_C:
            {
                float fValue = (ins.Src1 == 0xff) ? sqrtf(is.DecodeScalar()) : sqrtf(Retrive_UniformElement(ins.Src1));
                float * dst = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
            /*****************************************************************/
            /*                              INVSQRT                          */
            /*****************************************************************/
        case OP_INVSQRT_R:
            {
                float * dst = &Retrive_Element(ins.Dst, batchOffset);
                float * src = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    dst[i] = 1.0f / sqrtf(src[i]);
                }
                break;
            }
        case OP_INVSQRT_C:
            {
                float fValue = 1.0f / ((ins.Src1 == 0xff) ? sqrtf(is.DecodeScalar()) : sqrtf(Retrive_UniformElement(ins.Src1)));
                float * dst = &Retrive_Element(ins.Dst, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    dst[i] = fValue;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Execute_Normalize
     * Executes a normalize instruction.
     */
    Status_t VirtualMachine::Execute_Normalize(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) {
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_VECTOR2_NORMALIZE_R:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                
                for(size_t i = 0; i < batchSize; i++) {
                    (dst++)->u.v2 = (src1++)->u.v2.normalize();
                }
                break;
            }
        case OP_VECTOR2_NORMALIZE_C:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector2 value   = (ins.Src1 == 0xff) ? is.Decode2()->normalize() : Retrive_Uniform(ins.Src1)->u.v2.normalize();

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v2 = value;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_VECTOR3_NORMALIZE_R:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                
                for(size_t i = 0; i < batchSize; i++) {
                    (dst++)->u.v3 = (src1++)->u.v3.normalize();
                }
                break;
            }
        case OP_VECTOR3_NORMALIZE_C:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector3 value   = (ins.Src1 == 0xff) ? is.Decode3()->normalize() : Retrive_Uniform(ins.Src1)->u.v3.normalize();

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v3 = value;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_VECTOR4_NORMALIZE_R:
            {
                Vector * dst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * src1 = Retrive_Register(ins.Src1, batchOffset);
                
                for(size_t i = 0; i < batchSize; i++) {
                    (dst++)->u.v4 = (src1++)->u.v4.normalize();
                }
                break;
            }
        case OP_VECTOR4_NORMALIZE_C:
            {
                Vector * dst    = Retrive_Register(ins.Dst, batchOffset);
                Vector4 value   = (ins.Src1 == 0xff) ? is.Decode4()->normalize() : Retrive_Uniform(ins.Src1)->u.v4.normalize();

                for(size_t i = 0; i < batchSize; ++i) {
                    (dst++)->u.v4 = value;
                }
                break;
            }

        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Executes a min() instruction.
     */
    Status_t VirtualMachine::Execute_Min(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_MIN_SCALAR_RR:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                const float * fSrc2     = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    fDst[i] = std::min(fSrc1[i], fSrc2[i]);
                }
                break;
            }
        case OP_MIN_SCALAR_RC:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    fDst[i] = std::min(fSrc1[i], rhs);
                }
                break;
            }
        case OP_MIN_SCALAR_CR:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc2     = &Retrive_Element(ins.Src2, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    fDst[i] = std::min(lhs, fSrc2[i]);
                }
                break;
            }
        case OP_MIN_SCALAR_CC:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    fDst[i] = std::min(lhs, rhs);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_MIN_VECTOR2_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v2, pSrc1[i].u.v2, pSrc2[i].u.v2);
                }
                break;
            }
        case OP_MIN_VECTOR2_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v2, pSrc1[i].u.v2, rhs);
                }
                break;
            }
        case OP_MIN_VECTOR2_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v2, lhs, pSrc2[i].u.v2);
                }
                break;
            }
        case OP_MIN_VECTOR2_CC:
            {
                Vector2 result;
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;
                vf::Min(result, lhs, rhs);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v2 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_MIN_VECTOR3_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v3, pSrc1[i].u.v3, pSrc2[i].u.v3);
                }
                break;
            }
        case OP_MIN_VECTOR3_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v3, pSrc1[i].u.v3, rhs);
                }
                break;
            }
        case OP_MIN_VECTOR3_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v3, lhs, pSrc2[i].u.v3);
                }
                break;
            }
        case OP_MIN_VECTOR3_CC:
            {
                Vector3 result;
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;
                vf::Min(result, lhs, rhs);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v3 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_MIN_VECTOR4_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v4, pSrc1[i].u.v4, pSrc2[i].u.v4);
                }
                break;
            }
        case OP_MIN_VECTOR4_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v4, pSrc1[i].u.v4, rhs);
                }
                break;
            }
        case OP_MIN_VECTOR4_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Min(pDst[i].u.v4, lhs, pSrc2[i].u.v4);
                }
                break;
            }
        case OP_MIN_VECTOR4_CC:
            {
                Vector4 result;
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;
                vf::Min(result, lhs, rhs);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v4 = result;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Executes a max() instruction.
     */
    Status_t VirtualMachine::Execute_Max(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_MAX_SCALAR_RR:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                const float * fSrc2     = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    fDst[i] = std::max(fSrc1[i], fSrc2[i]);
                }
                break;
            }
        case OP_MAX_SCALAR_RC:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < (batchSize * 4); i += 4) {
                    fDst[i] = std::max(fSrc1[i], rhs);
                }
                break;
            }
        case OP_MAX_SCALAR_CR:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc2     = &Retrive_Element(ins.Src2, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    fDst[i] = std::max(lhs, fSrc2[i]);
                }
                break;
            }
        case OP_MAX_SCALAR_CC:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                float result            = std::max(lhs, rhs);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    fDst[i] = result;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_MAX_VECTOR2_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v2, pSrc1[i].u.v2, pSrc2[i].u.v2);
                }
                break;
            }
        case OP_MAX_VECTOR2_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v2, pSrc1[i].u.v2, rhs);
                }
                break;
            }
        case OP_MAX_VECTOR2_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v2, lhs, pSrc2[i].u.v2);
                }
                break;
            }
        case OP_MAX_VECTOR2_CC:
            {
                Vector2 result;
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;
                vf::Max(result, lhs, rhs);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v2 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_MAX_VECTOR3_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v3, pSrc1[i].u.v3, pSrc2[i].u.v3);
                }
                break;
            }
        case OP_MAX_VECTOR3_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v3, pSrc1[i].u.v3, rhs);
                }
                break;
            }
        case OP_MAX_VECTOR3_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v3, lhs, pSrc2[i].u.v3);
                }
                break;
            }
        case OP_MAX_VECTOR3_CC:
            {
                Vector3 result;
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;
                vf::Max(result, lhs, rhs);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v3 = result;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_MAX_VECTOR4_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v4, pSrc1[i].u.v4, pSrc2[i].u.v4);
                }
                break;
            }
        case OP_MAX_VECTOR4_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v4, pSrc1[i].u.v4, rhs);
                }
                break;
            }
        case OP_MAX_VECTOR4_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Max(pDst[i].u.v4, lhs, pSrc2[i].u.v4);
                }
                break;
            }
        case OP_MAX_VECTOR4_CC:
            {
                Vector4 result;
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;
                vf::Max(result, lhs, rhs);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v4 = result;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Execute_Dot - Dot-product.
     */
    Status_t VirtualMachine::Execute_Dot(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) {
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_DOT_VECTOR2_RR:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot((pSrc1++)->u.v2, (pSrc2++)->u.v2);
                }
                break;
            }
        case OP_DOT_VECTOR2_RC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot((pSrc1++)->u.v2, rhs);
                }
                break;
            }
        case OP_DOT_VECTOR2_CR:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot(lhs, (pSrc2++)->u.v2);
                }
                break;
            }
        case OP_DOT_VECTOR2_CC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;
                float v                 = vf::Dot(lhs, rhs);

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = v;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_DOT_VECTOR3_RR:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot((pSrc1++)->u.v3, (pSrc2++)->u.v3);
                }
                break;
            }
        case OP_DOT_VECTOR3_RC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot((pSrc1++)->u.v3, rhs);
                }
                break;
            }
        case OP_DOT_VECTOR3_CR:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot(lhs, (pSrc2++)->u.v3);
                }
                break;
            }
        case OP_DOT_VECTOR3_CC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;
                float v                 = vf::Dot(lhs, rhs);

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = v;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_DOT_VECTOR4_RR:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot((pSrc1++)->u.v4, (pSrc2++)->u.v4);
                }
                break;
            }
        case OP_DOT_VECTOR4_RC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot((pSrc1++)->u.v4, rhs);
                }
                break;
            }
        case OP_DOT_VECTOR4_CR:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = vf::Dot(lhs, (pSrc2++)->u.v4);
                }
                break;
            }
        case OP_DOT_VECTOR4_CC:
            {
                float * pDst            = &Retrive_Element(ins.Dst, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;
                float v                 = vf::Dot(lhs, rhs);

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = v;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Execute_Cross - Executes a cross() virtual machine instruction.
     */
    Status_t VirtualMachine::Execute_Cross(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) 
        {
        case OP_CROSS_RR:
            {
                Vector * pDst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2 = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Cross((pDst++)->u.v3, (pSrc1++)->u.v3, (pSrc2++)->u.v3);
                }
                break;
            }
        case OP_CROSS_RC:
            {
                Vector * pDst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1 = Retrive_Register(ins.Src1, batchOffset);
                const Vector3 & rhs = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Cross((pDst++)->u.v3, (pSrc1++)->u.v3, rhs);
                }
                break;
            }
        case OP_CROSS_CR:
            {
                Vector * pDst        = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2 = Retrive_Register(ins.Src2, batchOffset);
                const Vector3 & lhs  = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Cross((pDst++)->u.v3, lhs, (pSrc2++)->u.v3);
                }
                break;
            }
        case OP_CROSS_CC:
            {
                Vector3 result;
                Vector * pDst        = Retrive_Register(Register_Index(ins.Dst)) + batchOffset;
                const Vector3 & lhs  = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                const Vector3 & rhs  = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                vf::Cross(result, lhs, rhs);
                for(size_t i = 0; i < batchSize; ++i) {
                    (pDst++)->u.v3 = result;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }

        return Err_Success;
    }

    /**
     * Executes a sample1D(), sample2D() or a sample3D() bytecode instruction.
     */
    Status_t VirtualMachine::Execute_Sampler(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) {
        case OP_SAMPLE1D_R:
            {
                Vector * pDst                   = Retrive_Register(ins.Dst, batchOffset);
                Vector * pExp                   = Retrive_Register(ins.Src2, batchOffset);
                const vf::ISampler * sampler    = GetSampler(ins.Src1);

                if (!sampler->sample1D(pExp, pDst, batchSize)) {
                    return Err_SamplingFailed;
                }
                break;
            }
        case OP_SAMPLE1D_C:
            {
                Vector * pDst                   = Retrive_Register(ins.Dst, batchOffset);
                float fwide                     = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                const vf::ISampler * sampler    = GetSampler(ins.Src1);

                if (!sampler->sample1D(fwide, pDst, batchSize)) {
                    return Err_SamplingFailed;
                }
                break;
            }
        case OP_SAMPLE2D_R:
            {
                Vector * pDst                   = Retrive_Register(ins.Dst, batchOffset);
                Vector * pExp                   = Retrive_Register(ins.Src2, batchOffset);
                const vf::ISampler * sampler    = GetSampler(ins.Src1);
                
                if (!sampler->sample2D(pExp, pDst, batchSize)) {
                    return Err_SamplingFailed;
                }
                break;
            }
        case OP_SAMPLE2D_C:
            {
                Vector * pDst                   = Retrive_Register(ins.Dst, batchOffset);
                const vf::Vector2 & pos         = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;
                const vf::ISampler * sampler    = GetSampler(ins.Src1);

                if (!sampler->sample2D(pos, pDst, batchSize)) {
                    return Err_SamplingFailed;
                }
                break;
            }
        case OP_SAMPLE3D_R:
            {
                Vector * pDst                   = Retrive_Register(ins.Dst, batchOffset);
                Vector * pExp                   = Retrive_Register(ins.Src2, batchOffset);
                const vf::ISampler * sampler    = GetSampler(ins.Src1);

                if (!sampler->sample3D(pExp, pDst, batchSize)) {
                    return Err_SamplingFailed;
                }
                break;
            }
        case OP_SAMPLE3D_C:
            {
                Vector * pDst                   = Retrive_Register(ins.Dst, batchOffset);
                const vf::Vector3 & pos         = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;
                const vf::ISampler * sampler    = GetSampler(ins.Src1);

                if (!sampler->sample3D(pos, pDst, batchSize)) {
                    return Err_SamplingFailed;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /** 
     * Execute_Floor, executes a floor() instructiion.
     */
    Status_t VirtualMachine::Execute_Floor(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) 
        {
        case OP_FLOOR_SCALAR_R:
            {
                float * pDst        = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc  = &Retrive_Element(ins.Src1, batchOffset);
                
                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = floorf(pSrc[i]);
                }
                break;
            }
        case OP_FLOOR_SCALAR_C:
            {
                float * pDst = &Retrive_Element(ins.Dst, batchOffset);
                float value  = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = floorf(value);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_FLOOR_VECTOR2_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Floor((pDst++)->u.v2, (pSrc++)->u.v2);
                }
                break;
            }
        case OP_FLOOR_VECTOR2_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & w   = Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Floor((pDst++)->u.v2, w);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_FLOOR_VECTOR3_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Floor((pDst++)->u.v3, (pSrc++)->u.v3);
                }
                break;
            }
        case OP_FLOOR_VECTOR3_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & w   = Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Floor((pDst++)->u.v3, w);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_FLOOR_VECTOR4_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Floor((pDst++)->u.v4, (pSrc++)->u.v4);
                }
                break;
            }
        case OP_FLOOR_VECTOR4_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & w   = Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Floor((pDst++)->u.v4, w);
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
     }

    /** 
     * Execute_Ceil, executes a ceil() instructiion.
     */
    Status_t VirtualMachine::Execute_Ceil(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode) 
        {
        case OP_CEIL_SCALAR_R:
            {
                float * pDst        = &Retrive_Element(ins.Dst, batchOffset);
                const float * pSrc  = &Retrive_Element(ins.Src1, batchOffset);
                
                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = ceilf(pSrc[i]);
                }
                break;
            }
        case OP_CEIL_SCALAR_C:
            {
                float * pDst = &Retrive_Element(ins.Dst, batchOffset);
                float value  = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    pDst[i] = ceilf(value);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_CEIL_VECTOR2_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Ceil((pDst++)->u.v2, (pSrc++)->u.v2);
                }
                break;
            }
        case OP_CEIL_VECTOR2_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & w   = Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Ceil((pDst++)->u.v2, w);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_CEIL_VECTOR3_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Ceil((pDst++)->u.v3, (pSrc++)->u.v3);
                }
                break;
            }
        case OP_CEIL_VECTOR3_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & w   = Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Ceil((pDst++)->u.v3, w);
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_CEIL_VECTOR4_R:
            {
                Vector * pDst = Retrive_Register(ins.Dst, batchOffset);
                Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Ceil((pDst++)->u.v4, (pSrc++)->u.v4);
                }
                break;
            }
        case OP_CEIL_VECTOR4_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & w   = Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    vf::Ceil((pDst++)->u.v4, w);
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
     }

    /**
     * Execute bytecode instructions for comparing scalar values.
     */
    Status_t VirtualMachine::Execute_Comparison(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        uint8_t * flags = GetFlags();
        switch(ins.Opcode)
        {
            /*****************************************************************/
            /*                              Greater                          */
            /*****************************************************************/
        case OP_CMP_GRT_RR:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                const float * pRhs  = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] > pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_GRT_RC:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
            
                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] > rhs) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_GRT_CR:
            {
                const float * pRhs  = &Retrive_Element(ins.Src2, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (lhs > pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_GRT_CC:
            {
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                if (lhs > rhs) {
                    memset(flags, FLAG_CMP, batchSize);
                } else {
                    memset(flags, 0, batchSize);
                }
                break;
            }
            /*****************************************************************/
            /*                              Less                             */
            /*****************************************************************/
        case OP_CMP_LE_RR:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                const float * pRhs  = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] < pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_LE_RC:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
            
                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] < rhs) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_LE_CR:
            {
                const float * pRhs  = &Retrive_Element(ins.Src2, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (lhs < pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_LE_CC:
            {
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                if (lhs < rhs) {
                    memset(flags, FLAG_CMP, batchSize);
                } else {
                    memset(flags, 0, batchSize);
                }
                break;
            }
            /*****************************************************************/
            /*                              Equal                            */
            /*****************************************************************/
        case OP_CMP_EQ_RR:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                const float * pRhs  = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] == pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_EQ_RC:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
            
                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] == rhs) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_EQ_CR:
            {
                const float * pRhs  = &Retrive_Element(ins.Src2, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (lhs == pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_EQ_CC:
            {
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                if (lhs == rhs) {
                    memset(flags, FLAG_CMP, batchSize);
                } else {
                    memset(flags, 0, batchSize);
                }
                break;
            }
            /*****************************************************************/
            /*                          Greater or equal                     */
            /*****************************************************************/
        case OP_CMP_GEQ_RR:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                const float * pRhs  = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] >= pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_GEQ_RC:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
            
                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] >= rhs) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_GEQ_CR:
            {
                const float * pRhs  = &Retrive_Element(ins.Src2, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (lhs >= pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_GEQ_CC:
            {
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                if (lhs >= rhs) {
                    memset(flags, FLAG_CMP, batchSize);
                } else {
                    memset(flags, 0, batchSize);
                }
                break;
            }
            /*****************************************************************/
            /*                          Less or equal                        */
            /*****************************************************************/
        case OP_CMP_LEQ_RR:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                const float * pRhs  = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] <= pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_LEQ_RC:
            {
                const float * pLhs  = &Retrive_Element(ins.Src1, batchOffset);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
            
                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (pLhs[i] <= rhs) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_LEQ_CR:
            {
                const float * pRhs  = &Retrive_Element(ins.Src2, batchOffset);
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < (batchSize*4); i += 4) {
                    flags[i] = (lhs <= pRhs[i]) ? FLAG_CMP : 0;
                }
                break;
            }
        case OP_CMP_LEQ_CC:
            {
                float lhs           = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs           = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);
                if (lhs <= rhs) {
                    memset(flags, FLAG_CMP, batchSize);
                } else {
                    memset(flags, 0, batchSize);
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /*************************************************************************/
    /*                                  Statements                           */
    /*************************************************************************/

    /**
     * Execute_ConditionalAssignment. Exectutes a conditional assignment bytecode instruction.
     */
    Status_t VirtualMachine::Execute_ConditionalAssignment(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        const uint8_t * flags = GetFlags();
        switch(ins.Opcode) {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_COND_SCALAR_RR:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                const float * fSrc2     = &Retrive_Element(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; i++) {
                    fDst[i] = (flags[i] & FLAG_CMP) ? fSrc1[i] : fSrc2[i];
                }
                break;
            }
        case OP_COND_SCALAR_RC:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc1     = &Retrive_Element(ins.Src1, batchOffset);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    fDst[i] = (flags[i] & FLAG_CMP) ? fSrc1[i] : rhs;
                }
                break;
            }
        case OP_COND_SCALAR_CR:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                const float * fSrc2     = &Retrive_Element(ins.Src2, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);

                for(size_t i = 0; i < batchSize; i++) {
                    fDst[i] = (flags[i] & FLAG_CMP) ? lhs : fSrc2[i];
                }
                break;
            }
        case OP_COND_SCALAR_CC:
            {
                float * fDst            = &Retrive_Element(ins.Dst, batchOffset);
                float lhs               = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                float rhs               = (ins.Src2 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src2);

                for(size_t i = 0; i < batchSize; i++) {
                    fDst[i] = (flags[i] & FLAG_CMP) ? lhs : rhs;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_COND_VECTOR2_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v2 = (flags[i] & FLAG_CMP) ? pSrc1[i].u.v2 : pSrc2[i].u.v2;
                }
                break;
            }
        case OP_COND_VECTOR2_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v2 = (flags[i] & FLAG_CMP) ? pSrc1[i].u.v2 : rhs;
                }
                break;
            }
        case OP_COND_VECTOR2_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v2 = (flags[i] & FLAG_CMP) ? lhs : pSrc2[i].u.v2;
                }
                break;
            }
        case OP_COND_VECTOR2_CC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 & lhs     = (ins.Src1 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src1)->u.v2;
                const Vector2 & rhs     = (ins.Src2 == 0xff) ? *is.Decode2() : Retrive_Uniform(ins.Src2)->u.v2;
                
                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v2 = (flags[i] & FLAG_CMP) ? lhs : rhs;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_COND_VECTOR3_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v3 = (flags[i] & FLAG_CMP) ? pSrc1[i].u.v3 : pSrc2[i].u.v3;
                }
                break;
            }
        case OP_COND_VECTOR3_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v3 = (flags[i] & FLAG_CMP) ? pSrc1[i].u.v3 : rhs;
                }
                break;
            }
        case OP_COND_VECTOR3_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v3 = (flags[i] & FLAG_CMP) ? lhs : pSrc2[i].u.v3;
                }
                break;
            }
        case OP_COND_VECTOR3_CC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & lhs     = (ins.Src1 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src1)->u.v3;
                const Vector3 & rhs     = (ins.Src2 == 0xff) ? *is.Decode3() : Retrive_Uniform(ins.Src2)->u.v3;
                
                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v3 = (flags[i] & FLAG_CMP) ? lhs : rhs;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector4                          */
            /*****************************************************************/
        case OP_COND_VECTOR4_RR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v4 = (flags[i] & FLAG_CMP) ? pSrc1[i].u.v4 : pSrc2[i].u.v4;
                }
                break;
            }
        case OP_COND_VECTOR4_RC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc1    = Retrive_Register(ins.Src1, batchOffset);
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v4 = (flags[i] & FLAG_CMP) ? pSrc1[i].u.v4 : rhs;
                }
                break;
            }
        case OP_COND_VECTOR4_CR:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc2    = Retrive_Register(ins.Src2, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v4 = (flags[i] & FLAG_CMP) ? lhs : pSrc2[i].u.v4;
                }
                break;
            }
        case OP_COND_VECTOR4_CC:
            {
                Vector * pDst           = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & lhs     = (ins.Src1 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src1)->u.v4;
                const Vector4 & rhs     = (ins.Src2 == 0xff) ? *is.Decode4() : Retrive_Uniform(ins.Src2)->u.v4;
                
                for(size_t i = 0; i < batchSize; ++i) {
                    pDst[i].u.v4 = (flags[i] & FLAG_CMP) ? lhs : rhs;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }

    /**
     * Execute_Assignment. Executes a assignment bytecode instruction.
     */
    Status_t VirtualMachine::Execute_Assignment(const Instruction_t & ins, InstructionStream & is, size_t batchSize, size_t batchOffset)
    {
        switch(ins.Opcode)
        {
            /*****************************************************************/
            /*                              Scalar                           */
            /*****************************************************************/
        case OP_ASSIGN_SCALAR_R:
            {
                float * pDst = &Retrive_Element(ins.Dst, batchOffset);
                float * pSrc = &Retrive_Element(ins.Src1, batchOffset);

                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = pSrc[i];
                }
                break;
            }
        case OP_ASSIGN_SCALAR_C:
            {
                float * pDst = &Retrive_Element(ins.Dst, batchOffset);
                float fValue = (ins.Src1 == 0xff) ? is.DecodeScalar() : Retrive_UniformElement(ins.Src1);
                
                for(size_t i = 0; i < (batchOffset*4); i += 4) {
                    pDst[i] = fValue;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector2                          */
            /*****************************************************************/
        case OP_ASSIGN_VECTOR2_R:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    (pDst++)->u.v2 = (pSrc++)->u.v2;
                }
                break;
            }
        case OP_ASSIGN_VECTOR2_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector2 w     = Retrive_Uniform(ins.Src1)->u.v2;

                for(size_t i = 0; i < batchSize; ++i) {
                    (pDst++)->u.v2 = w;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_ASSIGN_VECTOR3_R:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    (pDst++)->u.v3 = (pSrc++)->u.v3;
                }
                break;
            }
        case OP_ASSIGN_VECTOR3_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector3 & w   = Retrive_Uniform(ins.Src1)->u.v3;

                for(size_t i = 0; i < batchSize; ++i) {
                    (pDst++)->u.v3 = w;
                }
                break;
            }
            /*****************************************************************/
            /*                              Vector3                          */
            /*****************************************************************/
        case OP_ASSIGN_VECTOR4_R:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector * pSrc = Retrive_Register(ins.Src1, batchOffset);

                for(size_t i = 0; i < batchSize; ++i) {
                    (pDst++)->u.v4 = (pSrc++)->u.v4;
                }
                break;
            }
        case OP_ASSIGN_VECTOR4_C:
            {
                Vector * pDst       = Retrive_Register(ins.Dst, batchOffset);
                const Vector4 & w   = Retrive_Uniform(ins.Src1)->u.v4;

                for(size_t i = 0; i < batchSize; ++i) {
                    (pDst++)->u.v4 = w;
                }
                break;
            }
        default:
            return Err_InvalidBytecode;
        }
        return Err_Success;
    }
}