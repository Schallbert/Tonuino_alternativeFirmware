#include <interface_UserInput.h>
#include <Folder.h>
#include <NfcTag.h>
#include <Mp3PlayerControl.h>

class System
{
public:
    void getInputs()
    {
        m_cardState = inputManager.getCardState();
        m_userEvent = inputManager.getUserInput();
        if (m_cardState == InputManager::NEW_KNOWN_CARD)
        {
            m_folder = inputManager.getCurrentFolder();
        }
    }
    void determineOutputs()
    {
        outputManager.runDispatcher(m_cardState, m_userEvent);
    }

private:
    InputManager inputManager{InputManager()};
    OutputManager outputManager{OutputManager()};
    InputManager::eCardState m_cardState{InputManager::NO_CARD};
    UserInput::UserRequest_e m_userEvent{UserInput::NO_ACTION};
    Folder m_folder{};
};

class InputManager
{
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
    InputManager();

public:
    eCardState getCardState_fromReader();
    UserInput::UserRequest_e getUserInput();

private:
    KeepAlive &m_keepAlive;
    StatusLed &m_statusLed;
    UserInput &m_userInput;

    Mp3PlayerControl &mp3;
    NfcTag *m_nfcTagReader;

    bool m_bWriteCardError{false};
};

class OutputManager
{
public:
    //OutputManager(Mp3PlayerControl *mp3, NfcTag *nfc);

public:
    void setCurrentFolder(Folder &source);
    bool runDispatcher();

private:
    // ----- Wrapper methods to call target object's methods -----
    // No action performed
    void none(){};
    // Play/pause
    void plPs() { m_mp3->play_pause(); };
    // next track
    void next() { m_mp3->next_track(); };
    // previous track
    void prev() { m_mp3->prev_track(); };
    // increase volume
    void incV() { m_mp3->volume_up(); };
    // decrease volume
    void decV() { m_mp3->volume_down(); };
    // play help prompt
    void help() { m_mp3->play_specific_file(MSG_HELP); };
    // delete and unlink NFC card
    void delt(); // delete menu entry
    void delC(); // confirm deletion
    // link NFC card to SD card folder
    void link(); // link menu entry
    void linC(); // confirm link
    void linN(); // link next command
    void linP(); // link prev command
    // read and Play card's linked folder
    void read();
    // play error prompt
    void erro() { m_mp3->play_specific_file(MSG_ERROR); };
    // aborts current menu or process
    void abrt();

    // ----- Wrapper methods to call target object's methods -----
    // TODO: Start timeout for any menu we're going into
    void setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput);
    void handleErrors();

private:
    Mp3PlayerControl *m_mp3{};
    NfcTag *m_nfcTagReader{};
    Folder m_currentFolder{};
    LinkMenu linkMenu{};

    // dispatcher is a function pointer, belonging to this class
    typedef void (OutputManager::*dispatcher)();
    InputManager::eCardState m_eCardState{InputManager::NO_CARD};
    UserInput::UserRequest_e m_eUserInput{UserInput::NO_ACTION};

    bool m_bDeleteMenu{false};
    bool m_bDeleteReady{false};
    bool m_bLinkMenu{false};
};

class LinkMenu
{

public:
// returns true if configuring is complete
    void init_link();
    bool select_confirm();
    void select_next();
    void select_prev();
    Folder get_folder();

private:
    void play_voice_prompt();

private:
    Folder m_linkedFolder{};
    // initialized for folderId state
    bool m_bLinkState {false};
    uint8_t m_ui8Option{0};
    uint8_t m_ui8OptionRange{0};

};