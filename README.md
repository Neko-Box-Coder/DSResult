# Dead Simple Result With Error Tracing

A single header only simple C++11 struct for allowing golang/rust like error handling and tracing
with expected like container

Supports [tl::expected](https://github.com/TartanLlama/expected), 
[expected lite](https://github.com/martinmoene/expected-lite.git),
[std expected](https://cppreference.com/w/cpp/header/expected.html) or custom expected like container.

## Integration

### CMake

You just need to add the project with `add_subdirectory()` and link it with `target_link_libraries`.
```cmake
add_subdirectory("path/to/DSResult")
target_link_libraries(yourTarget PRIVATE DSResult)
```

By default it uses "TartanLlama/expected" as expected container backend.

You can change the backend option by setting the `DS_EXPECTED_BACKEND` cmake option to either 
`TL`, `LITE`, `STD` or `CUSTOM`.

If you don't want any file paths in the binary, you can set the `DS_NO_PATH` to true in cmake.

Then you can include DSResult with `#include "DSResult/DSResult.hpp"`.

### Manual

Add `Include` to your project include path and include DSResult with `#include "DSResult/DSResult.hpp"`.

By default it uses `tl::expected`, if nothing is instructed. 

Define one of the following macros to choose a different backend

```cpp
#define DS_USE_TL_EXPECTED 1
#define DS_USE_EXPECTED_LITE 1
#define DS_USE_STD_EXPECTED 1
#define DS_USE_CUSTOM_EXPECTED 1
```

If you don't want any file paths in the binary, define the following macro
```cpp
#define DS_NO_PATH 1
```

If you are using a custom expected like container, you need to define the macros `DS_EXPECTED_TYPE` 
and `DS_UNEXPECTED_TYPE`. For example, 

```cpp
#define DS_EXPECTED_TYPE MyNamespace::MyExpected        //MyNamespace::MyExpected<T, E>
#define DS_UNEXPECTED_TYPE MyNamespace::MyUnexpected    //MyNamespace::MyUnexpected<E>
```

---

## Usage

### Type Definitions

The following types are defined
```cpp
namespace DS
{
    struct ErrorTrace
    {
        std::string Message;
        std::vector<TraceElement> Stack;
        int ErrorCode;
        ...
        operator std::string() const;
        std::string ToString() const;
    };
    
    struct Result : public expected<T, DS::ErrorTrace>
    {
        inline bool HasValue();                     //Does it have a value?
        inline const T& Value();                    //Get value without checking
        inline const DS::ErrorTrace& Error();       //Get error without checking
        ...
    }
    struct Error : public unexpected<DS::ErrorTrace>;
}
```

### Function Declaration that uses DS::Result
```cpp
DS::Result<int> MyFunction(...);
```

### Creates An Error Trace With Message (and with error code)
- `DS::Error DS_ERROR_MSG(const std::string& msg)`
- `DS::Error DS_ERROR_MSG_EC(const std::string& msg, int errorCode)`

```cpp
DS::Result<int> MyFunction()
{
    ...
    if(...)
        return DS_ERROR_MSG("Failed");
    int errorCode = 1;
    if(...)
        return DS_ERROR_MSG_EC("Failed", errorCode);
    ...
    return 0;
}
```

When trying to convert a value type to string, the `DS_STR(val)` macro can be used to do so.

In order to be able to be converted to string, it needs to have one of the following properties:
- Convertible with `std::to_string()`
- Is a pointer
- Constructible with `std::string` constructor
- Castable to `std::string`

```cpp
DS::Result<int> MyFunction()
{
    int myVal = 5;
    return DS_ERROR_MSG("Failed" + DS_STR(myVal));
}
```

### Appending Error Trace
- `DS::Error& DS_APPEND_TRACE(DS::Error& error)`

```cpp
DS::Result<int> MyFunction()
{
    int myVal = 5;
    return DS_ERROR_MSG("Failed" + DS_STR(myVal));
}

#include <iostream>
int main()
{
    DS::Result<int> res = MyFunction();
    if(!res.HasValue())
    {
        DS_APPEND_TRACE(res.Error());       //Add this line to error trace
        std::cout << res.Error().ToString() << std::endl;
        //Or
        //std::cout << DS_APPEND_TRACE(res.Error()).ToString() << std::endl;
        return 1;
    }
    return 0;
}
```

### Return If Assertion Failed
- `DS_ASSERT_TRUE(op)`
- `DS_ASSERT_FALSE(op)`
- `DS_ASSERT_EQ(op, assertValue)`
- `DS_ASSERT_NOT_EQ(op, assertValue)`
- `DS_ASSERT_GT(op, assertValue)`
- `DS_ASSERT_GT_EQ(op, assertValue)`
- `DS_ASSERT_LT(op, assertValue)`
- `DS_ASSERT_LT_EQ(op, assertValue)`

This assertion is run for both release and debug mode.

Error code variants:
- `DS_ASSERT_TRUE_EC(op, errorCode)`
- `DS_ASSERT_FALSE_EC(op, errorCode)`
- `DS_ASSERT_EQ_EC(op, assertValue, errorCode)`
- `DS_ASSERT_NOT_EQ_EC(op, assertValue, errorCode)`
- `DS_ASSERT_GT_EC(op, assertValue, errorCode)`
- `DS_ASSERT_GT_EQ_EC(op, assertValue, errorCode)`
- `DS_ASSERT_LT_EC(op, assertValue, errorCode)`
- `DS_ASSERT_LT_EQ_EC(op, assertValue, errorCode)`

When the assertion failed, the message will contain the string form for `op` and `assertValue` using
`DS_STR` macro. Use `DS_ASSERT_TRUE` instead to perform assertion if the type cannot be converted to 
string.

```cpp
{
    int myVal = 5;
    DS_ASSERT_EQ(myVal, 5);     //Pass
    DS_ASSERT_FALSE(true);      //Fail
}
```

### Assigning Value From A Result. Return Error If Failed.
- `DS_TRY()`: will return the error if failed. Same as `DS_VALUE_OR(); DS_CHECK_PREV()`
- `DS_TRY_ACT(failedActions)`: will execute `failedActions` if failed. In `failedActions`, 
    the `DS::ErrorTrace` being returned is accessible with `DS_TMP_ERROR` macro.

> [!CAUTION]
> **Do not omit curly braces when using this macro**
> **The macros only works with assignment, i.e. `<var> = <Result>.DS_TRY();`**

```cpp
DS::Result<int> MyFunction();

DS::Result<int> MyFunction2()
{
    //Will not continue if `MyFunction()` failed
    int myInt = MyFunction().DS_TRY();
    ...
    return 0;
}

bool MyFunction2()
{
    int myInt = MyFunction().DS_TRY_ACT(//Failed actions:
                                        std::cout << DS_TMP_ERROR.ToString() << std::endl; 
                                        return false);
    ...
    int myErrorCode = 0;
    myInt = MyFunction().DS_TRY_ACT(//Or accessing the error code:
                                    myErrorCode = DS_TMP_ERROR.ErrorCode;
                                    ...
                                    return false);
    ...
    //Or simply returning a boolean
    if(...)
    {
        myInt = MyFunction().DS_TRY_ACT(return false);
    }
    
    //DON'T DO THIS. The `DS_TRY_ACT` macro evaluates to more than 1 statement
    //if(...)
    //    myInt = MyFunction().DS_TRY_ACT(return false);
    ...
    return true;
}
```

If for any reason, you cannot use the assignment to extract the value, continue below.

### Getting Result Value And Act On Failure

There are 2 part when trying to get a value from a ds result object. First is getting the value 
itself and the second part is what to do when there's a failure.

- `DS_VALUE_OR()`: Returns the value if the result contains any, if not it will register the error
and return the default value. No restrictions on how to use this.
- `DS_CHECK_PREV()`: Checks any registered error with `DS_VALUE_OR()` and return the first 
error as a result for the current function. Otherwise continues execution. This also clears any 
registered error.
- `DS_CHECK_PREV_ACT(failedActions)`: Same as `DS_CHECK_PREV()` except performs the failed action(s)
instead of returning the error. `DS::ErrorTrace` is accessible with `DS_TMP_ERROR` macro inside the
`failedActions`.

```cpp
DS::Result<int> MyFunction(int);

DS::Result<int> MyFunction2()
{
    int myInt = MyFunction(1).DS_VALUE_OR();
    if(...)
        myInt = MyFunction(2).DS_VALUE_OR();    //Okay
    DS_CHECK_PREV();    //Will not continue if any of the previous `MyFunction()` calls failed.
                        //Returns the first error.
    ...
    return 0;
}

```

> [!CAUTION]
> **`DS_CHECK_PREV()` (and `DS_CHECK_PREV_ACT(...)`) must be used within the same translation unit.**
> i.e. `DS_CHECK_PREV()` must be used within the same `.cpp` file for checking any registered error 
> by `DS_VALUE_OR()`.

### Examples

See `FunctionWithAssert()` in `Examples/ExampleCommon.cpp` and `Examples/TryExamples.cpp` for all the 
usage examples and example outputs.
