#include <vector>
#include "vec4.hpp"
#include "vf.h"
#include "sampler.hpp"
#include "vfutil.h"

namespace vf
{
    enum {
        FLAG_CMP = (1 << 0)
    }; 

    /**
     * VirtualMachine, executes bytecode from a instruction stream.
     */
    class VirtualMachine
    {
    public:
        VirtualMachine(const vfutil::Bitmap & IoMap, uint8_t NumRegisters, uint8_t NumUniforms, uint8_t NumSamplers);

        Status_t    Execute(vf::InstructionStream & stream, size_t batchSize, size_t batchOffset);
        Status_t    SetRegisterPointer(size_t, void *);
        Status_t    SetUniform(size_t, float);
        Status_t    SetUniform(size_t, const vf::Vector2 &);
        Status_t    SetUniform(size_t, const vf::Vector3 &);
        Status_t    SetUniform(size_t, const vf::Vector4 &);
        Status_t    SetSampler(size_t, vf::ISampler *);
        void        SetFlagPointer(void *);

    protected: // methods

        typedef Status_t (VirtualMachine::*pInstrImpl_t) (const Instruction_t &, InstructionStream &, size_t, size_t);
        void BuildCallTable();

        /*********************************************************************/
        /*                              Instructions                         */
        /*********************************************************************/
        Status_t Execute_Add(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Sub(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Mul(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Div(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Negate(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Dot(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Trigonometric(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Length(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Sqrt(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Normalize(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Cross(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Assignment(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_ConditionalAssignment(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Comparison(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Min(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Max(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Sampler(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Floor(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);
        Status_t Execute_Ceil(const Instruction_t &, InstructionStream &, size_t batchSize, size_t batchOffset);

    protected: // helper methods used during execution.
        Vector *        Retrive_Register(uint8_t, size_t offset = 0);
        float &         Retrive_Element(uint8_t, size_t offset = 0);
        Vector *        Retrive_Uniform(uint8_t);
        float &         Retrive_UniformElement(uint8_t);
        vf::ISampler *  GetSampler(uint8_t);
        uint8_t *       GetFlags();

    protected: // variables

        std::vector<void *>         m_Registers;
        std::vector<vf::Vector>     m_Uniforms;
        std::vector<vf::ISampler *> m_Samplers;
        uint8_t *                   m_Flags;
        std::vector<pInstrImpl_t>   m_CallTable;
        const vfutil::Bitmap &      m_IoMap;
    };


    inline uint8_t Register_Index(uint8_t b)
    {
        return b >> 2;
    }

    inline uint8_t Register_Member(uint8_t b)
    {
        return b & 0x03;
    }
}