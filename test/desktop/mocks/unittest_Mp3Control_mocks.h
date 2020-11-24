#ifndef UNITTEST_MP3PLAYERCONTROL_MOCKS_H
#define UNITTEST_MP3PLAYERCONTROL_MOCKS_H

#include <gmock/gmock.h>
#include "../Mp3/Mp3Control/Mp3Control_interface.h"
class Mock_Mp3Control : public Mp3Control_interface
{
public:
    MOCK_METHOD(void, loop, (), (override));
    MOCK_METHOD(void, volumeUp, (), (override));
    MOCK_METHOD(void, volumeDown, (), (override));
    MOCK_METHOD(void, nextTrack, (), (override));
    MOCK_METHOD(void, prevTrack, (), (override));
    MOCK_METHOD(void, play, (), (override));
    MOCK_METHOD(void, pause, (), (override));
    MOCK_METHOD(void, togglePlayPause, (), (override));
};

#endif // UNITTEST_MP3PLAYERCONTROL_MOCKS_H