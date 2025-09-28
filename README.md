# Dead Simple Result With Error Tracing

A single header only simple C++11 struct for allowing golang/rust like error handling and tracing
with expected like container

Supports [tl::expected](https://github.com/TartanLlama/expected), 
[expected lite](https://github.com/martinmoene/expected-lite.git),
[std expected](https://cppreference.com/w/cpp/header/expected.html) or custom expected like container.

Just add `Include` to your project include path and do `#include "DSResult/DSResult.hpp"`.

By default it uses `tl::expected`, if nothing is instructed. Define the following macro to choose 
a different backend

```cpp
#define DS_USE_TL_EXPECTED 1
#define DS_USE_EXPECTED_LITE 1
#define DS_USE_STD_EXPECTED 1
#define DS_USE_CUSTOM_EXPECTED 1
```

You can integrate DSResult to your cmake project easily with
```cmake
add_subdirectory("path/to/DSResult")
add_subdirectory("path/to/expected") # Or other expected libraries
target_link_libraries(yourTarget PRIVATE DSResult expected)
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
    template<typename T>
    using Result = DS_EXPECTED_TYPE<T, DS::ErrorTrace>;
    using Error = DS_UNEXPECTED_TYPE<DS::ErrorTrace>;
}
```

See the respective expected and unexpected type on how to use them, but mainly the following
- `bool DS::Result<T>::has_value()`
- `T& DS::Result<T>::value()`
- `DS::Error DS::Result<T>::error()`

### Function Declaration that uses DS::Result
```cpp
DS::Result<int> MyFunction(...);
```

### Returning an error message
```cpp
int myValue;
return DS_ERROR_MSG("Something wrong: " + DS_STR(myValue));
```

### Returning an error if assertion fails
```cpp
std::vector<int> myData;
DS_ASSERT_FALSE(myData.empty());  //Returns DS::Error if `myData.empty()` is true

//DS_ASSERT_TRUE(expression);
//DS_ASSERT_FALSE(expression);
//DS_ASSERT_NOT_EQ(expression, compareValue);
//DS_ASSERT_EQ(expression, compareValue);
//DS_ASSERT_GT(expression, compareValue);
//DS_ASSERT_GT_EQ(expression, compareValue);
//DS_ASSERT_LT(expression, compareValue);
//DS_ASSERT_LT_EQ(expression, compareValue);
```

### Unwrapping to a variable (Or append backtrace and return error if failed)
```cpp
DS::Result<void> MyVoidFunction()
{
    //Declaring a variable and unwrap to it
    DS_UNWRAP_DECL(int myInt, MyFunction());
    
    //Unwrap and assigning to existing variable
    DS_UNWRAP_ASSIGN(myInt, MyFunction());
    return {};
}

{
    //Unwrapping a void function
    DS_UNWRAP_VOID(MyVoidFunction());
}
```

### Get the error trace if a function failed
```cpp
#include <iostream>
int main()
{
    DS::Result<void> result = MyFunction();
    if(!result.has_value())
    {
        std::cout << result.error().ToString() << std::endl;
        //Error:
        //  ...
        //
        //Stack trace
        //  at ...
        //  at ...
        //  ...
        
        //or this to show the current line as final stack trace
        std::cout << DS_APPEND_TRACE(result.error()).ToString() << std::endl;
        return 1;
    }
    return 0;
}
```
