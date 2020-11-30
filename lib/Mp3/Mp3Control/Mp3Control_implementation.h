#ifndef MP3PLAYERCONTROL_IMPLEMENTATION_H
#define MP3PLAYERCONTROL_IMPLEMENTATION_H

#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"

#include "UserInput_interface/UserInput_interface.h"
#include "Mp3Control/Mp3Control_interface.h"
#include "Mp3Play/Mp3Play_interface.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "../ErrorHandler/ErrorHandler_interface.h"

#include "../Config/Tonuino_config.h"
#include "../Config/Arduino_config.h"
#include "../Utilities/SimpleTimer.h"

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
                   MessageHander_interface *pMsgHandler);

        void setUserInput(UserInput::eUserRequest input) override;
        void loop() override;

    private:
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
        MessageHander_interface *m_pMessageHandler{nullptr};

        UserInput::eUserRequest m_userInput{UserInput::NO_ACTION};
        typedef void (Mp3Control::*dispatcher)(); // table of function pointers
    };

#endif // MP3PLAYERCONTROL_IMPLEMENTATION_H