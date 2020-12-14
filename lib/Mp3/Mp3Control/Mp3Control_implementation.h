#ifndef MP3PLAYERCONTROL_IMPLEMENTATION_H
#define MP3PLAYERCONTROL_IMPLEMENTATION_H

#include "../Arduino_HardwareAbstraction/Arduino_DIcontainer_interface.h"
#include "../MessageHandler/MessageHandler_interface.h"
#include "../Nfc/NfcControl/NfcControl_interface.h"

#include "Mp3Control/Mp3Control_interface.h"
#include "Mp3Play/Mp3Play_interface.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"

class Mp3ControlNotify
{
public:
    enum eDebugMessage
    {
        noMessage = 0,
        volumeUp,
        volumeDown,
        next,
        prev,
        play,
        pause
    };

    static const char *toString(eDebugMessage message)
    {
#if DEBUGSERIAL
        static const char *NOTIFY_STRING[] = {
            nullptr,
            "volume up",
            "volume down",
            "next track",
            "previous track",
            "play"
            "pause"};
        return NOTIFY_STRING[message];
#endif
        return nullptr;
    }
};

// Implementation of Player controller interface.
// Uses Software Serial to communicate with DfMiniMp3 player.
// Dependencies use interfaces themselves for
// Hardware abstraction and better testabililty.
class Mp3Control : public Mp3Control_interface
{
public:
    Mp3Control(DfMiniMp3_interface *pDfMini,
               Mp3Play_interface *pPlayer,
               NfcControl_interface *pNfcControl,
               MessageHander_interface *pMsgHandler);

    void setUserInput(UserInput_interface::eUserRequest input) override;
    void loop() override;
    void setTagState(NfcControl_interface::eTagState input) override;
    void setBlocked(bool isBlocked) override;

private:
    void handleCardInput();
    void handleUserInput();
    void autoplay();

    void none(){};
    void play();
    void pause();

    void plPs();
    void next();
    void prev();

    void incV();
    void decV();
    void help();

private:
    // Solution for constructor error found here: https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    //SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)}; // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX) because compiler interprets this as a class method call
    //DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
    DfMiniMp3_interface *m_pDfMiniMp3{nullptr};
    Mp3Play_interface *m_pMp3Player{nullptr};
    NfcControl_interface *m_pNfcControl{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};

    UserInput_interface::eUserRequest m_userInput{UserInput_interface::NO_ACTION};
    NfcControl_interface::eTagState m_tagState{NfcControl_interface::NO_TAG};
    bool m_blocked{false};
    typedef void (Mp3Control::*dispatcher)(); // table of function pointers
};

#endif // MP3PLAYERCONTROL_IMPLEMENTATION_H