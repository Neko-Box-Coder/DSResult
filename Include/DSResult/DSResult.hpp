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
#include <type_traits>
#include <utility>

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
    #define INTERNAL_DS_TEMP_NANE(name) INTERNAL_DS_COMPOSE(INTERNAL_DS_CONCAT, (name, __LINE__))
    
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
    struct InternalHasToString
    {
        template<typename U>
        static decltype(std::to_string(std::declval<U>()), std::true_type()) Test(int);
        
        template<typename U>
        static std::false_type Test(...);
        
        static constexpr bool Value = decltype(Test<T>(1))::value;
    };

    template<typename T>
    struct InternalHasStringCtor
    {
        template<typename U>
        static decltype(std::string(std::declval<U>()), std::true_type()) Test(int);
        
        template<typename U>
        static std::false_type Test(...);
        
        static constexpr bool Value = decltype(Test<T>(1))::value;
    };

    template<typename T, typename std::enable_if<InternalHasToString<T>::Value, bool>::type = true>
    std::string ToString(const T& value)
    {
        return std::to_string(value);
    }

    template<   typename T, 
                typename std::enable_if<!InternalHasToString<T>::Value && 
                                        InternalHasStringCtor<T>::Value, bool>::type = true>
    std::string ToString(const T& value)
    {
        return std::string(value);
    }
    
    template<   typename T, 
                typename std::enable_if
                <
                    !InternalHasToString<T>::Value &&
                    !InternalHasStringCtor<T>::Value &&
                    std::is_convertible<T, std::string>::value, 
                    bool
                >::type = true>
    std::string ToString(const T& value)
    {
        return (std::string)(value);
    }

    template<   typename T, 
                typename std::enable_if
                <
                    !InternalHasToString<T>::Value && 
                    !InternalHasStringCtor<T>::Value &&
                    !std::is_convertible<T, std::string>::value, 
                    bool
                >::type = true>
    std::string ToString(const T&)
    {
        static_assert(  std::false_type::value, 
                        "--> DS Error: No valid conversion to string for this type. "
                        "Either provide a string operator or "
                        "consider using DS_ASSERT_TRUE() or DS_ASSERT_FALSE() instead");
        return "";
    }
    
    template<typename T>
    using Result = DS_EXPECTED_TYPE<T, DS::ErrorTrace>;
    using Error = DS_UNEXPECTED_TYPE<DS::ErrorTrace>;

    #define DS_ERROR_MSG(msg) \
        DS::Error(DS::ErrorTrace(msg, DS::TraceElement(__func__, DSGetFileName(__FILE__), __LINE__)))
    #define DS_STR(nonStr) DS::ToString(nonStr)
    #define DS_APPEND_TRACE(prev) \
        (prev.AppendTrace(DS::TraceElement(__func__, DSGetFileName(__FILE__), __LINE__)), prev)
    
    #define INTERNAL_DS_ASSERT(left, op, right) \
        do \
        { \
            auto INTERNAL_DS_TEMP_NANE(autoLeft) = left; \
            auto INTERNAL_DS_TEMP_NANE(autoRight) = right; \
            if(!(INTERNAL_DS_TEMP_NANE(autoLeft) op INTERNAL_DS_TEMP_NANE(autoRight))) \
            { \
                return DS_ERROR_MSG(std::string("Expression \"") + \
                                    DS_STR(INTERNAL_DS_TEMP_NANE(autoLeft)) + \
                                    " " + #op + " " + \
                                    DS_STR(INTERNAL_DS_TEMP_NANE(autoRight)) + "\" has failed."); \
            } \
        } \
        while(false)
    
    
    //NOTE: Legacy, don't use
    #define DS_CHECKED_RETURN(resultVar) \
        do \
        { \
            if(!resultVar.has_value()) \
                return DS::Error(DS_APPEND_TRACE(resultVar.error())); \
        } \
        while(false)
    
    //NOTE: Legacy, don't use
    #define DS_UNWRAP_VOID_RETURN(op) \
        do \
        { \
            auto INTERNAL_DS_TEMP_NANE(dsResult) = op; \
            DS_CHECKED_RETURN(INTERNAL_DS_TEMP_NANE(dsResult)); \
        } \
        while(false)
    
    //NOTE: Legacy, don't use
    #define DS_UNWRAP_RETURN(unwrapVar, op) \
        auto INTERNAL_DS_TEMP_NANE(dsResult) = op; \
        DS_CHECKED_RETURN(INTERNAL_DS_TEMP_NANE(dsResult)); \
        unwrapVar = INTERNAL_DS_TEMP_NANE(dsResult).value()
    
    //NOTE: Legacy, don't use
    #define DS_ASSERT_RETURN(op) \
        do \
        { \
            if(!(op)) \
                return DS::Error(DS_ERROR_MSG("Expression \"" #op "\" has failed.")); \
        } \
        while(false)

    #define DS_CHECK(resultVar) DS_CHECKED_RETURN(resultVar)
    
    #define DS_UNWRAP_VOID(op) DS_UNWRAP_VOID_RETURN(op)
    #define DS_UNWRAP_DECL(unwrapVar, op) DS_UNWRAP_RETURN(unwrapVar, op)
    #define DS_UNWRAP_ASSIGN(unwrapVar, op) \
        do \
        { \
            DS_UNWRAP_RETURN(unwrapVar, op); \
        } \
        while(false)
    
    #define DS_ASSERT_TRUE(op) INTERNAL_DS_ASSERT(op, ==, true)
    #define DS_ASSERT_FALSE(op) INTERNAL_DS_ASSERT(op, ==, false)
    #define DS_ASSERT_EQ(op, val) INTERNAL_DS_ASSERT(op, ==, val)
    #define DS_ASSERT_NOT_EQ(op, val) INTERNAL_DS_ASSERT(op, !=, val)
    #define DS_ASSERT_GT(op, val) INTERNAL_DS_ASSERT(op, >, val)
    #define DS_ASSERT_GT_EQ(op, val) INTERNAL_DS_ASSERT(op, >=, val)
    #define DS_ASSERT_LT(op, val) INTERNAL_DS_ASSERT(op, <, val)
    #define DS_ASSERT_LT_EQ(op, val) INTERNAL_DS_ASSERT(op, <=, val)
}

#endif
