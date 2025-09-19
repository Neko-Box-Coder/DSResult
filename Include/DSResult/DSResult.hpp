#ifndef DS_RESULT_HPP
#define DS_RESULT_HPP

#if !defined(DS_USE_TL_EXPECTED) && \
    !defined(DS_USE_EXPECTED_LITE) && \
    !defined(DS_USE_STD_EXPECTED) && \
    !defined(DS_USE_CUSTOM_EXPECTED)
    
    #define DS_USE_TL_EXPECTED 1
#endif

#if defined(DS_USE_TL_EXPECTED) && DS_USE_TL_EXPECTED
    #include "tl/expected.hpp"
    #define DS_EXPECTED_TYPE tl::expected
    #define DS_UNEXPECTED_TYPE tl::unexpected
#elif defined(DS_USE_EXPECTED_LITE) && DS_USE_EXPECTED_LITE
    #include "nonstd/expected.hpp"
    #define DS_EXPECTED_TYPE nonstd::expected
    #define DS_UNEXPECTED_TYPE nonstd::unexpected_type
#elif defined(DS_USE_STD_EXPECTED) && DS_USE_STD_EXPECTED
    #include <expected>
    #define DS_EXPECTED_TYPE std::expected
    #define DS_UNEXPECTED_TYPE std::unexpected
#elif defined(DS_USE_CUSTOM_EXPECTED) && DS_USE_CUSTOM_EXPECTED
    //User custom expected
    #if !defined(DS_EXPECTED_TYPE) || !defined(DS_UNEXPECTED_TYPE)
        static_assert(false,    "DS_EXPECTED_TYPE and DS_UNEXPECTED_TYPE must be defined for custom "
                                "expected type");
    #endif
#else
    static_assert(false,    "DS_USE_TL_EXPECTED or DS_USE_EXPECTED_LITE or DS_USE_STD_EXPECTED or "
                            "DS_USE_CUSTOM_EXPECTED must be defined");
#endif

#if __cplusplus >= 202002L
    #define INTERNAL_DS_FUNC_CONSTEVAL consteval
#elif __cplusplus >= 201402L
    #define INTERNAL_DS_FUNC_CONSTEVAL constexpr
#else
    #define INTERNAL_DS_FUNC_CONSTEVAL
#endif

#include <string>
#include <vector>

namespace
{
    inline INTERNAL_DS_FUNC_CONSTEVAL const char* DSGetFileName(const char* path) 
    {
        const char* lastSlash = path;
        const char* curr = path;
        while(*curr) 
        {
            if(*curr == '/' || *curr == '\\')
                lastSlash = curr + 1;
            ++curr;
        }
        return lastSlash;
    }
}

namespace DS
{
    #define INTERNAL_DS_CONCAT(a, b) a ## b
    #define INTERNAL_DS_COMPOSE(a, b) a b
    #define INTERNAL_DS_TEMP_NANE INTERNAL_DS_COMPOSE(INTERNAL_DS_CONCAT, (dsResult, __LINE__))
    
    struct TraceElement
    {
        const char* Function;
        const char* File;
        int Line;

        inline INTERNAL_DS_FUNC_CONSTEVAL
        TraceElement(   const char* func, 
                        const char* filepath, 
                        const int line) :   Function(func),
                                            File(filepath),
                                            Line(line)
        {}

        inline TraceElement& operator=(const TraceElement& other)
        {
            if(this != &other)
            {
                Function = other.Function;
                File = other.File;
                Line = other.Line;
            }
            return *this;
        }

        inline TraceElement(const TraceElement& other)
        {
            *this = other;
        }
        
        inline TraceElement& operator=(TraceElement&& other)
        {
            Function = other.Function;
            File = other.File;
            Line = other.Line;
            return *this;
        }
        
        inline TraceElement(TraceElement&& other)
        {
            *this = other;
        }

        inline std::string ToString() const 
        {
            return  std::string(File) + ":" + std::to_string(Line) + " in " + std::string(Function) + 
                    "()";
        }
    };

    struct ErrorTrace
    {
        std::string Message;
        std::vector<TraceElement> Stack;

        //Constructor for new error
        inline ErrorTrace(const std::string& msg, const TraceElement& element) : Message(msg)
        {
            Stack.emplace_back(element);
        }

        inline ErrorTrace& operator=(const ErrorTrace& other)
        {
            if(this != &other)
            {
                Message = std::move(other.Message);
                Stack = std::move(other.Stack);
            }
            return *this;
        }

        inline ErrorTrace(const ErrorTrace& other)
        {
            *this = other;
        }
        
        inline ErrorTrace& operator=(ErrorTrace&& other)
        {
            Message = other.Message;
            Stack = other.Stack;
            return *this;
        }
        
        inline ErrorTrace(ErrorTrace&& other)
        {
            *this = other;
        }

        inline void AppendTrace(const TraceElement& element)
        {
            Stack.push_back(element);
        }

        inline operator std::string() const 
        {
            std::string result = "Error:\n  " + Message + "\n\nStack trace:";
            for(const TraceElement& trace : Stack)
                result += "\n  at " + trace.ToString();
            
            return result;
        }

        inline std::string ToString() const 
        {
            return static_cast<std::string>(*this);
        }
    };

    template<typename T>
    using Result = DS_EXPECTED_TYPE<T, DS::ErrorTrace>;
    using Error = DS_UNEXPECTED_TYPE<DS::ErrorTrace>;

    #define DS_ERROR_MSG(msg) \
        DS::Error(DS::ErrorTrace(msg, DS::TraceElement(__func__, DSGetFileName(__FILE__), __LINE__)))
    #define DS_STR(nonStr) std::to_string(nonStr)
    #define DS_APPEND_TRACE(prev) \
        (prev.AppendTrace(DS::TraceElement(__func__, DSGetFileName(__FILE__), __LINE__)), prev)
    
    #define DS_CHECKED_RETURN(resultVar) \
        do \
        { \
            if(!resultVar.has_value()) \
                return DS::Error(DS_APPEND_TRACE(resultVar.error())); \
        } \
        while(false)
    
    #define DS_UNWRAP_VOID_RETURN(op) \
        do \
        { \
            auto INTERNAL_DS_TEMP_NANE = op; \
            DS_CHECKED_RETURN(INTERNAL_DS_TEMP_NANE); \
        } \
        while(false)
    
    #define DS_UNWRAP_RETURN(unwrapVar, op) \
        auto INTERNAL_DS_TEMP_NANE = op; \
        DS_CHECKED_RETURN(INTERNAL_DS_TEMP_NANE); \
        unwrapVar = INTERNAL_DS_TEMP_NANE.value()

    #define DS_ASSERT_RETURN(op) \
        do \
        { \
            if(!(op)) \
                return DS::Error(DS_ERROR_MSG("Expression \"" #op "\" has failed.")); \
        } \
        while(false)

    #define DS_UNWRAP_VOID(op) DS_UNWRAP_VOID_RETURN(op)
    #define DS_UNWRAP_DECL(unwrapVar, op) DS_UNWRAP_RETURN(unwrapVar, op)
    #define DS_UNWRAP_ASSIGN(unwrapVar, op) \
        do \
        { \
            DS_UNWRAP_RETURN(unwrapVar, op); \
        } \
        while(false)
    #define DS_CHECK(resultVar) DS_CHECKED_RETURN(resultVar)
    #define DS_ASSERT(op) DS_ASSERT_RETURN(op)
}

#endif
