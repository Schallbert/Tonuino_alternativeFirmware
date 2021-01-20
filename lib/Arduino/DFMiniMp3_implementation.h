#ifndef DFMINIMP3_IMPLEMENTATION_H
#define DFMINIMP3_IMPLEMENTATION_H

#include "Arduino.h"

#include "MessageHandler_interface.h"
#include "Arduino_config.h"

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
        bufferedMessage.setContents(Message::ERRORCOM);
    };

    static void OnPlayFinished(DfMp3_PlaySources src, uint16_t track)
    {
        bufferedMessage.setContents(Message::TRACKFINISHED);
    };

    static void OnPlaySourceOnline(uint16_t code)
    {
        bufferedMessage.setContents(Message::SDONLINE);
    };

    static void OnPlaySourceInserted(uint16_t code)
    {
        bufferedMessage.setContents(Message::SDINSERT);
    };

    static void OnPlaySourceRemoved(uint16_t code)
    {
        bufferedMessage.setContents(Message::SDREMOVE);
    };

    static Message &getMessage()
    {
        return bufferedMessage;
    };

    static void clearMessage()
    {
        bufferedMessage.reset();
    }

private:
    static Message bufferedMessage;
};

// Wrapper class to interface DfMiniMp3 hardware
class DfMini : public DfMiniMp3_interface
{
public:
    explicit DfMini(MessageHander_interface &rMessageHandler) : m_rMessageHandler(rMessageHandler)
    {
        m_dfMiniMp3.begin(); // init serial and start DfMiniMp3 module
        m_dfMiniMp3.loop();
    };

    ~DfMini(){};

    void loop() override
    {
        m_dfMiniMp3.loop();
    };

    void setEq(eMp3Eq eq) override
    {
        m_dfMiniMp3.setEq((DfMp3_Eq)eq);
        m_dfMiniMp3.loop();
    };

    void setVolume(uint8_t volume) override
    {
        m_dfMiniMp3.setVolume(volume);
        m_dfMiniMp3.loop();
    };

    uint8_t getVolume() override
    {
        m_dfMiniMp3.loop();
        return m_dfMiniMp3.getVolume();
    };

    void increaseVolume() override
    {
        m_dfMiniMp3.increaseVolume();
        m_dfMiniMp3.loop();
    };

    void decreaseVolume() override
    {
        m_dfMiniMp3.decreaseVolume();
        m_dfMiniMp3.loop();
    };

    void pause() override
    {
        m_dfMiniMp3.pause();
        m_dfMiniMp3.loop();
    };

    void start() override
    {
        m_dfMiniMp3.start();
        m_dfMiniMp3.loop();
    };

    void stop() override
    {
        m_dfMiniMp3.stop();
        m_dfMiniMp3.loop();
    };

    void playFolderTrack(uint8_t folderId, uint8_t trackId) override
    {
        m_dfMiniMp3.playFolderTrack(folderId, trackId);
        m_dfMiniMp3.loop();
    };

    void playMp3FolderTrack(uint16_t trackId) override
    {
        m_dfMiniMp3.loop();
        m_dfMiniMp3.playMp3FolderTrack(trackId);
    };

    uint8_t getFolderTrackCount(uint8_t folderId) override
    {
        m_dfMiniMp3.loop();
        return static_cast<uint8_t>(m_dfMiniMp3.getFolderTrackCount(static_cast<uint16_t>(folderId)));
    };

    bool isTrackFinished() const override
    {
        bool status{false};
        if (Mp3Notify::getMessage() == Message{Message::TRACKFINISHED})
        {
            Mp3Notify::clearMessage();
            status = true;
        }
        return status;
    };

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
    //because compiler interprets this as a class method call
    SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)};
    DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{
        DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
};
#endif // DFMINIMP3_IMPLEMENTATION_H