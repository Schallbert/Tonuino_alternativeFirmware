#ifndef SYSTEM_H
#define SYSTEM_H

// project includes -------
#include <StatusLed.h>
#include "interface_KeepAlive/interface_KeepAlive.h"
#include <interface_UserInput.h>
#include <Folder.h>
#include <NfcTag.h>
#include <Arduino_implementation.h>
#include <Mp3PlayerControl.h>
#include <DFMiniMp3_implementation.h>
#include <EEPROM_implementation.h>
#include <MFRC522_implementation.h>

/* This is the "main" class of the project.
// It contains all the objects and dependencies needed for operation.
// It also manages timers and asynchronous events.
// It is not designed to be unit testable,
// thus it does not contain business logic.
*/
class System
{
public:
    System();
    ~System();

public:
    void loop();
    void shutdown();

private:
    // Dependency objects -------------------------------------
    KeepAlive_StatusLed *m_pPwrCtrl{nullptr};
    // Arduino hardware control
    Arduino_pins *m_pPinControl{nullptr};
    Arduino_com *m_pUsbSerial{nullptr};
    Arduino_delay *m_pDelayControl{nullptr};
    // Init tag reader
    Mfrc522 *m_pReader{nullptr};
    NfcTag *m_pNfcTagReader{nullptr}; // Constructor injection of concrete reader
    // DFPlayer Mini setup
    DfMini *m_pDfMini{nullptr};
    Mp3PlayerControl *m_pMp3{nullptr};
    // Folder for queuing etc.
    Folder *m_pCurrentFolder{nullptr};
    // Eeprom init
    Eeprom *m_pEeprom{nullptr};
    // Work member objects -----------------------
    InputManager m_inputManager{InputManager(m_pPinControl, m_pNfcTagReader)};
    OutputManager m_outputManager{OutputManager(m_pPwrCtrl, m_pNfcTagReader, m_pMp3)};
    // POD mambers -------------------
    InputManager::eCardState m_cardState{InputManager::NO_CARD};
    UserInput::UserRequest_e m_userEvent{UserInput::NO_ACTION};
};

class InputManager
{
public:
    InputManager(Arduino_pins *pinCtrl,
                 NfcTag *nfcReader) : m_pPinControl(pinCtrl),
                                      m_pNfcTagReader(nfcReader)
    {
        //init UserInput
        m_pUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
        m_pUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
        m_pUserInput->init();
    }

public:
    enum eCardState
    {
        NO_CARD = 0,       // allow certain actions (help, deleteCard etc)
        ACTIVE_KNOWN_CARD, // full playback
        NEW_KNOWN_CARD,    // read card, get folder, full playback
        UNKNOWN_CARD_MENU, // play voice menu, link folder to card
        DELETE_CARD_MENU,  // delete card menu
        NUMBER_OF_CARD_STATES
    };

public:
    uint32_t initRandomGenerator();
    eCardState getCardState();
    UserInput::UserRequest_e getUserInput();

private:
    Arduino_pins *m_pPinControl{nullptr};
    UserInput *m_pUserInput{nullptr};
    NfcTag *m_pNfcTagReader{nullptr};
};

class OutputManager
{
public:
    OutputManager(KeepAlive_StatusLed *pwrCtrl,
                  NfcTag *nfcReader,
                  Mp3PlayerControl *mp3) : m_pSysPwr(pwrCtrl),
                                           m_pNfcTagReader(nfcReader),
                                           m_pMp3(mp3){};

public:
    // Sets input states from card and buttons, and determines internal state.
    void setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput);
    // Runs desicion table that calls functions depending on user input
    bool runDispatcher();

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
    // TODO: Start timeout for any menu we're going into
    void handleErrors();

private:
    typedef void (OutputManager::*dispatcher)(); // table of function pointers
    // members by dependency injection
    KeepAlive_StatusLed *m_pSysPwr{nullptr};
    Mp3PlayerControl *m_pMp3{nullptr};
    NfcTag *m_pNfcTagReader{nullptr};
    // Member variables
    Folder m_currentFolder{};
    LinkMenu m_linkMenu{m_pMp3};
    DeleteMenu m_deleteMenu{};
    InputManager::eCardState m_eCardState{InputManager::NO_CARD};
    UserInput::UserRequest_e m_eUserInput{UserInput::NO_ACTION};
};

// TODO: ADD MENU TIMEOUT!
class DeleteMenu
{
public:
    enum eDelMenuState
    {
        NO_MENU = 0,
        DELETE_MENU,
        DELETE_READY
    };

public:
    void set_state(eDelMenuState state);
    bool get_state(eDelMenuState state) { return (m_eMenuState == state); };

private:
    eDelMenuState m_eMenuState{NO_MENU};
};

/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
*/
class LinkMenu
{
public:
    LinkMenu(Mp3PlayerControl *mp3) : m_pMp3(mp3){};

public:
    // initializes linking process and plays voice prompt if TRUE, else reset object.
    void set_state(bool state);
    // returns true if link menu has been initialized
    bool get_state() { return m_bMenuState; };
    // returns true if configuring is complete
    bool select_confirm();
    // selects next menu item, e.g. next folderId
    void select_next();
    // selects previous menu item, e.g. previous folderId
    void select_prev();
    // returns a valid folder object when configuring is complete.
    Folder get_folder();

private:
    // plays the voice prompt selected option, adjusted to current menu state
    void play_voice_prompt();
    // Initializes values and prompts for folderId.
    void init();
    // resets internal state and values.
    void leave();

private:
    // needed for menu to be able to play voice prompts & previews
    Mp3PlayerControl *m_pMp3{nullptr};
    Folder m_linkedFolder{};
    // initialized for folderId state of linkMenu
    bool m_bMenuState{false};
    bool m_bLinkState{false};
    uint8_t m_ui8Option{0};
    uint8_t m_ui8OptionRange{0};
};

// controls keepalive relay and status LED output.
class KeepAlive_StatusLed
{

public:
    // assumes that mp3 is playing on TRUE
    void set_playback(bool isPlaying);
    // sets LED behavior for delete Menu
    void set_delMenu();
    // sets LED behavior for link Menu
    void set_linkMenu();
    // Keeps system active
    void setup();
    // shuts down the system
    void shutdown();

private:
    StatusLed m_led{StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, HIGH)};
    KeepAlive m_keep{KeepAlive(KEEPALIVE, false, MAXIDLE)};
};

#endif // SYSTEM_H