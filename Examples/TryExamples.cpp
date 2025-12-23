#include "TryExamples.hpp"

namespace
{
    DS::Result<int> FunctionWithMsg()
    {
        int myValue = 12345;
        return DS_ERROR_MSG("Something wrong: " + DS_STR(myValue));
    }
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
