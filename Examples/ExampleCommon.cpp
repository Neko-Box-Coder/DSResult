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
    int errorCode = 5;
    DS_ASSERT_NOT_EQ_EC(testVar, 0, errorCode);
    return testVar * 2;
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

DS::Result<int> FunctionWithTry()
{
    int resultInt = FunctionWithMsg().DS_TRY();
    return resultInt;
}

bool FunctionWithTryAct()
{
    int resultInt = FunctionWithMsg().DS_TRY_ACT(return false);
    return true;
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

        #if 0
        //For testing string conversion
        {
            struct ABC
            {
                int a = 1;
                //operator std::string() const { return ""; }
                bool operator ==(const ABC& other) const { return a == other.a; }
            };
            
            ABC a;
            ABC b;
            DS_ASSERT_EQ(a, b);
        }
        #endif
        
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
    
    #define APPEND_ERROR() \
        resultString += DS_APPEND_TRACE(DS_TMP_ERROR).ToString() + "\n---------\n";
    
    int intResult = FunctionWithAssert(2).DS_TRY_ACT(APPEND_ERROR());
    (void)intResult;
    intResult = FunctionWithAssert(0).DS_TRY_ACT(APPEND_ERROR());
    FunctionWithUnwrapDecl().DS_TRY_ACT(APPEND_ERROR());
    FunctionWithUnwrapAssign().DS_TRY_ACT(APPEND_ERROR());
    FunctionWithUnwrapVoid().DS_TRY_ACT(APPEND_ERROR());
    FunctionWithTry().DS_TRY_ACT(APPEND_ERROR());
    FunctionWithTryAct();
    
    for(int i = 0; i < 9; ++i)
    {
        AssertExample(i).DS_TRY_ACT(APPEND_ERROR());
    }
    
    std::cout << resultString << std::endl;
    
    std::string expectedResultString = R"(Error:
  Expression "0 != 0" has failed.
Error Code: 5

Stack trace:
  at ExampleCommon.cpp:16 in FunctionWithAssert()
  at ExampleCommon.cpp:146 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:22 in FunctionWithUnwrapDecl()
  at ExampleCommon.cpp:147 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:30 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:148 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:30 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:37 in FunctionWithUnwrapVoid()
  at ExampleCommon.cpp:149 in main()
---------
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:10 in FunctionWithMsg()
  at ExampleCommon.cpp:150 in main()
---------
Error:
  Expression "0 == 1" has failed.

Stack trace:
  at ExampleCommon.cpp:106 in AssertExample()
  at ExampleCommon.cpp:155 in main()
---------
Error:
  Expression "1 == 0" has failed.

Stack trace:
  at ExampleCommon.cpp:109 in AssertExample()
  at ExampleCommon.cpp:155 in main()
---------
Error:
  Expression "5 == 4" has failed.

Stack trace:
  at ExampleCommon.cpp:112 in AssertExample()
  at ExampleCommon.cpp:155 in main()
---------
Error:
  Expression "5 != 5" has failed.

Stack trace:
  at ExampleCommon.cpp:115 in AssertExample()
  at ExampleCommon.cpp:155 in main()
---------
Error:
  Expression "5 > 6" has failed.

Stack trace:
  at ExampleCommon.cpp:118 in AssertExample()
  at ExampleCommon.cpp:155 in main()
---------
Error:
  Expression "5 >= 6" has failed.

Stack trace:
  at ExampleCommon.cpp:121 in AssertExample()
  at ExampleCommon.cpp:155 in main()
---------
Error:
  Expression "5 < 4" has failed.

Stack trace:
  at ExampleCommon.cpp:124 in AssertExample()
  at ExampleCommon.cpp:155 in main()
---------
Error:
  Expression "5 <= 4" has failed.

Stack trace:
  at ExampleCommon.cpp:127 in AssertExample()
  at ExampleCommon.cpp:155 in main()
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
