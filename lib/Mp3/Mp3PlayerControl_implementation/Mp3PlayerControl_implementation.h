#ifndef MP3PLAYERCONTROL_IMPLEMENTATION_H
#define MP3PLAYERCONTROL_IMPLEMENTATION_H

#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"

#include "../Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"
#include "../Mp3/DFMiniMp3_interface/DFMiniMp3_interface.h"
#include "../Folder/Folder.h"

#include "../Config/Tonuino_config.h"
#include "../Config/Arduino_config.h"
#include "../Utilities/SimpleTimer.h"

// Implementation of Player controller interface.
// Uses Software Serial to communicate with DfMiniMp3 player.
// Dependencies use interfaces themselves for
// Hardware abstraction and better testabililty.
class Mp3PlayerControl : public Mp3PlayerControl_interface
{
public:
    Mp3PlayerControl(Arduino_DIcontainer_interface *pArduinoHal,
                     DfMiniMp3_interface *pPlayer,
                     SimpleTimer *pLullabyeTimer,
                     SimpleTimer *pDfMiniMsgTimeout);

public:
    void loop() override;

    void play_pause() override;
    void next_track() override;
    void prev_track() override;

    void volume_up() override;
    void volume_down() override;

    bool is_playing() override;
    uint8_t get_trackCount_of_folder(uint8_t folderId) override;

    void play_folder(Folder *m_pCurrentFolder) override;
    void play_prompt(VoicePrompt prompt) override;

#if DEBUGSERIAL
    // Prints message from player periphery or player controller to Serial.
    void print_debug_message()
    {
        Arduino_interface_com *m_pSerial = m_pArduinoHal->getSerial();
        m_pSerial->com_println("PLAYER CONTROL DEBUG:");
        m_pSerial->com_println(stringFromPlayerCtrlNotify(m_debugMessage));
        m_pSerial->com_println("MP3 DEBUG: DfMiniMp3");
        m_pSerial->com_println(m_pDfMiniMp3->getPlayerNotification());
    };
#endif

private:
    void waitForPlayerReady();
    void autoplay();
    void dontSkipCurrentTrack();

    bool isFolderValid();

private:
    enum eDebugMessage
    {
        noMessage = 0,
        volumeUp,
        volumeDown,
        autoplayOneLargeTrack,
        autoplayLullabye,
        autoplayNext,
        next_noFolder,
        next,
        prev_noFolder,
        prev,
        play_noFolder,
        play
    };

#if DEBUGSERIAL
    const char *stringFromPlayerCtrlNotify(eDebugMessage value)
    {
        static const char *NOTIFY_STRING[] = {
            "No Message",
            "volume up",
            "volume down",
            "autoplay: OneLargeTrack complete. Paused",
            "autoplay: Lullabye timeout. Paused",
            "autoplay: next track",
            "next: no folder linked!",
            "next track",
            "prev: now folder linked!",
            "previous track",
            "play: no folder linked!",
            "play folder"};

        return NOTIFY_STRING[value];
    };
#endif

private:
    // Solution for constructor error found here: https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    //SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)}; // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX) because compiler interprets this as a class method call
    //DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    DfMiniMp3_interface *m_pDfMiniMp3{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pDfMiniMsgTimeout{nullptr};
    Folder *m_pCurrentFolder{nullptr};

    eDebugMessage m_debugMessage{noMessage};
};

#endif // MP3PLAYERCONTROL_IMPLEMENTATION_H