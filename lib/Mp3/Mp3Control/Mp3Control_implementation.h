#ifndef MP3PLAYERCONTROL_IMPLEMENTATION_H
#define MP3PLAYERCONTROL_IMPLEMENTATION_H

//#include "../Arduino_HardwareAbstraction/Arduino_DIcontainer_interface.h"

#include "Mp3Control/Mp3Control_interface.h"

#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "Mp3Play/Mp3Play_interface.h"
#include "Mp3Prompt/Mp3Prompt_interface.h"
#include "../Nfc/NfcControl/NfcControl_interface.h"
#include "../PowerManager/PowerManager_interface.h"

class Folder;

// Implementation of Player controller interface.
// Uses Software Serial to communicate with DfMiniMp3 player.
// Dependencies use interfaces themselves for
// Hardware abstraction and better testabililty.
class Mp3Control : public Mp3Control_interface
{
public:
    Mp3Control(DfMiniMp3_interface &rDfMini,
               Mp3Play_interface &rPlayer,
               Mp3Prompt_interface &rPrompt,
               PowerManager_interface &rPowerManager,
               MessageHander_interface &rMsgHandler) : m_rDfMiniMp3(rDfMini),
                                                       m_rMp3Player(rPlayer),
                                                       m_rMp3Prompt(rPrompt),
                                                       m_rPowerManager(rPowerManager),
                                                       m_rMessageHandler(rMsgHandler){};
    ~Mp3Control() = default;
    Mp3Control(const Mp3Control &cpy) = delete;

    void setUserInput(Message::eMessageContent input) override;
    void loop() override;
    void playFolder(Folder &folder) override;

private:
    void handleUserInput();
    void handlePromptStatus();
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
    Mp3Prompt_interface &m_rMp3Prompt;
    PowerManager_interface &m_rPowerManager;
    MessageHander_interface &m_rMessageHandler;

    Message::eMessageContent m_userInput{Message::INPUTNONE};
    Message::eMessageContent m_tagState{Message::NOTAG};
    typedef void (Mp3Control::*dispatcher)(); // table of function pointers
};

#endif // MP3PLAYERCONTROL_IMPLEMENTATION_H