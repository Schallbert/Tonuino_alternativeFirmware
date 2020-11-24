#ifndef UNITTEST_MP3PLAYERCONTROL_MOCKS_H
#define UNITTEST_MP3PLAYERCONTROL_MOCKS_H

#include <gmock/gmock.h>
#include "../Mp3/Mp3Control/Mp3Control_interface.h"
class Mock_Mp3Control : public Mp3Control_interface
{
public:
    MOCK_METHOD(void, loop, (), (override));
    MOCK_METHOD(void, volume_up, (), (override));
    MOCK_METHOD(void, volume_down, (), (override));
    MOCK_METHOD(void, next_track, (), (override));
    MOCK_METHOD(void, prev_track, (), (override));
    MOCK_METHOD(void, play, (), (override));
    MOCK_METHOD(void, pause, (), (override));
    MOCK_METHOD(void, togglePlayPause, (), (override));
};

#endif // UNITTEST_MP3PLAYERCONTROL_MOCKS_H