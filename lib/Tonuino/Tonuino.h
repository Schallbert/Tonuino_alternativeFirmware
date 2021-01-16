#ifndef TONUINO_H
#define TONUINO_H

#include "UserInput/UserInput_interface.h"
#include "NfcControl/NfcControl_interface.h"
#include "Mp3Control/Mp3Control_interface.h"

#include "VoiceMenu.h"

class Tonuino
{
public:
    Tonuino(UserInput_interface *pUserInput,
            NfcControl_interface &rNfcControl,
            Mp3Control_interface &rMp3Control,
            VoiceMenu &rVoiceMenu) : m_pUserInput(pUserInput),
                                                 m_rNfcControl(rNfcControl),
                                                 m_rMp3Control(rMp3Control),
                                                 m_rVoiceMenu(rVoiceMenu){};

    void loop();

private:
    UserInput_interface *m_pUserInput{nullptr};
    NfcControl_interface &m_rNfcControl;
    Mp3Control_interface &m_rMp3Control;
    VoiceMenu &m_rVoiceMenu;
};

#endif // TONUINO_H