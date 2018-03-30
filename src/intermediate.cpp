#include "intermediate.hpp"
#include <stdexcept>
#include <algorithm>
#include "codegen.hpp"

using namespace std;

namespace vf
{
    /**
     * Parses the source to form a program.
     *
     * \param       pSource     The program source.
     */
    Program::Program(const char * pSource) 
    {
        if (pSource) {
            Status_t err = Parse(pSource);
            switch(err) {
            case Err_AllocationError: break;
            case Err_ParseError: break;
            }
        }
    }

    /**
     * Performs any necessary cleanup.
     */
    Program::~Program()
    {
    }

    /*************************************************************************/
    /*                  Interface for parsing source code.                   */
    /*************************************************************************/
    Status_t Program::Parse(const char * pSourceCode)
    {
        if (!pSourceCode) {
            return Err_InvalidParameter;
        }
            
        stringstream source(pSourceCode);
        m_pTokenizer = make_shared<vf::Tokenizer>(source);
        if (!m_pTokenizer) {
            return Err_AllocationError;
        }
        
        if (!(m_pProgram = m_Parser.Parse( *m_pTokenizer ))) {
            return Err_ParseError;
        }
        
        auto & symtab = m_pTokenizer->SymbolTable();

        m_Parser.GetRoot()->GetVariables(m_pTokenizer->SymbolTable(), m_InputOutput, ATTRIBUTE_IN | ATTRIBUTE_OUT | ATTRIBUTE_INOUT);
        m_Parser.GetRoot()->GetVariables(m_pTokenizer->SymbolTable(), m_Uniforms, ATTRIBUTE_UNIFORM);
        m_Parser.GetRoot()->GetVariables(m_pTokenizer->SymbolTable(), m_Samplers, ATTRIBUTE_SAMPLER);
        m_NumRequiredRegisters = m_Parser.GetRoot()->GetNumRegisters();

        return Err_Success;
    }

    Status_t Program::Parse(const std::string & SourceCode)
    {
        return Parse(SourceCode.c_str());
    }

    /*************************************************************************/
    /*                  Interface to set uniform variables                   */
    /*************************************************************************/

    /** Sets a float uniform value */
    void Program::SetUniform(const char * pName, float fValue)
    {
        m_CodeGenerator.SetUniform(m_pTokenizer->SymbolTable().Insert(pName, false), fValue);
    }

    /** Sets a vector2 uniform value */
    void Program::SetUniform(const char * pName, const vf::Vector2 & v2)
    {
        m_CodeGenerator.SetUniform(m_pTokenizer->SymbolTable().Insert(pName, false), v2);
    }

    /** Sets a vector3 uniform value */
    void Program::SetUniform(const char * pName, const vf::Vector3 & v3)
    {
        m_CodeGenerator.SetUniform(m_pTokenizer->SymbolTable().Insert(pName, false), v3);
    }

    /** Sets a vector4 uniform value */
    void Program::SetUniform(const char * pName, const vf::Vector4 & v4)
    {
        m_CodeGenerator.SetUniform(m_pTokenizer->SymbolTable().Insert(pName, false), v4);
    }

    /*************************************************************************/
    /*                              Compilation                              */
    /*************************************************************************/

    /**
     * Compiles a program into the corresponding bytecode.
     */
    shared_ptr<vf::ByteCode> Program::Compile()
    {
        auto & symtab       = m_pTokenizer->SymbolTable();
        auto & functions    = m_pProgram->GetFunctions();

        std::vector<shared_ptr<vf::ByteCode_Method> >   methods;
        for(auto it = functions.begin(); it != functions.end(); it++)
        {
            shared_ptr<vf::ByteCode_Method> pMethod =  make_shared<vf::ByteCode_Method>( symtab.Retrive((*it)->m_Name) );
            if (!m_CodeGenerator.SetSink( *pMethod ).Compile( *it, m_Parser.GetRoot() )) {
                return shared_ptr<vf::ByteCode>();
            }
            methods.push_back(pMethod);
        }

        uint8_t numRegisters = m_NumRequiredRegisters + m_CodeGenerator.NumTemporaryRegisters();
        shared_ptr<vf::ByteCode> bc = make_shared<vf::ByteCode> (numRegisters, m_InputOutput, m_Uniforms, m_Samplers, methods);
        return bc;
    }

}