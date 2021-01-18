#ifndef UNITTEST_USERINPUT_MOCKS_H
#define UNITTEST_USERINPUT_MOCKS_H

#include <gmock/gmock.h>
#include "UserInput/UserInput_interface.h"

class Mock_UserInput : public UserInput_interface
{
    MOCK_METHOD(eUserRequest, getUserRequest, (void), (override));
    MOCK_METHOD(void, userinputServiceIsr, (void), (override));
};

#endif // UNITTEST_USERINPUT_MOCKS_H