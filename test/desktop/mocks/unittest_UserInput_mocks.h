#ifndef UNITTEST_USERINPUT_MOCKS_H
#define UNITTEST_USERINPUT_MOCKS_H

#include <gmock/gmock.h>
#include "UserInput/UserInput_interface.h"

class Mock_UserInput : public UserInput_interface
{
public:
    MOCK_METHOD(eUserRequest, getUserRequest, (), (override));
    MOCK_METHOD(void, userinputServiceIsr, (), (override));
};

#endif // UNITTEST_USERINPUT_MOCKS_H