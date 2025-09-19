#include "DSResult/DSResult.hpp"

#include <iostream>
#include <string>


DS::Result<int> FunctionWithMsg()
{
    int myValue = 12345;
    return DS_ERROR_MSG("Something wrong: " + DS_STR(myValue));
}

DS::Result<int> FunctionWithAssert(int testVar)
{
    DS_ASSERT(testVar != 0);
    return testVar * 2;
}

DS::Result<void> FunctionWithCheck()
{
    DS::Result<int> functionResult = FunctionWithMsg();
    DS_CHECK(functionResult);
    return {};
}

DS::Result<void> FunctionWithUnwrapDecl()
{
    DS_UNWRAP_DECL(int resultInt, FunctionWithMsg());
    (void)resultInt;
    return {};
}

DS::Result<void> FunctionWithUnwrapAssign()
{
    int resultInt;
    DS_UNWRAP_ASSIGN(resultInt, FunctionWithMsg());
    (void)resultInt;
    return {};
}

DS::Result<void> FunctionWithUnwrapVoid()
{
    DS_UNWRAP_VOID(FunctionWithUnwrapAssign());
    return {};
}

int main()
{
    std::string resultString;
    int myInt = 0;
    (void)myInt;
    
    DS::Result<int> intResult = FunctionWithAssert(2);
    if(!intResult.has_value())
        resultString += DS_APPEND_TRACE(intResult.error()).ToString() + "\n---------\n";
    else
        myInt = intResult.value();
    
    intResult = FunctionWithAssert(0);
    if(!intResult.has_value())
        resultString += DS_APPEND_TRACE(intResult.error()).ToString() + "\n---------\n";
    else
        myInt = intResult.value();

    DS::Result<void> voidResult = FunctionWithCheck();
    if(!voidResult.has_value())
        resultString += DS_APPEND_TRACE(voidResult.error()).ToString() + "\n---------\n";
    
    voidResult = FunctionWithUnwrapDecl();
    if(!voidResult.has_value())
        resultString += DS_APPEND_TRACE(voidResult.error()).ToString() + "\n---------\n";
    
    voidResult = FunctionWithUnwrapAssign();
    if(!voidResult.has_value())
        resultString += DS_APPEND_TRACE(voidResult.error()).ToString() + "\n---------\n";
    
    voidResult = FunctionWithUnwrapVoid();
    if(!voidResult.has_value())
        resultString += DS_APPEND_TRACE(voidResult.error()).ToString() + "\n---------\n";
    
    std::cout << resultString << std::endl;
    
    std::string expectedResultString = R"(Error:
  Expression "testVar != 0" has failed.

Stack trace:
  at ExampleCommon.cpp:15 in FunctionWithAssert()
  at ExampleCommon.cpp:61 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:22 in FunctionWithCheck()
  at ExampleCommon.cpp:67 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:28 in FunctionWithUnwrapDecl()
  at ExampleCommon.cpp:71 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:36 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:75 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:36 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:43 in FunctionWithUnwrapVoid()
  at ExampleCommon.cpp:79 in main()
---------
)";

    if(resultString != expectedResultString)
    {
        std::cout << "\n\nFailed. Expected:" << std::endl;
        std::cout << expectedResultString << std::endl;
        
        if(resultString.size() != expectedResultString.size())
        {
            std::cout << "resultString.size(): " << resultString.size() << std::endl;
            std::cout << "expectedResultString.size(): " << expectedResultString.size() << std::endl;
            return 1;
        }
        
        for(int i = 0; i < (int)resultString.size(); ++i)
        {
            if(resultString[i] != expectedResultString[i])
            {
                std::cout << "Mismatch at index " << i << std::endl;
                std::cout << "resultString[i]: " << resultString[i] << std::endl;
                std::cout << "expectedResultString[i]: " << expectedResultString[i] << std::endl;
                return 1;
            }
        }
        
        return 1;
    }
    
    return 0;
}
