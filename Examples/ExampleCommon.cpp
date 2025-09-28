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
    DS_ASSERT_NOT_EQ(testVar, 0);
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

bool ReturnBool(bool returnVal)
{
    if(true)
        return returnVal;
    
    return !returnVal;
}

int ReturnInt(int returnVal)
{
    if(true)
        return returnVal;
    
    return returnVal - 5;
}

DS::Result<void> AssertExample(int assertIndex)
{
    //Positive
    if(assertIndex == 0)
    {
        DS_ASSERT_TRUE(ReturnBool(true));
        DS_ASSERT_FALSE(ReturnBool(false));
        DS_ASSERT_EQ(ReturnInt(5), 5);
        DS_ASSERT_NOT_EQ(ReturnInt(5), 4);
        DS_ASSERT_GT(ReturnInt(7), 6);
        DS_ASSERT_GT_EQ(ReturnInt(7), 6);
        DS_ASSERT_LT(ReturnInt(3), 4);
        DS_ASSERT_LT_EQ(ReturnInt(3), 4);
    }
    //Error cases
    else
    {
        switch(assertIndex)
        {
            case 1:
                DS_ASSERT_TRUE(ReturnBool(false));
                break;
            case 2:
                DS_ASSERT_FALSE(ReturnBool(true));
                break;
            case 3:
                DS_ASSERT_EQ(ReturnInt(5), 4);
                break;
            case 4:
                DS_ASSERT_NOT_EQ(ReturnInt(5), 5);
                break;
            case 5:
                DS_ASSERT_GT(ReturnInt(5), 6);
                break;
            case 6:
                DS_ASSERT_GT_EQ(ReturnInt(5), 6);
                break;
            case 7:
                DS_ASSERT_LT(ReturnInt(5), 4);
                break;
            case 8:
                DS_ASSERT_LT_EQ(ReturnInt(5), 4);
                break;
        }
    }
    
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
    
    for(int i = 0; i < 9; ++i)
    {
        voidResult = AssertExample(i);
        if(!voidResult.has_value())
            resultString += DS_APPEND_TRACE(voidResult.error()).ToString() + "\n---------\n";
    }
    
    std::cout << resultString << std::endl;
    
    std::string expectedResultString = R"(Error:
  Expression "0 != 0" has failed.

Stack trace:
  at ExampleCommon.cpp:15 in FunctionWithAssert()
  at ExampleCommon.cpp:126 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:22 in FunctionWithCheck()
  at ExampleCommon.cpp:132 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:28 in FunctionWithUnwrapDecl()
  at ExampleCommon.cpp:136 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:36 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:140 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:36 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:43 in FunctionWithUnwrapVoid()
  at ExampleCommon.cpp:144 in main()
---------
Error:
  Expression "0 == 1" has failed.

Stack trace:
  at ExampleCommon.cpp:83 in AssertExample()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "1 == 0" has failed.

Stack trace:
  at ExampleCommon.cpp:86 in AssertExample()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "5 == 4" has failed.

Stack trace:
  at ExampleCommon.cpp:89 in AssertExample()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "5 != 5" has failed.

Stack trace:
  at ExampleCommon.cpp:92 in AssertExample()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "5 > 6" has failed.

Stack trace:
  at ExampleCommon.cpp:95 in AssertExample()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "5 >= 6" has failed.

Stack trace:
  at ExampleCommon.cpp:98 in AssertExample()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "5 < 4" has failed.

Stack trace:
  at ExampleCommon.cpp:101 in AssertExample()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "5 <= 4" has failed.

Stack trace:
  at ExampleCommon.cpp:104 in AssertExample()
  at ExampleCommon.cpp:150 in main()
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
