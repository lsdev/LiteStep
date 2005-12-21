#if !defined(MATHEXCEPTION_H)
#define MATHEXCEPTION_H

#include <stdexcept>
#include <string>


/**
 * Exception thrown when an error occurs parsing or evaluating a math expression.
 */
class MathException : public std::runtime_error
{
public:

    /**
     * Constructs a MathException with the specified message.
     */
    MathException(const std::string& message) throw() : std::runtime_error(message) { }
};


#endif
