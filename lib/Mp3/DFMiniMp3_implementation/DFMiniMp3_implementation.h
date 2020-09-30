#ifndef DFMINIMP3_IMPLEMENTATION_H
#define DFMINIMP3_IMPLEMENTATION_H
#include <DFMiniMp3.h>
#include "../Mp3/DFMiniMp3_interface/DFMiniMp3_interface.h"
#include <Arduino_config.h>
#include <SoftwareSerial.h>

class Mp3Notify
{
    public:
     enum eDfMiniNotify
    {
        noMessage = 0,
        playFinished,
        playSourceOnline,
        playSourceInserted,
        playSourceRemoved,
        playerError
    };

private:
    // Disallow creating an instance of this object
    Mp3Notify() {}

public:
    static void OnError(uint16_t errorCode)
    {
        setMessage(playerError);
    }
    static void OnPlayFinished(DfMp3_PlaySources src, uint16_t track)
    {
        setMessage(playFinished);
    }
    static void OnPlaySourceOnline(uint16_t code)
    {
        setMessage(playSourceOnline);
    }
    static void OnPlaySourceInserted(uint16_t code)
    {
        setMessage(playSourceInserted);
    }
    static void OnPlaySourceRemoved(uint16_t code)
    {
        setMessage(playSourceRemoved);
    }
    // returns true if track has been set finished
    // to set state call method setFinished = true
    // to get state call method setFinished = false
    static eDfMiniNotify getMessage()
    {
        return setMessage(noMessage);
    }

private:
    // Had to solve this in a static method, a static class member variable would lead to linker errors. Couldn't inline.
    static eDfMiniNotify setMessage(eDfMiniNotify incomingMessage)
    {
        auto m_eMessage = noMessage;
        if (incomingMessage != noMessage)
        {
            m_eMessage = incomingMessage;
        }
        return m_eMessage;
    }
};

// Classes

// Wrapper class to interface DfMiniMp3 hardware
// No business logic in here!
class DfMini : public DfMiniMp3_interface
{
public:
    ~DfMini(){};

public:
    void begin() override { m_dfMiniMp3.begin(); };
    void setEq(eMp3Eq eq) override { m_dfMiniMp3.setEq((DfMp3_Eq)eq); };
    void setVolume(uint8_t volume) override { m_dfMiniMp3.setVolume(volume); };
    void loop() override { m_dfMiniMp3.loop(); };
    uint8_t getVolume() override { return m_dfMiniMp3.getVolume(); };
    void increaseVolume() override { m_dfMiniMp3.increaseVolume(); };
    void decreaseVolume() override { m_dfMiniMp3.decreaseVolume(); };
    void pause() override { m_dfMiniMp3.pause(); };
    void start() override { m_dfMiniMp3.start(); };
    void stop() override { m_dfMiniMp3.stop(); };
    void playFolderTrack(uint8_t folderId, uint8_t trackId) override
    {
        m_dfMiniMp3.playFolderTrack(folderId, trackId);
    };
    void playAdvertisement(uint16_t trackId) override
    {
        m_dfMiniMp3.playAdvertisement(trackId);
    };
    uint8_t getFolderTrackCount(uint8_t folderId) override
    {
        return static_cast<uint8_t>(m_dfMiniMp3.getFolderTrackCount(static_cast<uint16_t>(folderId)));
    };
    const char *getPlayerNotification() override
    {
        return stringFromDfMiniNotify(Mp3Notify::getMessage());
    };
    bool checkTrackFinished() override
    {
        return (Mp3Notify::getMessage() == Mp3Notify::playFinished);
    };

private:
    static inline const char *stringFromDfMiniNotify(Mp3Notify::eDfMiniNotify value)
    {
#if DEBUGSERIAL
        static const char *NOTIFY_STRING[] = {
            "No Message",
            "finished playing track",
            "SD card online",
            "SD card inserted",
            "SD Card removed",
            "Com Error"};

        return NOTIFY_STRING[value];
#endif
        return "";
    }

private:
    // Solution for constructor error found here:
    //https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX)
    //because compiler interprets this as a class method call
    SoftwareSerial m_Mp3SwSerial{
        SoftwareSerial(DFMINI_RX, DFMINI_TX)};
    DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{
        DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
};
#endif // DFMINIMP3_IMPLEMENTATION_H