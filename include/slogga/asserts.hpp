#ifndef SLOGGA_ASSERTS_HPP
#define SLOGGA_ASSERTS_HPP

#include "log.hpp"
#include <stdexcept>
#include <source_location>

// Attribute assume allows the compiler to optimize assuming the expression will always evaluate to true.
// This should always be the case provided the code was properly tested.
#ifdef __has_cpp_attribute
	#if __has_cpp_attribute(assume)
		#define SLOGGA_ASSUME(x) [[assume(x)]]
	#elif __has_cpp_attribute(gnu::assume)
		#define SLOGGA_ASSUME(x) [[gnu::assume(x)]]
	#else
		#define SLOGGA_ASSUME(x) //nothing
	#endif
#else
	#define SLOGGA_ASSUME(x) //nothing
#endif

// Macros are used to avoid evaluation of the condition in release builds
#ifdef NDEBUG
    // Empty implementation for release builds

    #define EXPECTS(expr) SLOGGA_ASSUME(expr)
    #define ENSURES(expr) SLOGGA_ASSUME(expr)
    #define ASSERTS(expr) SLOGGA_ASSUME(expr)
#else
    // Actual implementation for debug builds
    #define EXPECTS(x) (slogga::assertion(x, #x, "precondition check"))
    #define ENSURES(x) (slogga::assertion(x, #x, "postcondition check"))
    #define ASSERTS(x) (slogga::assertion(x, #x, "assertion"))

    namespace slogga {
        class assertion_failed_exception : public std::exception {
            const char* m_cond_str;
            const char* m_assertion_type;
            std::source_location m_sl;
            mutable std::string m_what_string_cache = std::string();
        public:
            assertion_failed_exception(const char* cond_str, const char* assertion_type, std::source_location sl) :
                m_cond_str(cond_str), m_assertion_type(assertion_type), m_sl(std::move(sl)) {}

            virtual const char* what() const noexcept override {
                if(m_what_string_cache.empty()) {
                    m_what_string_cache =  std::format("{} failed at function '{}' at {}:{}:{}; '{}' evaluated to false",
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
