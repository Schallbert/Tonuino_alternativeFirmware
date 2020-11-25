#ifndef UNITTEST_MP3PLAYERCONTROL_MOCKS_H
#define UNITTEST_MP3PLAYERCONTROL_MOCKS_H

#include <gmock/gmock.h>
#include "../Mp3/Mp3Control/Mp3Control_interface.h"
class Mock_Mp3Control : public Mp3Control_interface
{
public:
    MOCK_METHOD(void, loop, (), (override, const));
    MOCK_METHOD(void, volumeUp, (), (override, const));
    MOCK_METHOD(void, volumeDown, (), (override, const));
    MOCK_METHOD(void, nextTrack, (), (override, const));
    MOCK_METHOD(void, prevTrack, (), (override, const));
    MOCK_METHOD(void, play, (), (override, const));
    MOCK_METHOD(void, pause, (), (override, const));
    MOCK_METHOD(void, togglePlayPause, (), (override, const));
    MOCK_METHOD(void, help, (), (override, const));
};

#endif // UNITTEST_MP3PLAYERCONTROL_MOCKS_H