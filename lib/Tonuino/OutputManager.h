#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include <Arduino_interface.h>
#include <PowerManager_interface.h>
#include <NfcTag.h>
#include <Mp3PlayerControl.h>


#include "LinkMenu.h"
#include "DeleteMenu.h"
#include "SimpleTimer.h"
#include "InputManager/InputManager.h" // Use Enums

// TODO: for testability: Setup interface classes for PowerManager
// and UserInterface (enums)

class OutputManager
{
public:


    OutputManager(Arduino_interface_com *pUsb,
                  PowerManager_interface *pPwrCtrl,
                  NfcTag *pNfcReader,
                  Mp3PlayerControl *pMp3,
                  SimpleTimer *pMenuTimer,
                  EEPROM_interface *pEeprom,
                  uint32_t ui32Seed) : m_pUsb(pUsb),
                                       m_pSysPwr(pPwrCtrl),
                                       m_pNfcTagReader(pNfcReader),
                                       m_pMp3(pMp3),
                                       m_pMenuTimer(pMenuTimer),
                                       m_pEeprom(pEeprom),
                                       m_ui32RandomSeed(ui32Seed){};

public:
    // Sets input states from card and buttons, and determines internal state.
    void setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput);
    // Runs desicion table that calls functions depending on user input
    void runDispatcher();
    bool getShutdownRequest();

private:
    // ----- Wrapper methods to call target object's methods -----
    // No action performed
    void none(){};
    // Play/pause
    void plPs() { m_pMp3->play_pause(); };
    // next track
    void next() { m_pMp3->next_track(); };
    // previous track
    void prev() { m_pMp3->prev_track(); };
    // increase volume
    void incV() { m_pMp3->volume_up(); };
    // decrease volume
    void decV() { m_pMp3->volume_down(); };
    // play help prompt
    void help() { m_pMp3->play_specific_file(MSG_HELP); };
    // delete and unlink NFC card
    void delt(); // delete menu entry
    void delC(); // confirm deletion
    // link NFC card to SD card folder
    void linC(); // confirm link
    void linN(); // link next command
    void linP(); // link prev command
    // read and Play card's linked folder
    void read();
    // play error prompt
    void erro() { m_pMp3->play_specific_file(MSG_ERROR); };
    // aborts current menu or process
    void abrt();

    // ----- Wrapper methods to call target object's methods -----
    void handleInputErrors();

private:
    typedef void (OutputManager::*dispatcher)(); // table of function pointers
    // members by dependency injection
    Arduino_interface_com *m_pUsb{nullptr};
    PowerManager_interface *m_pSysPwr{nullptr};
    NfcTag *m_pNfcTagReader{nullptr};
    Mp3PlayerControl *m_pMp3{nullptr};
    SimpleTimer *m_pMenuTimer{nullptr};
    EEPROM_interface *m_pEeprom{nullptr};
    uint32_t m_ui32RandomSeed{0};

    // Member objects
    Folder m_currentFolder{};
    LinkMenu m_linkMenu{m_pMp3, m_pEeprom};
    DeleteMenu m_deleteMenu{};
    InputManager::eCardState m_eCardState{InputManager::NO_CARD};
    UserInput::UserRequest_e m_eUserInput{UserInput::NO_ACTION};
};

#endif // OUTPUTMANAGER_H