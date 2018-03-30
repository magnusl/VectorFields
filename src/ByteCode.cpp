#include "bytecode.hpp"
#include <algorithm>

namespace vf
{
    using namespace std;

    /*************************************************************************/
    /*                              InstructionStream                        */
    /*************************************************************************/
    InstructionStream::InstructionStream(const std::vector<uint32_t> & stream) : 
        m_Code(stream), m_CodeIndex(0), m_CodeSize(stream.size())
    {
    }

    bool InstructionStream::Decode(Instruction_t & instr)
    {
        if (m_CodeIndex < m_CodeSize) {
            uint32_t dw     = m_Code[m_CodeIndex++];
            instr.Opcode    = (VM_Op) (dw >> 24);
            instr.Dst       = (dw >> 16) & 0xff;
            instr.Src1      = (dw >> 8) & 0xff;
            instr.Src2      = (dw & 0xff);
        } else {
            return false;
        }
        return true;
    }

    float InstructionStream::DecodeScalar()
    {
        if (m_CodeIndex < m_CodeSize) {
            return *((float *)&m_Code[m_CodeIndex++]);
        }
        throw std::runtime_error("Failed to decode float since stream is empty.");
    }

    const Vector2 * InstructionStream::Decode2()
    {
        if (m_CodeIndex < (m_CodeSize + 1)) {
            const Vector2 * ptr = ((const Vector2 *)&m_Code[m_CodeIndex]);
            m_CodeIndex += 2;
            return ptr;
        }
        throw std::runtime_error("Failed to decode Vector2 from instruction stream.");
    }

    const Vector3 * InstructionStream::Decode3()
    {
        if (m_CodeIndex < (m_CodeSize + 2)) {
            const Vector3 * ptr = ((const Vector3 *)&m_Code[m_CodeIndex]);
            m_CodeIndex += 3;
            return ptr;
        }
        throw std::runtime_error("Failed to decode Vector3 from instruction stream.");
    }

    const Vector4 * InstructionStream::Decode4()
    {
        if (m_CodeIndex < (m_CodeSize + 3)) {
            const Vector4 * ptr = ((const Vector4 *)&m_Code[m_CodeIndex]);
            m_CodeIndex += 4;
            return ptr;
        }
        throw std::runtime_error("Failed to decode Vector4 from instruction stream.");
    }

    /*************************************************************************/
    /*                              ByteCode_Method                          */
    /*************************************************************************/

    /**
     * Constructor.
     */
    ByteCode_Method::ByteCode_Method(const char * name) : m_Name(name)
    {
    }

    /**
     * Adds a instruction to the bytecode.
     */
    bool ByteCode_Method::emit(uint32_t instruction)
    {
        m_ByteCode.push_back(instruction);
        return true;
    }

    /**
     * Returns the name of the method.
     */
    const std::string & ByteCode_Method::GetName() const
    {
        return m_Name;
    }

    /**
     * Returns the bytecode instructions.
     */
    const std::vector<uint32_t> & ByteCode_Method::GetCode() const
    {
        return m_ByteCode;
    }

    /**
     * Inserts data into the instruction stream, used for constants
     */
    bool ByteCode_Method::emit(const void * ptr, size_t numBytes)
    {
        if ((ptr == nullptr) || (numBytes == 0) || (numBytes % 4)) {
            return false;
        }
        /** append the data to the vector */
        size_t presentSize = m_ByteCode.size();
        m_ByteCode.resize(presentSize + (numBytes / 4));
        memcpy(&m_ByteCode[presentSize], ptr, numBytes);
        return true;
    }

    /*************************************************************************/
    /*                              ByteCode                                 */
    /*************************************************************************/
    ByteCode::ByteCode
        (
            uint8_t NumRegisters,
            const std::map<std::string, Variable> & InputOutput,        /** Input/Output registers */
            const std::map<std::string, Variable> & Uniforms,           /** The uniform variables that hasn't been assigned a value */
            const std::map<std::string, Variable> & Samplers,           /** Samplers */
            std::vector<std::shared_ptr<ByteCode_Method> > & methods    /** Methods */
        ) 
        : m_NumRegisters(NumRegisters), 
        m_InputOutput(InputOutput), 
        m_Uniforms(Uniforms), 
        m_Samplers(Samplers),
        m_Methods(methods)
    {
    }

    /** Destructor */
    ByteCode::~ByteCode()
    {
    }

    /** Returns the number of registers the bytecode requires to execute */
    uint8_t ByteCode::GetNumRegisters() const
    {
        return m_NumRegisters;
    }

    /** Returns the number of uniform variables that are declared in the bytecode program */
    uint8_t ByteCode::GetNumUniforms() const
    {
        return static_cast<uint8_t>(m_Uniforms.size());
    }

    /** Returns the number of sampler variables that are declared in the bytecode program */
    uint8_t ByteCode::GetNumSamplers() const
    {
        return static_cast<uint8_t>(m_Samplers.size());
    }

    /** Retrive the index of the stream with the supplied name */
    int ByteCode::UniformLocation(const char * UniformName) const
    {
        std::map<std::string, Variable>::const_iterator it = m_Uniforms.find(UniformName);
        return (it == m_Uniforms.end()) ? -1 : static_cast<int>(it->second.m_Register);
    }

    /** Retrive the index of the stream with the supplied name */
    int ByteCode::StreamLocation(const char * StreamName) const
    {
        std::map<std::string, Variable>::const_iterator it = m_InputOutput.find(StreamName);
        return (it == m_InputOutput.end()) ? -1 : static_cast<int>(it->second.m_Register);
    }

    /** Retrive the index of the sampler with the supplied name */
    int ByteCode::SamplerLocation(const char * SamplerName) const
    {
        std::map<std::string, Variable>::const_iterator it = m_Samplers.find(SamplerName);
        return (it == m_Samplers.end()) ? -1 : static_cast<int>(it->second.m_SampleId);
    }

    /** Returns the stream variables used in the bytecode */
    const std::map<std::string, Variable> & ByteCode::GetInputOutput() const
    {
        return m_InputOutput;
    }

    /** Returns the uniform variables used in the bytecode */
    const std::map<std::string, Variable> & ByteCode::GetUniforms() const
    {
        return m_Uniforms;
    }

    /** Returns the uniform variables used in the bytecode */
    const std::map<std::string, Variable> & ByteCode::GetSamplers() const
    {
        return m_Samplers;
    }

    /** Returns the methods defined in the bytecode */
    const std::vector<std::shared_ptr<ByteCode_Method> > & ByteCode::GetMethods() const
    {
        return m_Methods;
    }
}