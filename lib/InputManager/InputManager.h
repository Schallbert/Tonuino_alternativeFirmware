#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "Arduino_interface.h"
#include "NfcTagControl.h"
#include "UserInput_interface.h"
#include "Arduino_config.h"

class InputManager
{
public:
    enum eTagState
    {
        NO_CARD = 0,       // allow certain actions (help, deleteCard etc)
        ACTIVE_KNOWN_CARD, // full playback
        NEW_KNOWN_CARD,    // read card, get folder, full playback
        UNKNOWN_CARD_MENU, // play voice menu, link folder to card
        DELETE_CARD_MENU,  // delete card menu
        NUMBER_OF_CARD_STATES
    };

public:
    InputManager(Arduino_interface_pins *pPinCtrl,
                 Arduino_interface_com *pUsb,
                 UserInput *pUserInput,
                 NfcTagControl *pNfcReader) : m_pPinControl(pPinCtrl),
                                              m_pUsb(pUsb),
                                              m_pUserInput(pUserInput),
                                              m_pNfcTagReader(pNfcReader){};

public:
    uint32_t getRandomSeed();
    eTagState getCardState();
    UserInput::UserRequest_e getUserInput();

private:
    Arduino_interface_pins *m_pPinControl{nullptr};
    Arduino_interface_com *m_pUsb{nullptr};
    UserInput *m_pUserInput{nullptr};
    NfcTagControl *m_pNfcTagReader{nullptr};
};

#endif // INPUTMANAGER_H