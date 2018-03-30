/**
 * \file            emit.cpp
 * \description     Implements the functionality for emitting bytecode for the Virtual Machine (VM).
 *
 * \author          Magnus Leksell
 */

#include "common.hpp"
#include "codegen.hpp"
#include "vm.hpp"

#include <assert.h>

namespace vf
{

/**
 * Emits a constant into the instruction stream.
 */
static void emit_constant(IEmitSink * sink,const vf::ExpInfo & exp)
{
    switch(exp.type) {
    case vf::Type_Float:    sink->emit(exp.value.u.f); break;
    case vf::Type_Vec2:     sink->emit(exp.value.u.v2); break;
    case vf::Type_Vec3:     sink->emit(exp.value.u.v3); break;
    case vf::Type_Vec4:     sink->emit(exp.value.u.v4); break;
    }
}

int Make_TypeOffset(const vf::ExpInfo & left)
{
    switch(left.type) {
    case vf::Type_Float:    return 0;
    case vf::Type_Vec2:     return 1;
    case vf::Type_Vec3:     return 2;
    case vf::Type_Vec4:     return 3;
    default:                return 0;
    }
}

int Make_OperandOffset(const vf::ExpInfo & left, const vf::ExpInfo & right)
{
    if (left.isuniform || left.isconst) {
        if (right.isconst || right.isuniform) { // _CC
            return 3;
        } else { // _CR
            return 2;
        }
    } else if (right.isuniform || right.isconst) { // _RC
        return 1;
    } else { // _RR
        return 0;
    }
}

uint8_t Make_Operand(const vf::ExpInfo & info)
{
    if (info.isuniform) {
        return (info.uniform_index << 2) | (info.regidx & 0x03);
    } else if (info.isconst) {
        return 0xff;
    } else {
        return Make_Register(info.reg, info.regidx);
    }
}

/** Emits a addition into the instruction stream */
bool emit_add(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & left, vf::ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) {
    case vf::Type_Float:    opcode = OP_SCALAR_ADD_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec2:     opcode = OP_VECTOR2_ADD_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3:     opcode = OP_VECTOR3_ADD_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4:     opcode = OP_VECTOR4_ADD_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }
    uint32_t instr = 
        Make_Opcode(opcode) |
        Make_Destination(Make_Register(reg, offset)) |
        Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);

    return true;
}

/** Emits a subtraction into the instruction stream */
bool emit_sub(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & left, vf::ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) {
    case vf::Type_Float:    opcode = OP_SCALAR_SUB_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec2:     opcode = OP_VECTOR2_SUB_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3:     opcode = OP_VECTOR3_SUB_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4:     opcode = OP_VECTOR4_SUB_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }
    uint32_t instr = 
        Make_Opcode(opcode) |
        Make_Destination(Make_Register(reg, offset)) |
        Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);

    return true;
}

/** Emits a addition into the instruction stream */
bool emit_mul(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & left, vf::ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) {
    case vf::Type_Float:    opcode = OP_SCALAR_MUL_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec2:     opcode = OP_VECTOR2_SCALAR_MUL_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3:     opcode = OP_VECTOR3_SCALAR_MUL_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4:     opcode = OP_VECTOR4_SCALAR_MUL_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }
    uint32_t instr = 
        Make_Opcode(opcode) |
        Make_Destination(Make_Register(reg, offset)) |
        Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);

    return true;
}

/** Emits a addition into the instruction stream */
bool emit_div(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & left, vf::ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) {
    case vf::Type_Float:    opcode = OP_SCALAR_DIV_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec2:     opcode = OP_VECTOR2_SCALAR_DIV_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3:     opcode = OP_VECTOR3_SCALAR_DIV_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4:     opcode = OP_VECTOR4_SCALAR_DIV_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }
    uint32_t instr = Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) |
        Make_FirstOperand(Make_Operand(left)) | Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);
    
    return true;
}

/**
 * Emits the bytecode for performing a dot product between two vectors.
 */
bool emit_dot(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & left, vf::ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) {
    case vf::Type_Vec2: opcode = OP_DOT_VECTOR2_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3: opcode = OP_DOT_VECTOR3_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4: opcode = OP_DOT_VECTOR4_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }

    uint32_t instr =
        Make_Opcode(opcode) |
        Make_Destination(Make_Register(reg, offset)) |
        Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);

    return true;
}

/**
 * Emits the bytecode for performing a cross product between two 3d vectors.
 */
bool emit_cross(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & left, vf::ExpInfo & right)
{
    uint8_t opcode = static_cast<uint8_t>(OP_CROSS_RR + Make_OperandOffset(left, right));

    uint32_t instr =
        Make_Opcode(opcode) |
        Make_Destination(Make_Register(reg, offset)) |
        Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);

    return true;
}

/**
 * Emits the bytecode for calculating the length of a vector.
 */
bool emit_length(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    uint8_t opcode;
    bool    isconst = operand.isconst || operand.isuniform;

    switch(operand.type) {
    case vf::Type_Vec2:     opcode = isconst ? vf::OP_LENGTH_VECTOR2_C : vf::OP_LENGTH_VECTOR2_R; break;                
    case vf::Type_Vec3:     opcode = isconst ? vf::OP_LENGTH_VECTOR3_C : vf::OP_LENGTH_VECTOR3_R; break;
    case vf::Type_Vec4:     opcode = isconst ? vf::OP_LENGTH_VECTOR4_C : vf::OP_LENGTH_VECTOR4_R; break;
    default:                return false;
    }

    uint32_t instr = Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)    emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a sine operation.
 */
bool emit_sine(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_SINE_C : OP_SINE_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a cosine operation.
 */
bool emit_cosine(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_COSINE_C : OP_COSINE_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a tangent operation.
 */
bool emit_tangent(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_TANGENT_C : OP_TANGENT_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a arcsine operation.
 */
bool emit_arcsine(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_ARCSINE_C : OP_ARCSINE_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a arccosine operation.
 */
bool emit_arccosine(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_ARCCOSINE_C : OP_ARCCOSINE_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a cosine operation.
 */
bool emit_arctangent(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_ARCTANGENT_C : OP_ARCTANGENT_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a sqrt operation.
 */
bool emit_sqrt(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_SQRT_C : OP_SQRT_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits bytecode for performing a invsqrt operation.
 */
bool emit_invsqrt(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    bool    isconst = operand.isconst || operand.isuniform;
    uint32_t instr = Make_Opcode(isconst ? OP_INVSQRT_C : OP_INVSQRT_R) | Make_Destination(Make_Register(reg, offset)) | 
        Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Negation
 */
bool emit_negate(IEmitSink * sink, uint32_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    uint8_t opcode;
    bool    isconst = operand.isconst || operand.isuniform;

    switch(operand.type) {
    case vf::Type_Float:    opcode = (isconst ? OP_SCALAR_NEGATE_C : OP_SCALAR_NEGATE_R); break;
    case vf::Type_Vec2:     opcode = (isconst ? OP_VECTOR2_NEGATE_C : OP_VECTOR2_NEGATE_R); break;
    case vf::Type_Vec3:     opcode = (isconst ? OP_VECTOR3_NEGATE_C : OP_VECTOR3_NEGATE_R); break;
    case vf::Type_Vec4:     opcode = (isconst ? OP_VECTOR4_NEGATE_C : OP_VECTOR4_NEGATE_R); break;
    default:
        return false;
    }

    sink->emit(Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand)));
    if (operand.isconst)
        emit_constant(sink, operand);

    return true;
}

/**
 * Emits the bytecode for calculating the length of a vector.
 */
bool emit_normalize(IEmitSink * sink, uint8_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    uint8_t opcode;
    bool    isconst = operand.isconst || operand.isuniform;

    switch(operand.type) {
    case vf::Type_Vec2:     opcode = isconst ? OP_VECTOR2_NORMALIZE_C : OP_VECTOR2_NORMALIZE_R; break;              
    case vf::Type_Vec3:     opcode = isconst ? OP_VECTOR3_NORMALIZE_C : OP_VECTOR3_NORMALIZE_R; break;
    case vf::Type_Vec4:     opcode = isconst ? OP_VECTOR4_NORMALIZE_C : OP_VECTOR4_NORMALIZE_R; break;
    default:                return false;
    }

    uint32_t instr = Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand));

    sink->emit(instr);
    if (operand.isconst)    emit_constant(sink, operand);

    return true;
}

/**
 * Emits the bytecode for performing a assignment.
 */
bool emit_assign(IEmitSink * sink, uint8_t reg, uint8_t offset, ExpInfo & operand)
{
    uint32_t instr;
    bool constant = operand.isconst || operand.isuniform;

    switch(operand.type) {
    case vf::Type_Float:
        instr = Make_Opcode(constant ? OP_ASSIGN_SCALAR_C : OP_ASSIGN_SCALAR_R) | 
            Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand));
        break;
    case vf::Type_Vec2:
        instr = Make_Opcode(constant ? OP_ASSIGN_VECTOR2_C : OP_ASSIGN_VECTOR2_R) |
            Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand));
        break;
    case vf::Type_Vec3:
        instr = Make_Opcode(constant ? OP_ASSIGN_VECTOR3_C : OP_ASSIGN_VECTOR3_R) |
            Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand));
            break;
    case vf::Type_Vec4:
        instr = Make_Opcode(constant ? OP_ASSIGN_VECTOR4_C : OP_ASSIGN_VECTOR4_R) |
            Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand));
        break;
    }
    sink->emit(instr);
    if (operand.isconst) emit_constant(sink, operand);

    return true;
}

/**
 * Emits the bytecode for performing a conditional assignment based on the result off a 
 * previous comparison.
 */
bool emit_cond_assign(IEmitSink * sink, uint8_t reg, uint8_t offset, ExpInfo & left, ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) 
    {
    case vf::Type_Float:    opcode = OP_COND_SCALAR_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec2:     opcode = OP_COND_VECTOR2_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3:     opcode = OP_COND_VECTOR3_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4:     opcode = OP_COND_VECTOR4_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }
    uint32_t instr = Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);
    
    return true;
}

/**
 * Emits a bytecode for performing a > comparison of the two operands.
 */
bool emit_greater(IEmitSink * sink, vf::ExpInfo & operand1, vf::ExpInfo & operand2)
{
    uint8_t opcode = static_cast<uint8_t>(vf::OP_CMP_GRT_RR + Make_OperandOffset(operand1, operand2));
    if (!sink->emit(Make_Opcode(opcode)|Make_FirstOperand(Make_Operand(operand1))|
        Make_SecondOperand(Make_Operand(operand2)))) {
        return false;
    }
    if (operand1.isconst) emit_constant(sink, operand1);
    if (operand2.isconst) emit_constant(sink, operand2);

    return true;
}

/**
 * Emits a bytecode for performing a > comparison of the two operands.
 */
bool emit_greater_equal(IEmitSink * sink, vf::ExpInfo & operand1, vf::ExpInfo & operand2)
{
    uint8_t opcode = static_cast<uint8_t>(vf::OP_CMP_GEQ_RR + Make_OperandOffset(operand1, operand2));
    if (!sink->emit(Make_Opcode(opcode)|Make_FirstOperand(Make_Operand(operand1))|
        Make_SecondOperand(Make_Operand(operand2)))) {
        return false;
    }
    if (operand1.isconst) emit_constant(sink, operand1);
    if (operand2.isconst) emit_constant(sink, operand2);

    return true;
}

/**
 * Emits a bytecode for performing a < comparison of the two operands.
 */
bool emit_less(IEmitSink * sink, vf::ExpInfo & operand1, vf::ExpInfo & operand2)
{
    uint8_t opcode = static_cast<uint8_t>(vf::OP_CMP_LE_RR + Make_OperandOffset(operand1, operand2));
    if (!sink->emit(Make_Opcode(opcode)|Make_FirstOperand(Make_Operand(operand1))|
        Make_SecondOperand(Make_Operand(operand2)))) {
        return false;
    }
    if (operand1.isconst) emit_constant(sink, operand1);
    if (operand2.isconst) emit_constant(sink, operand2);

    return true;
}

/**
 * Emits a bytecode for performing a < comparison of the two operands.
 */
bool emit_less_equal(IEmitSink * sink, vf::ExpInfo & operand1, vf::ExpInfo & operand2)
{
    uint8_t opcode = vf::OP_CMP_LEQ_RR + Make_OperandOffset(operand1, operand2);
    if (!sink->emit(Make_Opcode(opcode)|Make_FirstOperand(Make_Operand(operand1))|
        Make_SecondOperand(Make_Operand(operand2)))) {
        return false;
    }
    if (operand1.isconst) emit_constant(sink, operand1);
    if (operand2.isconst) emit_constant(sink, operand2);

    return true;
}

/**
 * Emits a bytecode for performing a < comparison of the two operands.
 */
bool emit_equal(IEmitSink * sink, vf::ExpInfo & operand1, vf::ExpInfo & operand2)
{
    uint8_t opcode = vf::OP_CMP_EQ_RR + Make_OperandOffset(operand1, operand2);
    if (!sink->emit(Make_Opcode(opcode)|Make_FirstOperand(Make_Operand(operand1))|
        Make_SecondOperand(Make_Operand(operand2)))) {
        return false;
    }
    if (operand1.isconst) emit_constant(sink, operand1);
    if (operand2.isconst) emit_constant(sink, operand2);

    return true;
}

/**
 * Emits the bytecode for performing a min() operation.
 */
bool emit_min(IEmitSink * sink, uint8_t reg, uint8_t offset, ExpInfo & left, ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) 
    {
    case vf::Type_Float:    opcode = OP_MIN_SCALAR_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec2:     opcode = OP_MIN_VECTOR2_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3:     opcode = OP_MIN_VECTOR3_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4:     opcode = OP_MIN_VECTOR4_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }
    uint32_t instr = Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);
    
    return true;
}

/**
 * Emits the bytecode for performing a max() operation.
 */
bool emit_max(IEmitSink * sink, uint8_t reg, uint8_t offset, ExpInfo & left, ExpInfo & right)
{
    uint8_t opcode;
    switch(left.type) 
    {
    case vf::Type_Float:    opcode = OP_MAX_SCALAR_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec2:     opcode = OP_MAX_VECTOR2_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec3:     opcode = OP_MAX_VECTOR3_RR + Make_OperandOffset(left, right); break;
    case vf::Type_Vec4:     opcode = OP_MAX_VECTOR4_RR + Make_OperandOffset(left, right); break;
    default:
        return false;
    }
    uint32_t instr = Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(left)) |
        Make_SecondOperand(Make_Operand(right));

    sink->emit(instr);
    if (left.isconst)   emit_constant(sink, left);
    if (right.isconst)  emit_constant(sink, right);
    
    return true;
}

bool emit_sample(IEmitSink * sink, uint8_t reg, uint8_t offset, uint8_t sampler, vf::ExpInfo & operand)
{
    uint8_t opcode;
    switch(operand.type) {
    case Type_Float:    opcode = operand.isconst ? OP_SAMPLE1D_C : OP_SAMPLE1D_R; break;
    case Type_Vec2:     opcode = operand.isconst ? OP_SAMPLE2D_C : OP_SAMPLE2D_R; break;
    case Type_Vec3:     opcode = operand.isconst ? OP_SAMPLE3D_C : OP_SAMPLE3D_R; break;
    default:            return false;
    }

    sink->emit(Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(sampler) | 
        Make_SecondOperand(Make_Operand(operand)));
    
    if (operand.isconst)    emit_constant(sink, operand);
    return true;
}

bool emit_floor(IEmitSink * sink, uint8_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    uint8_t opcode;
    switch(operand.type) {
    case Type_Float:    opcode = operand.isconst ? OP_FLOOR_SCALAR_C : OP_FLOOR_SCALAR_R; break;
    case Type_Vec2:     opcode = operand.isconst ? OP_FLOOR_VECTOR2_C : OP_FLOOR_VECTOR2_R; break;
    case Type_Vec3:     opcode = operand.isconst ? OP_FLOOR_VECTOR3_C : OP_FLOOR_VECTOR3_C; break;
    case Type_Vec4:     opcode = operand.isconst ? OP_FLOOR_VECTOR4_C : OP_FLOOR_VECTOR4_C; break;
    default:            return false;
    }

    sink->emit(Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand)));
    
    if (operand.isconst)    emit_constant(sink, operand);
    return true;
}

bool emit_ceil(IEmitSink * sink, uint8_t reg, uint8_t offset, vf::ExpInfo & operand)
{
    uint8_t opcode;
    switch(operand.type) {
    case Type_Float:    opcode = operand.isconst ? OP_CEIL_SCALAR_C : OP_CEIL_SCALAR_R; break;
    case Type_Vec2:     opcode = operand.isconst ? OP_CEIL_VECTOR2_C : OP_CEIL_VECTOR2_R; break;
    case Type_Vec3:     opcode = operand.isconst ? OP_CEIL_VECTOR3_C : OP_CEIL_VECTOR3_C; break;
    case Type_Vec4:     opcode = operand.isconst ? OP_CEIL_VECTOR4_C : OP_CEIL_VECTOR4_C; break;
    default:            return false;
    }

    sink->emit(Make_Opcode(opcode) | Make_Destination(Make_Register(reg, offset)) | Make_FirstOperand(Make_Operand(operand)));
    
    if (operand.isconst)    emit_constant(sink, operand);
    return true;
}
}