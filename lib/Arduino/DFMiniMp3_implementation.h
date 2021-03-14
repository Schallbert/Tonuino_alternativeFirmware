#ifndef DFMINIMP3_IMPLEMENTATION_H
#define DFMINIMP3_IMPLEMENTATION_H

#include "Arduino.h"

#include "MessageHandler_interface.h"
#include "Arduino_config.h"
#include "Tonuino_config.h"

#include "DfMiniMp3/DFMiniMp3_interface.h"

#include <SoftwareSerial.h>

#include "DFMiniMp3.h"

class Mp3Notify
{

private:
    // Disallow creating an instance of this object
    Mp3Notify(){};

public:
    static void OnError(uint16_t errorCode)
    {
        bufferedMessage = Message::ERRORCOM;
    };

    static void OnPlayFinished(DfMp3_PlaySources src, uint16_t track)
    {
        bufferedMessage = Message::TRACKFINISHED;
    };

    static void OnPlaySourceOnline(uint16_t code)
    {
        bufferedMessage = Message::SDONLINE;
    };

    static void OnPlaySourceInserted(uint16_t code)
    {
        bufferedMessage = Message::SDINSERT;
    };

    static void OnPlaySourceRemoved(uint16_t code)
    {
        bufferedMessage = Message::SDREMOVE;
    };

    static Message::eMessageContent getMessage()
    {
        return bufferedMessage;
    };

    static void clearMessage()
    {
        bufferedMessage = Message::NONE;
    }

private:
    static Message::eMessageContent bufferedMessage;
};

// Wrapper class to interface DfMiniMp3 hardware
class DfMini : public DfMiniMp3_interface
{
public:
    explicit DfMini(MessageHander_interface &rMessageHandler) : m_rMessageHandler(rMessageHandler) {}
    ~DfMini() = default;
    DfMini(const DfMini &cpy) = delete;

    void init() override
    {
        m_dfMiniMp3.begin(); // init serial and start DfMiniMp3 module
        loop();
        setEq(DFMINI_EQ_SETTING);
        setVolume(VOLUME_INIT);
        Message::eMessageContent message{Message::ERRORCOM};
        if (getVolume() == VOLUME_INIT)
        {
            message = Message::PLAYERONLINE;
        }
        m_rMessageHandler.printMessage(message);
    }

    void loop() override
    {
        m_dfMiniMp3.loop();
    }

    void setEq(eMp3Eq eq) override
    {
        m_dfMiniMp3.setEq((DfMp3_Eq)eq);
        m_dfMiniMp3.loop();
    }

    void setVolume(uint8_t volume) override
    {
        m_dfMiniMp3.setVolume(volume);
        m_dfMiniMp3.loop();
    }

    uint8_t getVolume() override
    {
        m_dfMiniMp3.loop();
        return m_dfMiniMp3.getVolume();
    }

    void increaseVolume() override
    {
        m_dfMiniMp3.increaseVolume();
        m_dfMiniMp3.loop();
    }

    void decreaseVolume() override
    {
        m_dfMiniMp3.decreaseVolume();
        m_dfMiniMp3.loop();
    }

    void pause() override
    {
        m_dfMiniMp3.pause();
        m_dfMiniMp3.loop();
    }

    void start() override
    {
        m_dfMiniMp3.start();
        m_dfMiniMp3.loop();
    }

    void stop() override
    {
        m_dfMiniMp3.stop();
        m_dfMiniMp3.loop();
    }

    void playFolderTrack(uint8_t folderId, uint8_t trackId) override
    {
        m_dfMiniMp3.playFolderTrack(folderId, trackId);
        m_dfMiniMp3.loop();
        m_playsPrompt = false;
        Mp3Notify::clearMessage(); // to clear "finished playing Track" mesage from buffer
    }

    void stopPrompt() override
    {
        if (isPlaying() && m_playsPrompt)
        {
            m_dfMiniMp3.stop();
            m_dfMiniMp3.loop();
            m_playsPrompt = false;
        }
    }

    void playPrompt(uint16_t trackId) override
    {
        m_dfMiniMp3.loop();
        m_dfMiniMp3.playAdvertisement(trackId);
        m_playsPrompt = true;
    }

    uint8_t getFolderTrackCount(uint8_t folderId) override
    {
        m_dfMiniMp3.loop();
        return static_cast<uint8_t>(m_dfMiniMp3.getFolderTrackCount(static_cast<uint16_t>(folderId)));
    }

    bool isTrackFinished() const override
    {
        bool status{false};
        if (Mp3Notify::getMessage() == Message::TRACKFINISHED)
        {
            Mp3Notify::clearMessage();
            status = true;
        }
        return status;
    }

    bool isPlaying() const override
    {
        return !(digitalRead(DFMINI_PIN_ISIDLE));
    }

    void printStatus() const override
    {
        m_rMessageHandler.printMessage(Mp3Notify::getMessage());
    }

private:
    MessageHander_interface &m_rMessageHandler;
    // Solution for constructor error found here:
    //https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX)
    //because compiler interprets this as a class method call. Using curly braces also solves "Most Vexing Parse problem."
    SoftwareSerial m_Mp3SwSerial{DFMINI_RX, DFMINI_TX};
    DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{m_Mp3SwSerial};

    bool m_playsPrompt{false};
};
#endif // DFMINIMP3_IMPLEMENTATION_H