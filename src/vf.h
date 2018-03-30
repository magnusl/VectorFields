#ifndef _VF_H_
#define _VF_H_

#include "bytecode.hpp"
#include "vfexcept.h"

namespace vf
{
    class ExecutionImpl;

    typedef enum {
        Err_Success,
        Err_UnassignedRegisterPointer,
        Err_InvalidParameter,
        Err_InvalidBatchSize,
        Err_InvalidBytecode,
        Err_InvalidRegister,
        Err_UnknownMethod,
        Err_SamplingFailed,
        Err_InvalidIndex,
        Err_AllocationError,
        Err_ParseError
    } Status_t;

    /**
     * Used for executing bytecode.
     */
    class ByteCode_Execution
    {
    public:
        ByteCode_Execution(std::shared_ptr<vf::ByteCode>, void *, size_t);
        ~ByteCode_Execution();

        Status_t    Execute(size_t index, size_t batchSize);
        Status_t    SetRegisterPointer(size_t, void *);
        Status_t    SetUniform(size_t, float);
        Status_t    SetUniform(size_t, const vf::Vector2 &);
        Status_t    SetUniform(size_t, const vf::Vector3 &);
        Status_t    SetUniform(size_t, const vf::Vector4 &);
        Status_t    SetSampler(size_t, vf::ISampler *);

    protected:
        ByteCode_Execution(const ByteCode_Execution &);
        ByteCode_Execution & operator=(const ByteCode_Execution&);

        std::shared_ptr<ExecutionImpl> m_pImpl;
    };
}

#endif