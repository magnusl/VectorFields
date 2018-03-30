#include "vf.h"
#include "vfvm.h"
#include "vfutil.h"

#include <memory>

namespace vf
{
    /**
     * The execution implementation.
     */
    class ExecutionImpl
    {
    public:
        ExecutionImpl(std::shared_ptr<vf::ByteCode>, void *, size_t);

        Status_t    Execute(size_t, size_t);
        Status_t    SetRegisterPointer(size_t, void *);
        Status_t    SetUniform(size_t, float);
        Status_t    SetUniform(size_t, const vf::Vector2 &);
        Status_t    SetUniform(size_t, const vf::Vector3 &);
        Status_t    SetUniform(size_t, const vf::Vector4 &);
        Status_t    SetSampler(size_t, vf::ISampler *);

    protected:
        std::shared_ptr<vf::ByteCode>           m_pBytecode;
        std::shared_ptr<vf::VirtualMachine>     m_pVirtualMachine;
        vfutil::Bitmap                          m_IoMap;
        size_t                                  m_BatchLimit;
    };

    /*************************************************************************/
    /*                              ByteCode_Execution                       */
    /*************************************************************************/
    ByteCode_Execution::ByteCode_Execution(
        std::shared_ptr<vf::ByteCode> pByteCode, void * ptrMem, size_t nMemSize)
    {
        m_pImpl = std::make_shared<ExecutionImpl>(pByteCode, ptrMem, nMemSize);
    }

    ByteCode_Execution::~ByteCode_Execution()
    {
    }

    Status_t ByteCode_Execution::SetRegisterPointer(size_t index, void * ptrMemory)
    {
        return m_pImpl->SetRegisterPointer(index, ptrMemory);
    }

    Status_t ByteCode_Execution::SetUniform(size_t index, float value)
    {
        return m_pImpl->SetUniform(index, value);
    }

    Status_t ByteCode_Execution::SetUniform(size_t index, const vf::Vector2 & value)
    {
        return m_pImpl->SetUniform(index, value);
    }

    Status_t ByteCode_Execution::SetUniform(size_t index, const vf::Vector3 & value)
    {
        return m_pImpl->SetUniform(index, value);
    }

    Status_t ByteCode_Execution::SetUniform(size_t index, const vf::Vector4 & value)
    {
        return m_pImpl->SetUniform(index, value);
    }

    Status_t ByteCode_Execution::SetSampler(size_t index, vf::ISampler * sampler)
    {
        return m_pImpl->SetSampler(index, sampler);
    }

    Status_t ByteCode_Execution::Execute(size_t methodIndex, size_t batchSize)
    {
        return m_pImpl->Execute(methodIndex, batchSize);
    }

    /*************************************************************************/
    /*                              Execution_Impl                           */
    /*************************************************************************/

    /**
     * Constructor, performs the required initialization such as assigning memory to
     * temporary registers.
     */
    ExecutionImpl::ExecutionImpl(std::shared_ptr<vf::ByteCode> bytecode, void * ptrMem, size_t MemSize) 
        : m_pBytecode(bytecode), m_IoMap(bytecode->GetNumRegisters())
    {
        // Mark each i/o register in the io map.
        const std::map<std::string, vf::Variable> & ioStreams = bytecode->GetInputOutput();
        for(std::map<std::string, vf::Variable>::const_iterator it = ioStreams.begin(); 
            it != ioStreams.end(); 
            it++)
        {
            m_IoMap.Set(it->second.m_Register);
        }

        m_pVirtualMachine = std::make_shared<vf::VirtualMachine>
            (
            m_IoMap,
            bytecode->GetNumRegisters(),
            bytecode->GetNumUniforms(),
            bytecode->GetNumSamplers()
            );

        // Divide the memory to the temporary registers. The amount of memory set the upper limit
        // on how large each batch size may be.
        size_t NumTemps     = bytecode->GetNumRegisters() - ioStreams.size();
        m_BatchLimit        = MemSize / ((16 * NumTemps) + 1);
        if (m_BatchLimit == 0) {
            throw std::runtime_error("Not enough memory reserved.");
        }
        /** Divide the memory to non i/o stream registers. */
        uint8_t * ptr = (uint8_t *) ptrMem;
        if (NumTemps) {
            for(size_t i = 0, num = bytecode->GetNumRegisters(); i < num; ++i) {
                if (!m_IoMap.Get(i)) {
                    m_pVirtualMachine->SetRegisterPointer(i, ptr);
                    ptr += (m_BatchLimit * 16);
                }
            }
        }
        /** Assign memory to the status register. */
        m_pVirtualMachine->SetFlagPointer(ptr);
    }

    /**
     * Executes the method specified by the MethodIndex
     */
    Status_t ExecutionImpl::Execute(size_t MethodIndex, size_t batchSize)
    {
        const std::vector<std::shared_ptr<ByteCode_Method> > & methods = m_pBytecode->GetMethods();
        if (MethodIndex >= methods.size()) {
            return Err_InvalidIndex;
        }

        for(size_t offset = 0; offset < batchSize; offset += m_BatchLimit) {
            size_t remaining = batchSize - offset;
            InstructionStream stream(methods[MethodIndex]->GetCode());
            Status_t err = m_pVirtualMachine->Execute(stream, remaining > m_BatchLimit ? m_BatchLimit : remaining, offset);
            if (err != Err_Success) {
                return err;
            }
        }
        return Err_Success;
    }

    Status_t ExecutionImpl::SetRegisterPointer(size_t index, void * ptrData)
    {
        if ((index > m_pBytecode->GetNumRegisters()) || (!m_IoMap.Get(index))) {
            return Err_InvalidRegister;
        }
        return m_pVirtualMachine->SetRegisterPointer(index, ptrData);
    }

    Status_t ExecutionImpl::SetUniform(size_t index, float value)
    {
        if (index > m_pBytecode->GetNumUniforms()) {
            return Err_InvalidRegister;
        }
        return m_pVirtualMachine->SetUniform(index, value);
    }

    Status_t ExecutionImpl::SetUniform(size_t index, const vf::Vector2 & value)
    {
        if (index > m_pBytecode->GetNumUniforms()) {
            return Err_InvalidRegister;
        }
        return m_pVirtualMachine->SetUniform(index, value);
    }

    Status_t ExecutionImpl::SetUniform(size_t index, const vf::Vector3 & value)
    {
        if (index > m_pBytecode->GetNumUniforms()) {
            return Err_InvalidRegister;
        }
        return m_pVirtualMachine->SetUniform(index, value);
    }

    Status_t ExecutionImpl::SetUniform(size_t index, const vf::Vector4 & value)
    {
        if (index > m_pBytecode->GetNumUniforms()) {
            return Err_InvalidRegister;
        }
        return m_pVirtualMachine->SetUniform(index, value);
    }

    Status_t ExecutionImpl::SetSampler(size_t index, vf::ISampler * sampler)
    {
        if (index > m_pBytecode->GetNumSamplers()) {
            return Err_InvalidRegister;
        }
        return m_pVirtualMachine->SetSampler(index, sampler);
    }
}