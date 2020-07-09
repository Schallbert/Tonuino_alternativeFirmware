#ifndef DFMINIMP3_INTERFACE_H
#define DFMINIMP3_INTERFACE_H
#include <Arduino.h>
#include <DFMiniMp3.h>

// Wrapper class to separate dependency on dfMini Mp3 player from utilizing classes.
// Enabler for mocks.
class DfMiniMp3_interface
{
public:
    virtual ~DfMiniMp3_interface(){};

public:
    virtual void begin() = 0;
    virtual void setEq(DfMp3_Eq eq) = 0;
    virtual void setVolume(uint8_t volume) = 0;
    virtual void loop() = 0;
    virtual uint8_t getVolume() = 0;
    virtual void increaseVolume() = 0;
    virtual void decreaseVolume() = 0;
    virtual void pause() = 0;
    virtual void start() = 0; //can only restart playback if a valid folder is existing
    virtual void stop() = 0;
    virtual void playFolderTrack(uint8_t folderId, uint8_t trackId) = 0;
    virtual void playAdvertisement(uint16_t trackId) = 0;
    virtual uint8_t getFolderTrackCount(uint8_t folderID) = 0;
};
#endif // DFMINIMP3_INTERFACE_H