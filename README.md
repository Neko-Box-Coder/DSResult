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

If you are using a custom expected like container, you need to define the macros `DS_EXPECTED_TYPE` 
and `DS_UNEXPECTED_TYPE`. For example, 

```cpp
#define DS_EXPECTED_TYPE MyNamespace::MyExpected        //MyNamespace::MyExpected<T, E>
#define DS_UNEXPECTED_TYPE MyNamespace::MyUnexpected    //MyNamespace::MyUnexpected<E>
```

---

## Usage

### Type Definitions
```cpp
template<typename T>
using Result = DS_EXPECTED_TYPE<T, DS::ErrorTrace>;
using Error = DS_UNEXPECTED_TYPE<DS::ErrorTrace>;
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
return DS::Error(DS_ERROR_MSG("Something wrong: " + DS_STR(myValue)));
```

### Return error if assertion fails
```cpp
std::vector<int> myData;
DS_ASSERT_RETURN(!myData.empty());  //Returns DS::Error if `myData.empty()` is true
```

### Check result, append to trace and return if there's an error
```cpp
DS::Result<void> MyVoidFunction()
{
    DS::Result<int> functionResult = MyFunction();
    DS_CHECKED_RETURN(functionResult);
    int myInt = functioonResult.value();
    return {};
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
        DS::ErrorTrace errorTrace = DS_APPEND_TRACE(result.error());
        //Error:
        //  ...
        //
        //Stack trace
        //  at ...
        //  at ...
        //  ...
        std::cout << errorTrace.ToString() << std::endl;
        return 1;
    }
    return 0;
}
```
