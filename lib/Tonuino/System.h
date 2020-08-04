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
    LinkMenu linkMenu{m_mp3};

    // dispatcher is a function pointer, belonging to this class
    typedef void (OutputManager::*dispatcher)();
    InputManager::eCardState m_eCardState{InputManager::NO_CARD};
    UserInput::UserRequest_e m_eUserInput{UserInput::NO_ACTION};

    bool m_bDeleteMenu{false};
    bool m_bDeleteReady{false};
    bool m_bLinkMenu{false};
};

/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
*/
class LinkMenu
{
public:
    LinkMenu(Mp3PlayerControl *mp3);

public:
    // initializes linking process and plays voice prompt
    void init_link();
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

private:
    // needed for menu to be able to play voice prompts & previews
    Mp3PlayerControl *m_mp3{};
    Folder m_linkedFolder{};
    // initialized for folderId state of linkMenu
    bool m_bLinkState{false};
    uint8_t m_ui8Option{0};
    uint8_t m_ui8OptionRange{0};
};