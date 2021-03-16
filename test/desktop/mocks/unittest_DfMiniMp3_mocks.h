#ifndef UNITTEST_DFMINIMP3_MOCKS_H
#define UNITTEST_DFMINIMP3_MOCKS_H

#include <gmock/gmock.h>
#include "../Mp3/DFMiniMp3/DFMiniMp3_interface.h"

// Wrapper class to separate dependency on dfMini Mp3 player from utilizing classes.
// Enabler for mocks.
class Mock_DfMiniMp3 : public DfMiniMp3_interface
{
public:
    MOCK_METHOD(void, init, (), (override));
    MOCK_METHOD(void, setEq, (eMp3Eq eq), (override));
    MOCK_METHOD(void, setVolume, (uint8_t volume), (override));
    MOCK_METHOD(void, loop, (), (override)); // DOES LOOP REALLY WORK? WATCH OUT FOR STRANGE ERROR MESSAGES
    MOCK_METHOD(uint8_t, getVolume, (), (override));
    MOCK_METHOD(void, increaseVolume, (), (override));
    MOCK_METHOD(void, decreaseVolume, (), (override));
    MOCK_METHOD(void, pause, (), (override));
    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(void, stop, (), (override));
    MOCK_METHOD(void, playFolderTrack, (uint8_t folderId, uint8_t trackId), (override));
    MOCK_METHOD(void, playPrompt, (uint16_t trackId), (override));
    MOCK_METHOD(void, stopPrompt, (), (override));
    MOCK_METHOD(void, playAnnouncement, (uint16_t trackId), (override));
    MOCK_METHOD(uint8_t, getFolderTrackCount, (uint8_t folderId), (override));
    MOCK_METHOD(bool, isTrackFinished, (), (const, override));
    MOCK_METHOD(bool, isPlaying, (), (const, override));
    MOCK_METHOD(void, printStatus, (), (const, override));
};
#endif // UNITTEST_DFMINIMP3_MOCKS_H