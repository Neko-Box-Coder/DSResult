#ifndef DS_RESULT_HPP
#define DS_RESULT_HPP

#if !defined(DS_USE_TL_EXPECTED) && \
    !defined(DS_USE_EXPECTED_LITE) && \
    !defined(DS_USE_STD_EXPECTED) && \
    !defined(DS_USE_CUSTOM_EXPECTED)
    
    #define DS_USE_TL_EXPECTED 1
#endif

#if DS_USE_TL_EXPECTED
    #include "tl/expected.hpp"
    #define DS_EXPECTED_TYPE tl::expected
    #define DS_UNEXPECTED_TYPE tl::unexpected
#elif DS_USE_EXPECTED_LITE
    #include "nonstd/expected.hpp"
    #define DS_EXPECTED_TYPE nonstd::expected
    #define DS_UNEXPECTED_TYPE nonstd::unexpected_type
#elif DS_USE_STD_EXPECTED
    #include <expected>
    #define DS_EXPECTED_TYPE std::expected
    #define DS_UNEXPECTED_TYPE std::unexpected
#elif DS_USE_CUSTOM_EXPECTED
    //User custom expected
    #if !defined(DS_EXPECTED_TYPE) || !defined(DS_UNEXPECTED_TYPE)
        static_assert(false,    "DS_EXPECTED_TYPE and DS_UNEXPECTED_TYPE must be defined for custom "
                                "expected type");
    #endif
#else
    static_assert(false,    "DS_USE_TL_EXPECTED or DS_USE_EXPECTED_LITE or DS_USE_STD_EXPECTED or "
                            "DS_USE_CUSTOM_EXPECTED must be defined");
#endif

#include <string>
#include <vector>

namespace
{
    #if __cplusplus >= 201402L
        constexpr 
    #endif
    const char* DSGetFileName(const char* path) 
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
    struct TraceElement
    {
        const char* Function;
        const char* File;
        const int Line;
        #if __cplusplus >= 201402L
            constexpr 
        #endif
        TraceElement(   const char* func, 
                        const char* filepath, 
                        const int line) :   Function(func),
                                            File(DSGetFileName(filepath)),
                                            Line(line)
        {}

        std::string ToString() const 
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
        ErrorTrace( const std::string& msg, 
                    const char* func, 
                    const char* file, 
                    const int line) : Message(msg)
        {
            Stack.emplace_back(func, file, line);
        }

        void AppendTrace(const char* func, const char* file, const int line)
        {
            Stack.emplace_back(func, file, line);
        }

        operator std::string() const 
        {
            std::string result = "Error:\n  " + Message + "\n\nStack trace:";
            for(const TraceElement& trace : Stack)
                result += "\n  at " + trace.ToString();
            
            return result;
        }

        std::string ToString() const 
        {
            return static_cast<std::string>(*this);
        }
    };

    template<typename T>
    using Result = DS_EXPECTED_TYPE<T, DS::ErrorTrace>;
    using Error = DS_UNEXPECTED_TYPE<DS::ErrorTrace>;

    #define DS_ERROR_MSG(msg) DS::ErrorTrace(msg, __func__, __FILE__, __LINE__)
    #define DS_STR(nonStr) std::to_string(nonStr)
    #define DS_APPEND_TRACE(prev) (prev.AppendTrace(__func__, __FILE__, __LINE__), prev) 
    #define DS_CHECKED_RETURN(resultVar) \
        if(!resultVar.has_value()) \
            return DS::Error(DS_APPEND_TRACE(resultVar.error()));
    #define DS_ASSERT_RETURN(op) \
        if(!(op)) \
            return DS::Error(DS_ERROR_MSG("Expression \"" #op "\" has failed."));
}

#endif
