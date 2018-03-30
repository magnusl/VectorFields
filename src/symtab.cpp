#include "symtab.hpp"
#include <memory>

namespace vf
{
    /**
     * Returns the string associated with the SymIndex, or null.
     */
    const char * SymbolTable::Retrive(SymbolTable::SymIndex index) const
    {
        return this->m_StringPool.GetPointer(index);
    }

    /**
     * Inserts a string into the symboltable and returns the symindex for the string.
     */
    SymbolTable::SymIndex SymbolTable::Insert(const char * pStr, bool modify)
    {
        if (pStr==nullptr) {
            return (SymbolTable::SymIndex) -1;
        }

        SymbolTable::Node * pCurrent = m_pRoot, ** dst = &m_pRoot;
        while(pCurrent != nullptr) {
            int res = strcmp(pStr, pCurrent->m_pStr);
            if (res == 0) {
                return pCurrent->m_nOffset;
            } else if (res < 0) {  // check left subtree
                if (pCurrent->m_pLeft == nullptr) {  // insert it here
                    dst = &pCurrent->m_pLeft;
                    break;
                } else {
                    pCurrent = pCurrent->m_pLeft;
                }
            } else { // check right subtree.
                if (pCurrent->m_pRight == nullptr) {
                    dst = &pCurrent->m_pRight;
                    break;
                } else {
                    pCurrent = pCurrent->m_pRight;
                }
            }
        }

        if (!modify) {
            return (SymbolTable::SymIndex) -1;
        }

        *dst = new (std::nothrow) SymbolTable::Node;
        // insert the string into the string pool.
        size_t offset = m_StringPool.Insert(pStr, strlen(pStr) + 1);
        if (offset == ((size_t) -1)) {
            return (SymbolTable::SymIndex) -1;
        }
        (*dst)->m_pStr     = m_StringPool.GetPointer(offset);
        (*dst)->m_nOffset  = offset;
        if (!(*dst)->m_pStr) {
            return (SymbolTable::SymIndex) -1;
        }
        return offset;
    }
}