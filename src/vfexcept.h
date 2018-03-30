#ifndef _VFEXCEPT_H_
#define _VFEXCEPT_H_

#include <stdexcept>

namespace vf
{
    /**
     * Thrown when a parse error is encountered.
     */
    class ParseError_Exception
    {
    public:
        ParseError_Exception(const char *);
        ParseError_Exception(const std::string &);
    };
}

#endif