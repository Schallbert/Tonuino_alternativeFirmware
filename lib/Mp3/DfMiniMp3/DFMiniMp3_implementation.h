#ifndef DFMINIMP3_IMPLEMENTATION_H
#define DFMINIMP3_IMPLEMENTATION_H
#include "DFMiniMp3.h"
#include "../Mp3/DFMiniMp3/DFMiniMp3_interface.h"
#include "../MessageHandler/MessageHandler_interface.h"
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
    static inline const char *toString(Mp3Notify::eDfMiniNotify value)
    {
#if DEBUGSERIAL
        static const char *NOTIFY_STRING[] = {
            nullptr,
            "finished playing track",
            "SD card online",
            "SD card inserted",
            "SD Card removed",
            "Com Error"};
        return NOTIFY_STRING[value];
#endif
        return nullptr;
    };

private:
    // Disallow creating an instance of this object
    Mp3Notify(){};

public:
    static void OnError(uint16_t errorCode)
    {
        setMessage(playerError);
    };

    static void OnPlayFinished(DfMp3_PlaySources src, uint16_t track)
    {
        setMessage(playFinished);
    };

    static void OnPlaySourceOnline(uint16_t code)
    {
        setMessage(playSourceOnline);
    };

    static void OnPlaySourceInserted(uint16_t code)
    {
        setMessage(playSourceInserted);
    };

    static void OnPlaySourceRemoved(uint16_t code)
    {
        setMessage(playSourceRemoved);
    };

    static eDfMiniNotify getMessage()
    {
        return setMessage(noMessage);
    };

    static const char *messageToString()
    {
        return toString(getMessage());
    };

    static eDfMiniNotify setMessage(eDfMiniNotify incomingMessage)
    {
        static eDfMiniNotify m_eMessage = noMessage;
        if (incomingMessage != noMessage)
        {
            m_eMessage = incomingMessage;
        }
        return m_eMessage;
    };
};

// Wrapper class to interface DfMiniMp3 hardware
class DfMini : public DfMiniMp3_interface
{
public:
    DfMini(Arduino_interface_pins *pArduinoPins,
           MessageHander_interface *pMessageHandler) : m_pArduinoPins(pArduinoPins),
                                                       m_pMessageHandler(pMessageHandler)
    {
        m_dfMiniMp3.begin(); // init serial and start DfMiniMp3 module
        m_dfMiniMp3.loop();
    };

    ~DfMini(){};

    void loop()
    {
        m_dfMiniMp3.loop();
        sendMessage();
    };

    void setEq(eMp3Eq eq)
    {
        m_dfMiniMp3.setEq((DfMp3_Eq)eq);
        m_dfMiniMp3.loop();
    };

    void setVolume(uint8_t volume)
    {
        m_dfMiniMp3.setVolume(volume);
        m_dfMiniMp3.loop();
    };

    uint8_t getVolume()
    {
        return m_dfMiniMp3.getVolume();
        m_dfMiniMp3.loop();
    };

    void increaseVolume()
    {
        m_dfMiniMp3.increaseVolume();
        m_dfMiniMp3.loop();
    };

    void decreaseVolume()
    {
        m_dfMiniMp3.decreaseVolume();
        m_dfMiniMp3.loop();
    };

    void pause()
    {
        m_dfMiniMp3.pause();
        m_dfMiniMp3.loop();
    };

    void start()
    {
        m_dfMiniMp3.start();
        m_dfMiniMp3.loop();
    };

    void stop()
    {
        m_dfMiniMp3.stop();
        m_dfMiniMp3.loop();
    };

    void playFolderTrack(uint8_t folderId, uint8_t trackId)
    {
        m_dfMiniMp3.playFolderTrack(folderId, trackId);
        m_dfMiniMp3.loop();
    };

    void playAdvertisement(uint16_t trackId)
    {
        m_dfMiniMp3.playAdvertisement(trackId);
        m_dfMiniMp3.loop();
    };

    uint8_t getFolderTrackCount(uint8_t folderId)
    {
        m_dfMiniMp3.loop();
        return static_cast<uint8_t>(m_dfMiniMp3.getFolderTrackCount(static_cast<uint16_t>(folderId)));
    };

    bool isTrackFinished() const override
    {
        return (Mp3Notify::getMessage() == Mp3Notify::playFinished);
    };

    bool isPlaying() const override
    {
        return !(m_pArduinoPins->digital_read(DFMINI_PIN_ISIDLE));
    }

private:
    void sendMessage()
    {
        static const char *message = nullptr;
        const char *newMessage = Mp3Notify::messageToString();
        if (message != newMessage)
        {
            m_pMessageHandler->printMessage(message);
            message = newMessage;
        }
    }

private:
    Arduino_interface_pins *m_pArduinoPins{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};
    // Solution for constructor error found here:
    //https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX)
    //because compiler interprets this as a class method call
    SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)};
    DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{
        DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
};
#endif // DFMINIMP3_IMPLEMENTATION_H