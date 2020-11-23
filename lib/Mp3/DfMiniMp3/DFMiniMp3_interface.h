#ifndef DFMINIMP3_INTERFACE_H
#define DFMINIMP3_INTERFACE_H

#include "../Arduino/Arduino_interface/Arduino_types.h"

// Wrapper class to separate dependency on dfMini Mp3 player from utilizing classes.
// Enabler for mocks.
class DfMiniMp3_interface
{
public:
    enum eMp3Eq
    {
        DfMp3_Eq_Normal,
        DfMp3_Eq_Pop,
        DfMp3_Eq_Rock,
        DfMp3_Eq_Jazz,
        DfMp3_Eq_Classic,
        DfMp3_Eq_Bass
    };

public:
    virtual ~DfMiniMp3_interface(){};

public:
    virtual void loop() = 0;
    virtual void setEq(eMp3Eq eq) = 0;
    virtual void setVolume(uint8_t volume) = 0;
    virtual uint8_t getVolume() = 0;
    virtual void increaseVolume() = 0;
    virtual void decreaseVolume() = 0;
    virtual void pause() = 0;
    virtual void start() = 0; //can only restart playback if a valid folder is existing
    virtual void stop() = 0;
    // plays specified track on sd:/<folder_number>
    virtual void playFolderTrack(uint8_t folderId, uint8_t trackId) = 0;
    // plays specified file on sd:/advert/####fileId
    virtual void playAdvertisement(uint16_t trackId) = 0;
    virtual uint8_t getFolderTrackCount(uint8_t folderId) = 0;
    virtual bool checkTrackFinished() = 0;
    virtual const char *getPlayerNotification() = 0;
};
#endif // DFMINIMP3_INTERFACE_H