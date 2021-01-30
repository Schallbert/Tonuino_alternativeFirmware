#ifndef UNITTEST_MP3PLAYERCONTROL_MOCKS_H
#define UNITTEST_MP3PLAYERCONTROL_MOCKS_H

#include <gmock/gmock.h>

#include "../Mp3/Mp3Control/Mp3Control_interface.h"
#include "../UserInput/UserInput/UserInput_interface.h"

class Folder;

class Mock_Mp3Control : public Mp3Control_interface
{
public:
    MOCK_METHOD(void, setUserInput, (Message::eMessageContent), (override));
    MOCK_METHOD(void, playFolder, (Folder &folder), (override));
    MOCK_METHOD(void, loop, (), (override));
};

#endif // UNITTEST_MP3PLAYERCONTROL_MOCKS_H