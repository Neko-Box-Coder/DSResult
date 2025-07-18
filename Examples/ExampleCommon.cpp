#include "DSResult/DSResult.hpp"

#include <iostream>
#include <string>

DS::Result<int> FunctionWithMsg()
{
    int myValue = 12345;
    return DS::Error(DS_ERROR_MSG("Something wrong: " + DS_STR(myValue)));
}

DS::Result<int> FunctionWithAssert(int testVar)
{
    DS_ASSERT_RETURN(testVar != 0);
    return testVar * 2;
}

DS::Result<void> FunctionAppendTrace()
{
    DS::Result<int> functionResult = FunctionWithMsg();
    DS_CHECKED_RETURN(functionResult);
    return {};
}

int main()
{
    std::string resultString;
    int myInt = 0;
    (void)myInt;
    
    DS::Result<int> intResult = FunctionWithAssert(2);
    if(!intResult.has_value())
        resultString += DS_APPEND_TRACE(intResult.error()).ToString() + "\n";
    else
        myInt = intResult.value();
    
    intResult = FunctionWithAssert(0);
    if(!intResult.has_value())
        resultString += DS_APPEND_TRACE(intResult.error()).ToString() + "\n\n";
    else
        myInt = intResult.value();

    DS::Result<void> voidResult = FunctionAppendTrace();
    if(!voidResult.has_value())
    {
        DS::ErrorTrace errorTrace = DS_APPEND_TRACE(voidResult.error());
        resultString += errorTrace.ToString();
    }
    
    std::cout << resultString << std::endl;
    
    std::string expectedResultString = R"(Error:
  Expression "testVar != 0" has failed.

Stack trace:
  at ExampleCommon.cpp:14 in FunctionWithAssert()
  at ExampleCommon.cpp:39 in main()

Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:9 in FunctionWithMsg()
  at ExampleCommon.cpp:21 in FunctionAppendTrace()
  at ExampleCommon.cpp:46 in main())";
    
    if(resultString != expectedResultString)
    {
        std::cout << "Failed" << std::endl;
        std::cout << expectedResultString << std::endl;
        return 1;
    }
    
    return 0;
}
