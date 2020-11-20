#ifndef UNITTEST_MP3PLAYERCONTROL_MOCKS_H
#define UNITTEST_MP3PLAYERCONTROL_MOCKS_H

#include <gmock/gmock.h>
#include "../Mp3/Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"
class Mock_Mp3PlayerControl : public Mp3PlayerControl_interface
{
public:
    MOCK_METHOD(void, loop, (), (override));
    MOCK_METHOD(void, volume_up, (), (override));
    MOCK_METHOD(void, volume_down, (), (override));
    MOCK_METHOD(void, next_track, (), (override));
    MOCK_METHOD(void, prev_track, (), (override));
    MOCK_METHOD(void, play_pause, (), (override));
    MOCK_METHOD(bool, is_playing, (), (const, override));
    MOCK_METHOD(void, play_folder, (Folder *m_pCurrentFolder), (override));
    MOCK_METHOD(void, play_prompt, (const VoicePrompt &prompt), (const, override));
    MOCK_METHOD(uint8_t, get_trackCount_of_folder, (uint8_t folderId), (override));
};

#endif // UNITTEST_MP3PLAYERCONTROL_MOCKS_H