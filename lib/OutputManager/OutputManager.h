#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include "PowerManager_interface.h"
#include "Mp3PlayerControl_interface.h"
#include "InputManager.h" // Use Enums

#include "../Utilities/LinkMenu.h"
#include "../Utilities/DeleteMenu.h"
#include "../Utilities/SimpleTimer.h"

class OutputManager
{
public:
    OutputManager(Arduino_interface_com *pUsb,
                  PowerManager_interface *pPwrCtrl,
                  NfcControl *pNfcCtrl,
                  Mp3PlayerControl_interface *pMp3,
                  SimpleTimer *pMenuTimer,
                  EEPROM_interface *pEeprom,
                  uint32_t ui32Seed) : m_pUsb(pUsb),
                                       m_pSysPwr(pPwrCtrl),
                                       m_pNfcCtrl(pNfcCtrl),
                                       m_pMp3Ctrl(pMp3),
                                       m_pMenuTimer(pMenuTimer),
                                       m_pEeprom(pEeprom),
                                       m_ui32RandomSeed(ui32Seed){};

public:
    // Sets input states from card and buttons, and determines internal state.
    void setInputStates(Nfc_interface::eTagState cardState, UserInput::UserRequest_e userInput);
    // Runs desicion table that calls functions depending on user input
    void runDispatcher();

private:
    // handles errors from cardReader or UserInput interfaces
    void handleInputErrors();
    // Checks link menu state and plays according voice prompts
    void handleLinkMenu();
    // Checks delete menu state and plays according voice prompts
    void handleDeleteMenu();
    // Prompts the new option chosen in Link Menu
    void changeOption(uint16_t option);
    // Updates folder information on NFC card if necessary based on MP3 player read
    void updateFolderInformation();
    // ----- Wrapper methods to call target object's methods -----
    // No action performed
    void none(){};
    // Play/pause
    void plPs() { m_pMp3Ctrl->play_pause(); };
    // next track
    void next() { m_pMp3Ctrl->next_track(); };
    // previous track
    void prev() { m_pMp3Ctrl->prev_track(); };
    // increase volume
    void incV() { m_pMp3Ctrl->volume_up(); };
    // decrease volume
    void decV() { m_pMp3Ctrl->volume_down(); };
    // play help prompt
    void help() { m_pMp3Ctrl->play_specific_file(MSG_HELP); };
    // delete and unlink NFC card
    void delt(); // delete menu entry
    void delC(); // confirm deletion
    // link NFC card to SD card folder
    void linC(); // confirm link
    void linN() { changeOption(m_linkMenu.select_next()); }; // link next command
    void linP() { changeOption(m_linkMenu.select_prev()); }; // link prev command
    // read and Play card's linked folder
    void read();
    // aborts current menu or process
    void abrt();

private:
    typedef void (OutputManager::*dispatcher)(); // table of function pointers
    // members by dependency injection
    Arduino_interface_com *m_pUsb{nullptr};
    PowerManager_interface *m_pSysPwr{nullptr};
    NfcControl *m_pNfcCtrl{nullptr};
    Mp3PlayerControl_interface *m_pMp3Ctrl{nullptr};
    SimpleTimer *m_pMenuTimer{nullptr};
    EEPROM_interface *m_pEeprom{nullptr};
    uint32_t m_ui32RandomSeed{0};

    // Member objects
    Folder m_currentFolder{};
    LinkMenu m_linkMenu{};
    DeleteMenu m_deleteMenu{};
    Nfc_interface::eTagState m_eCardState{Nfc_interface::NO_TAG};
    UserInput::UserRequest_e m_eUserInput{UserInput::NO_ACTION};
};

#endif // OUTPUTMANAGER_H