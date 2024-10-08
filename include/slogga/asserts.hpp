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
    #define EXPECTS(x) (slogga::assertion(x, #x, "precondition"))
    #define ENSURES(x) (slogga::assertion(x, #x, "postcondition"))
    #define ASSERTS(x) (slogga::assertion(x, #x, "assertion"))

    namespace slogga {
        class assertion_failed_exception : std::exception {
            const char* m_cond_str;
            const char* m_assertion_type;
            std::source_location m_sl;
            mutable std::string m_what_string_cache = std::string();
        public:
            assertion_failed_exception(const char* cond_str, const char* assertion_type, std::source_location sl) :
                m_cond_str(cond_str), m_assertion_type(assertion_type), m_sl(std::move(sl)) {}

            virtual const char* what() const noexcept override {
                if(m_what_string_cache.empty()) {
                    m_what_string_cache =  std::format("{} check failed at function '{}' at {}:{}:{}; '{}' evaluated to false",
                        m_assertion_type, m_sl.function_name(), m_sl.file_name(), m_sl.line(), m_sl.column(), m_cond_str);
                }
                return m_what_string_cache.c_str();
            }
        };

        inline void assertion(bool cond, const char* cond_str, const char* assertion_type, std::source_location sl = std::source_location::current()) {
            if(!cond) {
                throw assertion_failed_exception(cond_str, assertion_type, std::move(sl));
            }
        }
    }
#endif


#endif // SLOGGA_ASSERTS_HPP
