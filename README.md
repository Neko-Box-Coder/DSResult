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
        inline T DefaultOr();                       //Alias for `value_or(T())`
        inline bool HasValue();                     //Alias for `has_value()`
        inline const T& Value();                    //Alias for `value()`
        inline const DS::ErrorTrace& Error();       //Alias for `error()`
        ...
    }
    struct Error : public unexpected<DS::ErrorTrace>;
}
```

### Function Declaration that uses DS::Result
```cpp
DS::Result<int> MyFunction(...);
```

### Creates An Error Trace With Message
- `DS_ERROR_MSG(msg)`
- `DS_ERROR_MSG_EC(msg, errorCode)`

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

### Getting Result Value And Act On Failure

There are 2 part when trying to get a value from a ds result object. First is getting the value 
itself and the second part is what to do when there's a failure.

- `DS_VALUE_OR()`: returns the result value or register error if any and return default value. 
It is used as a chained function call.
- `DS_CHECK_PREV()`: Checks any previous error registered with `DS_VALUE_OR()` and return the first 
error as a result for the current function. Otherwise continues execution. This also clears any 
registered error.
- `DS_CHECK_PREV_ACT(failedActions)`: Same as `DS_CHECK_PREV()` except performs the failed action(s)
instead of returning the error. `DS::ErrorTrace` is accessible with `DS_TMP_ERROR` macro inside the
`failedActions`.

```cpp
DS::Result<int> MyFunction();

DS::Result<int> MyFunction2()
{
    int myInt = MyFunction().DS_VALUE_OR();
    myInt = MyFunction().DS_VALUE_OR();
    DS_CHECK_PREV();    //Will not continue if any of the previous `MyFunction()` calls failed.
                        //Returns the first error.
    ...
    return 0;
}

```


### Try To Assign Result Value From A Function
- `DS_TRY()`: will return the error if failed. Same as `DS_VALUE_OR(); DS_CHECK_PREV()`
- `DS_TRY_ACT(failedActions)`: will execute `failedActions` if failed. Same as `DS_VALUE_OR(); 
DS_CHECK_PREV_ACT(failedActions)`

`DS::ErrorTrace` is accessible with `DS_TMP_ERROR` macro.

**Do not omit curly braces when using this macro**

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
                                    myErrorCode = DS_TMP_ERROR.ErrorCode);
    ...
    //Do this
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

## Advance Usages

### Unwrapping to a variable

The difference between unwrapping macros and try macros is that the unwrapping macros will return or 
perform failed actions before the assignment. Whereas the try macros will assign a default value if 
failed, before returning or performing the failed actions.

The other difference is that most of the unwrapping macros can be used with omitted curly braces.

- `DS_UNWRAP_DECL(unwrapVar, op)`: **Do not omit curly braces when using this macro**
- `DS_UNWRAP_ASSIGN(unwrapVar, op)`
- `DS_UNWRAP_VOID(op)`
- `DS_UNWRAP_DECL_ACT(unwrapVar, op, failedActions)`: **Do not omit curly braces when using this macro**
- `DS_UNWRAP_ASSIGN_ACT(unwrapVar, op, failedActions)`
- `DS_UNWRAP_VOID_ACT(op, failedActions)`

```cpp
DS::Result<int> MyFunction();

DS::Result<void> MyVoidFunction()
{
    //Declaring a variable and unwrap to it
    DS_UNWRAP_DECL(int myInt, MyFunction());
    
    //Unwrap and assigning to existing variable
    DS_UNWRAP_ASSIGN(myInt, MyFunction());
    
    //Only check success and print error if failed
    DS_UNWRAP_VOID_ACT(MyFunction(), std::cout << DS_TMP_ERROR.ToString() << std::endl);
    return {};
}

{
    //Unwrapping a void function
    DS_UNWRAP_VOID(MyVoidFunction());
}
```

### Examples

See `./Examples/ExampleCommon.cpp` for all the usage examples and example outputs
