#ifndef MP3PLAYERCONTROL_IMPLEMENTATION_H
#define MP3PLAYERCONTROL_IMPLEMENTATION_H

//#include "../Arduino_HardwareAbstraction/Arduino_DIcontainer_interface.h"

#include "Mp3Control/Mp3Control_interface.h"

#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "Mp3Play/Mp3Play_interface.h"
#include "../Nfc/NfcControl/NfcControl_interface.h"
#include "../PowerManager/PowerManager_interface.h"
#include "../MessageHandler/MessageHandler_interface.h"



// Implementation of Player controller interface.
// Uses Software Serial to communicate with DfMiniMp3 player.
// Dependencies use interfaces themselves for
// Hardware abstraction and better testabililty.
class Mp3Control : public Mp3Control_interface
{
public:
    Mp3Control(DfMiniMp3_interface &rDfMini,
               Mp3Play_interface &rPlayer,
               NfcControl_interface &rNfcControl,
               PowerManager_interface &rPowerManager,
               MessageHander_interface &rMsgHandler) : m_rDfMiniMp3(rDfMini),
                                                       m_rMp3Player(rPlayer),
                                                       m_rNfcControl(rNfcControl),
                                                       m_rPowerManager(rPowerManager),
                                                       m_rMessageHandler(rMsgHandler){};
    ~Mp3Control() = default;

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
    DfMiniMp3_interface &m_rDfMiniMp3;
    Mp3Play_interface &m_rMp3Player;
    NfcControl_interface &m_rNfcControl;
    PowerManager_interface &m_rPowerManager;
    MessageHander_interface &m_rMessageHandler;

    UserInput_interface::eUserRequest m_userInput{UserInput_interface::NO_ACTION};
    NfcControl_interface::eTagState m_tagState{NfcControl_interface::NO_TAG};
    bool m_blocked{false};
    typedef void (Mp3Control::*dispatcher)(); // table of function pointers
};

#endif // MP3PLAYERCONTROL_IMPLEMENTATION_H