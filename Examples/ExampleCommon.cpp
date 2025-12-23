#include "TryExamples.hpp"
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
    
    resultString += "1:\n";
    int intResult = FunctionWithAssert(2).DS_TRY_ACT(APPEND_ERROR());   //Pass
    (void)intResult;
    resultString += "2:\n";
    intResult = FunctionWithAssert(0).DS_TRY_ACT(APPEND_ERROR());       //Fail
    resultString += "3:\n";
    FunctionWithUnwrapDecl().DS_TRY_ACT(APPEND_ERROR());                //Fail
    resultString += "4:\n";
    FunctionWithUnwrapAssign().DS_TRY_ACT(APPEND_ERROR());              //Fail
    resultString += "5:\n";
    FunctionWithUnwrapVoid().DS_TRY_ACT(APPEND_ERROR());                //Fail
    resultString += "6:\n";
    FunctionWithTry().DS_TRY_ACT(APPEND_ERROR());                       //Fail
    resultString += "7:\n";
    resultString += std::to_string(FunctionWithTryAct()) + "\n";        //Fail with "0"
    resultString += "8:\n";
    for(int i = 0; i < 9; ++i)
    {
        resultString += "i == " + std::to_string(i) + ":\n";
        AssertExample(i).DS_TRY_ACT(APPEND_ERROR());                    //Pass first, fail rest
    }
    
    std::cout << resultString << std::endl;
    
    std::string expectedResultString = R"(1:
2:
Error:
  Expression "0 != 0" has failed.
Error Code: 5

Stack trace:
  at ExampleCommon.cpp:17 in FunctionWithAssert()
  at ExampleCommon.cpp:137 in main()
---------
3:
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:11 in FunctionWithMsg()
  at ExampleCommon.cpp:23 in FunctionWithUnwrapDecl()
  at ExampleCommon.cpp:139 in main()
---------
4:
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:11 in FunctionWithMsg()
  at ExampleCommon.cpp:31 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:141 in main()
---------
5:
Error:
  Something wrong: 12345

Stack trace:
  at ExampleCommon.cpp:11 in FunctionWithMsg()
  at ExampleCommon.cpp:31 in FunctionWithUnwrapAssign()
  at ExampleCommon.cpp:38 in FunctionWithUnwrapVoid()
  at ExampleCommon.cpp:143 in main()
---------
6:
Error:
  Something wrong: 12345

Stack trace:
  at TryExamples.cpp:8 in FunctionWithMsg()
  at TryExamples.cpp:14 in FunctionWithTry()
  at ExampleCommon.cpp:145 in main()
---------
7:
0
8:
i == 0:
i == 1:
Error:
  Expression "0 == 1" has failed.

Stack trace:
  at ExampleCommon.cpp:95 in AssertExample()
  at ExampleCommon.cpp:152 in main()
---------
i == 2:
Error:
  Expression "1 == 0" has failed.

Stack trace:
  at ExampleCommon.cpp:98 in AssertExample()
  at ExampleCommon.cpp:152 in main()
---------
i == 3:
Error:
  Expression "5 == 4" has failed.

Stack trace:
  at ExampleCommon.cpp:101 in AssertExample()
  at ExampleCommon.cpp:152 in main()
---------
i == 4:
Error:
  Expression "5 != 5" has failed.

Stack trace:
  at ExampleCommon.cpp:104 in AssertExample()
  at ExampleCommon.cpp:152 in main()
---------
i == 5:
Error:
  Expression "5 > 6" has failed.

Stack trace:
  at ExampleCommon.cpp:107 in AssertExample()
  at ExampleCommon.cpp:152 in main()
---------
i == 6:
Error:
  Expression "5 >= 6" has failed.

Stack trace:
  at ExampleCommon.cpp:110 in AssertExample()
  at ExampleCommon.cpp:152 in main()
---------
i == 7:
Error:
  Expression "5 < 4" has failed.

Stack trace:
  at ExampleCommon.cpp:113 in AssertExample()
  at ExampleCommon.cpp:152 in main()
---------
i == 8:
Error:
  Expression "5 <= 4" has failed.

Stack trace:
  at ExampleCommon.cpp:116 in AssertExample()
  at ExampleCommon.cpp:152 in main()
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
