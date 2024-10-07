#ifndef SLOGGA_ASSERTS_HPP
#define SLOGGA_ASSERTS_HPP

#include "log.hpp"
#include <stdexcept>

// Macros are used to avoid evaluation of the condition in release builds
#ifdef NDEBUG
    // Empty implementation for release builds

    // Attribute assume allows the compiler to optimize assuming the expression will always evaluate to false.
    // This should always be the case provided the code was properly tested.
    #define EXPECTS(expr) [[assume(expr)]]
    #define ENSURES(expr) [[assume(expr)]]
    #define ASSERTS(expr) [[assume(expr)]]
#else
    // Actual implementation for debug builds
    #include <source_location>
    #include "./log.hpp"
    #define EXPECTS(x) (slogga::detail::assertion(x, #x, "precondition"))
    #define ENSURES(x) (slogga::detail::assertion(x, #x, "postcondition"))
    #define ASSERTS(x) (slogga::detail::assertion(x, #x, "assertion"))

    namespace slogga::detail {

        inline void assertion(bool cond, const char* cond_str, const char* assertion_type, std::source_location sl = std::source_location::current()) {
            if(!cond) {

                std::string err = std::format("{} check failed at function '{}' at {}:{}:{}; '{}' evaluated to false",
                    assertion_type, sl.function_name(), sl.file_name(), sl.line(), sl.column(), cond_str);

                slogga::stdout_log.fatal(err);
                throw std::runtime_error(err);
            }
        }
    }
#endif


#endif // SLOGGA_ASSERTS_HPP
