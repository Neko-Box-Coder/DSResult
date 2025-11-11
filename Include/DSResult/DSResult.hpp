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

#if defined(DS_NO_PATH) && DS_NO_PATH
    #define DS_PATH "(Private File)"
#else
    #define DS_PATH __FILE__
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
        int ErrorCode;

        inline ErrorTrace() : Message(), Stack(), ErrorCode(0) {};

        //Constructor for new error
        inline ErrorTrace(const std::string& msg, const TraceElement& element) :    Message(msg),
                                                                                    Stack(),
                                                                                    ErrorCode(0)
        {
            Stack.emplace_back(element);
        }

        inline ErrorTrace(  const std::string& msg, 
                            const TraceElement& element,
                            int errorCode) :    Message(msg),
                                                Stack(),
                                                ErrorCode(errorCode)
        {
            Stack.emplace_back(element);
        }

        inline ErrorTrace& operator=(const ErrorTrace& other)
        {
            Message = other.Message;
            Stack = other.Stack;
            ErrorCode = other.ErrorCode;
            return *this;
        }

        inline ErrorTrace(const ErrorTrace& other)
        {
            *this = other;
        }
        
        inline ErrorTrace& operator=(ErrorTrace&& other)
        {
            if(this != &other)
            {
                Message = std::move(other.Message);
                Stack = std::move(other.Stack);
                ErrorCode = other.ErrorCode;
            }
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
            std::string result =
                "Error:\n  " + Message + 
                (ErrorCode == 0 ? "" : "\nError Code: " + std::to_string(ErrorCode)) +
                "\n\nStack trace:";
            
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

    template<typename T, typename std::enable_if<std::is_pointer<T>::value, bool>::type = true>
    std::string ToString(const T& value)
    {
        return std::to_string((unsigned long long)(void*)value);
    }

    template<   typename T, 
                typename std::enable_if<!std::is_pointer<T>::value &&
                                        InternalHasToString<T>::Value, bool>::type = true>
    std::string ToString(const T& value)
    {
        return std::to_string(value);
    }

    template<   typename T, 
                typename std::enable_if<!std::is_pointer<T>::value &&
                                        !InternalHasToString<T>::Value && 
                                        InternalHasStringCtor<T>::Value, bool>::type = true>
    std::string ToString(const T& value)
    {
        return std::string(value);
    }
    
    template<   typename T, 
                typename std::enable_if
                <
                    !std::is_pointer<T>::value &&
                    !InternalHasToString<T>::Value &&
                    !InternalHasStringCtor<T>::Value &&
                    std::is_convertible<T, std::string>::value, bool>::type = true
                >
    std::string ToString(const T& value)
    {
        return (std::string)(value);
    }

    template<   typename T, 
                typename std::enable_if
                <
                    !std::is_pointer<T>::value &&
                    !InternalHasToString<T>::Value && 
                    !InternalHasStringCtor<T>::Value &&
                    !std::is_convertible<T, std::string>::value, bool>::type = true
                >
    std::string ToString(const T&)
    {
        static_assert(  std::false_type::value, 
                        "--> DS Error: No valid conversion to string for this type. "
                        "Either provide a string operator or "
                        "consider using DS_ASSERT_TRUE() or DS_ASSERT_FALSE() instead");
        return "";
    }
    
    template<typename T>
    struct Result : public DS_EXPECTED_TYPE<T, DS::ErrorTrace> 
    {
        inline Result() : DS_EXPECTED_TYPE<T, DS::ErrorTrace>() {}
        inline Result(const T& val) : DS_EXPECTED_TYPE<T, DS::ErrorTrace>(val) {}
        
        template<   typename Y, 
                    typename std::enable_if<std::is_convertible<Y, T>::value, bool>::type = true>
        inline Result(const Y& val) : DS_EXPECTED_TYPE<T, DS::ErrorTrace>(val) {}
        
        inline Result(const DS_EXPECTED_TYPE<T, DS::ErrorTrace>& ex) : 
            DS_EXPECTED_TYPE<T, DS::ErrorTrace>(ex) {}
        
        template<   typename Y, 
                    typename std::enable_if<std::is_convertible<Y, T>::value, bool>::type = true>
        inline Result(const DS_EXPECTED_TYPE<Y, DS::ErrorTrace>& ex) : 
            DS_EXPECTED_TYPE<T, DS::ErrorTrace>(ex) {}
        
        inline ~Result() {};
        
        using Base = DS_EXPECTED_TYPE<T, DS::ErrorTrace>;
        
        template<class F>
        inline const Result<T>& CallIfFailed(F&& f) const &
        {
            if(!DS_EXPECTED_TYPE<T, DS::ErrorTrace>::has_value())
                f(DS_EXPECTED_TYPE<T, DS::ErrorTrace>::error());
            return *this;
        }
        
        inline T DefaultOr() const&
        {
            return DS_EXPECTED_TYPE<T, DS::ErrorTrace>::value_or(T());
        }
        
        inline T DefaultOr() const &&
        {
            return DS_EXPECTED_TYPE<T, DS::ErrorTrace>::value_or(T());
        }
        
        inline bool HasValue() const
        {
            return DS_EXPECTED_TYPE<T, DS::ErrorTrace>::has_value();
        }
        
        inline const T& Value() const&
        {
            return DS_EXPECTED_TYPE<T, DS::ErrorTrace>::value();
        }
        
        inline T&& Value() const &&
        {
            return std::move(DS_EXPECTED_TYPE<T, DS::ErrorTrace>::value());
        }
        
        inline const DS::ErrorTrace& Error() const&
        {
            return DS_EXPECTED_TYPE<T, DS::ErrorTrace>::error();
        }
        
        inline const DS::ErrorTrace&& Error() const &&
        {
            return std::move(DS_EXPECTED_TYPE<T, DS::ErrorTrace>::error());
        }
    };
    
    template<>
    struct Result<void> : public DS_EXPECTED_TYPE<void, DS::ErrorTrace> 
    {
        inline Result() : DS_EXPECTED_TYPE<void, DS::ErrorTrace>() {}
        inline Result(const DS_EXPECTED_TYPE<void, DS::ErrorTrace>& ex) : 
            DS_EXPECTED_TYPE<void, DS::ErrorTrace>(ex) {}
        
        inline ~Result() {};
        
        using Base = DS_EXPECTED_TYPE<void, DS::ErrorTrace>;
        
        template<class F>
        inline const Result<void>& CallIfFailed(F&& f) const &
        {
            if(!DS_EXPECTED_TYPE<void, DS::ErrorTrace>::has_value())
                f(DS_EXPECTED_TYPE<void, DS::ErrorTrace>::error());
            return *this;
        }
        
        inline void DefaultOr() const&      { return; }
        inline void DefaultOr() const &&    { return; }
        inline bool HasValue() const
        {
            return DS_EXPECTED_TYPE<void, DS::ErrorTrace>::has_value();
        }
        inline void Value() const&      { return; }
        inline void Value() const &&    { return; }
        
        inline const DS::ErrorTrace& Error() const&
        {
            return DS_EXPECTED_TYPE<void, DS::ErrorTrace>::error();
        }
        
        inline const DS::ErrorTrace&& Error() const &&
        {
            return std::move(DS_EXPECTED_TYPE<void, DS::ErrorTrace>::error());
        }
    };
    
    struct Error : public DS_UNEXPECTED_TYPE<DS::ErrorTrace>
    {
        Error(const DS::ErrorTrace& et) : DS_UNEXPECTED_TYPE<DS::ErrorTrace>(et) {}
        Error(const Error& other) : DS_UNEXPECTED_TYPE<DS::ErrorTrace>(other) {}
        
        template< typename T >
        operator Result<T>() const
        {
            return DS_EXPECTED_TYPE<T, DS::ErrorTrace>(DS_UNEXPECTED_TYPE<DS::ErrorTrace>(*this));
        }
    };
}

namespace
{
    thread_local DS::ErrorTrace DSGlobalErrorTrace;
}

namespace DS
{
    inline void ProcessError(DS::ErrorTrace et) 
    {
        if(DSGlobalErrorTrace.Stack.empty())
            DSGlobalErrorTrace = et;
        return;
    }

    #define DS_ERROR_MSG(msg) \
        DS::Error(DS::ErrorTrace(msg, DS::TraceElement(__func__, DSGetFileName(DS_PATH), __LINE__)))
    
    #define DS_ERROR_MSG_EC(msg, errorCode) \
        DS::Error(DS::ErrorTrace(   msg, \
                                    DS::TraceElement(__func__, DSGetFileName(DS_PATH), __LINE__), \
                                    (int)errorCode))
    
    #define DS_STR(nonStr) DS::ToString(nonStr)
    #define DS_APPEND_TRACE(prev) \
        (prev.AppendTrace(DS::TraceElement(__func__, DSGetFileName(DS_PATH), __LINE__)), prev)
    
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
    
    #define INTERNAL_DS_ASSERT_EC(left, op, right, errorCode) \
        do \
        { \
            auto INTERNAL_DS_TEMP_NANE(autoLeft) = left; \
            auto INTERNAL_DS_TEMP_NANE(autoRight) = right; \
            if(!(INTERNAL_DS_TEMP_NANE(autoLeft) op INTERNAL_DS_TEMP_NANE(autoRight))) \
            { \
                return DS_ERROR_MSG_EC( std::string("Expression \"") + \
                                        DS_STR(INTERNAL_DS_TEMP_NANE(autoLeft)) + \
                                        " " + #op + " " + \
                                        DS_STR(INTERNAL_DS_TEMP_NANE(autoRight)) + "\" has failed.", \
                                        errorCode); \
            } \
        } \
        while(false)
    
    //NOTE: Legacy, don't use
    #define DS_CHECKED_RETURN(resultVar) \
        do \
        { \
            if(!resultVar.has_value()) \
                return DS::Error(std::move(DS_APPEND_TRACE(resultVar.error()))); \
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

    #define DS_TMP_ERROR dsTempResultRef.error()
    #define DS_CHECK(resultVar) DS_CHECKED_RETURN(resultVar)
    #define DS_CHECK_ACT(resultVar, failedActions) \
        do \
        { \
            if(!resultVar.has_value()) \
            { \
                failedActions; \
            } \
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
    
    #define DS_UNWRAP_VOID_ACT(op, failedActions) \
        do \
        { \
            auto INTERNAL_DS_TEMP_NANE(dsResult) = op; \
            auto& dsTempResultRef = INTERNAL_DS_TEMP_NANE(dsResult); (void)dsTempResultRef; \
            if(!INTERNAL_DS_TEMP_NANE(dsResult).has_value()) \
            { \
                failedActions; \
            } \
        } \
        while(false)
    
    #define DS_UNWRAP_DECL_ACT(unwrapVar, op, failedActions) \
        auto INTERNAL_DS_TEMP_NANE(dsResult) = op; \
        auto& dsTempResultRef = INTERNAL_DS_TEMP_NANE(dsResult); (void)dsTempResultRef; \
        if(!INTERNAL_DS_TEMP_NANE(dsResult).has_value()) \
        { \
            failedActions; \
        } \
        unwrapVar = INTERNAL_DS_TEMP_NANE(dsResult).value()
    
    #define DS_UNWRAP_ASSIGN_ACT(unwrapVar, op, failedActions) \
        do \
        { \
            auto INTERNAL_DS_TEMP_NANE(dsResult) = op; \
            auto& dsTempResultRef = INTERNAL_DS_TEMP_NANE(dsResult); (void)dsTempResultRef; \
            if(!INTERNAL_DS_TEMP_NANE(dsResult).has_value()) \
            { \
                failedActions; \
            } \
            unwrapVar = INTERNAL_DS_TEMP_NANE(dsResult).value(); \
        } \
        while(false)
    
    #define DS_VALUE_OR() CallIfFailed(DS::ProcessError).DefaultOr()
    
    #define DS_CHECK_PREV() \
        do \
        { \
            if(!DSGlobalErrorTrace.Stack.empty()) \
            { \
                DS::ErrorTrace returnErrorTrace = std::move(DSGlobalErrorTrace); \
                DSGlobalErrorTrace = DS::ErrorTrace(); \
                return DS::Error(std::move(DS_APPEND_TRACE(returnErrorTrace))); \
            } \
        } while(false)

    #define DS_CHECK_PREV_ACT(failedActions) \
        do \
        { \
            if(!DSGlobalErrorTrace.Stack.empty()) \
            { \
                DS::Result<void> returnErr = DS::Error(std::move(DSGlobalErrorTrace)); \
                DSGlobalErrorTrace = DS::ErrorTrace(); \
                DS::Result<void>& dsTempResultRef = returnErr; (void)dsTempResultRef; \
                failedActions; \
            } \
        } while(false)

    #define DS_TRY() DS_VALUE_OR(); DS_CHECK_PREV()

    #define DS_TRY_ACT(failedActions) DS_VALUE_OR(); DS_CHECK_PREV_ACT(failedActions)

    #define DS_ASSERT_TRUE(op) INTERNAL_DS_ASSERT(op, ==, true)
    #define DS_ASSERT_FALSE(op) INTERNAL_DS_ASSERT(op, ==, false)
    #define DS_ASSERT_EQ(op, val) INTERNAL_DS_ASSERT(op, ==, val)
    #define DS_ASSERT_NOT_EQ(op, val) INTERNAL_DS_ASSERT(op, !=, val)
    #define DS_ASSERT_GT(op, val) INTERNAL_DS_ASSERT(op, >, val)
    #define DS_ASSERT_GT_EQ(op, val) INTERNAL_DS_ASSERT(op, >=, val)
    #define DS_ASSERT_LT(op, val) INTERNAL_DS_ASSERT(op, <, val)
    #define DS_ASSERT_LT_EQ(op, val) INTERNAL_DS_ASSERT(op, <=, val)
    
    #define DS_ASSERT_TRUE_EC(op, errorCode) INTERNAL_DS_ASSERT_EC(op, ==, true, errorCode)
    #define DS_ASSERT_FALSE_EC(op, errorCode) INTERNAL_DS_ASSERT_EC(op, ==, false, errorCode)
    #define DS_ASSERT_EQ_EC(op, val, errorCode) INTERNAL_DS_ASSERT_EC(op, ==, val, errorCode)
    #define DS_ASSERT_NOT_EQ_EC(op, val, errorCode) INTERNAL_DS_ASSERT_EC(op, !=, val, errorCode)
    #define DS_ASSERT_GT_EC(op, val, errorCode) INTERNAL_DS_ASSERT_EC(op, >, val, errorCode)
    #define DS_ASSERT_GT_EQ_EC(op, val, errorCode) INTERNAL_DS_ASSERT_EC(op, >=, val, errorCode)
    #define DS_ASSERT_LT_EC(op, val, errorCode) INTERNAL_DS_ASSERT_EC(op, <, val, errorCode)
    #define DS_ASSERT_LT_EQ_EC(op, val, errorCode) INTERNAL_DS_ASSERT_EC(op, <=, val, errorCode)
    
    
}

#endif
