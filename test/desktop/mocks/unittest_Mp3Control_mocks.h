#ifndef UNITTEST_MP3PLAYERCONTROL_MOCKS_H
#define UNITTEST_MP3PLAYERCONTROL_MOCKS_H

#include <gmock/gmock.h>

#include "../Mp3/Mp3Control/Mp3Control_interface.h"
#include "../UserInput/UserInput_interface/UserInput_interface.h"

class Mock_Mp3Control : public Mp3Control_interface
{
public:
    MOCK_METHOD(void, setUserInput, (UserInput::eUserRequest), (override));
    MOCK_METHOD(void, loop, (), (override));
};

#endif // UNITTEST_MP3PLAYERCONTROL_MOCKS_H