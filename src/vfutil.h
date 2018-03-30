#ifndef _VFUTIL_H_
#define _VFUTIL_H_

#include <vector>
#include <cstdint>

namespace vfutil
{
    /**
     * Bitmap utility class.
     */
    class Bitmap
    {
    public:
        Bitmap(size_t NumBits) : m_NumBits(NumBits)
        {
            m_vData.resize((NumBits / 32) + 1);
        }
        bool Get(size_t Index) const
        {
            if (Index < m_NumBits) {
                size_t ElementIndex = Index / 32, BitIndex = Index % 32;
                return (m_vData[ElementIndex] & (1 << BitIndex)) ? true : false;
            }
            return false;
        }
        void Clear(size_t Index)
        {
            if (Index < m_NumBits) {
                size_t ElementIndex = Index / 32, BitIndex = Index % 32;
                m_vData[ElementIndex] &= ~(1 << BitIndex);
            }
        }
        void Set(size_t Index)
        {
            if (Index < m_NumBits) {
                size_t ElementIndex = Index / 32, BitIndex = Index % 32;
                m_vData[ElementIndex] |= (1 << BitIndex);
            }
        }
    protected:
        std::vector<uint32_t>   m_vData;
        size_t m_NumBits;
    };
}

#endif