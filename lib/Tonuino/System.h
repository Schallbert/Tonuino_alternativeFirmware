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
        NO_CARD = 0,
        ACTIVE_KNOWN_CARD, 
        NEW_KNOWN_CARD,  // read card link folder play card allow all mp3 commands
        UNKNOWN_CARD, // play voice menu link folder to card
        DELETE_CARD,
        CARD_ERROR,
        NUMBER_OF_CARD_STATES
    };

public:
    InputManager();

public:
    eCardState getCardState();
    UserInput::UserRequest_e getUserInput();
    Folder getCurrentFolder();

private:
    KeepAlive &keepAlive;
    StatusLed &statusLed;
    UserInput &userInput;

    Mp3PlayerControl &mp3;
    NfcTag &nfcTagReader;
    Folder currentFolder{};
};

class OutputManager
{
public:
    //OutputManager(Mp3PlayerControl *mp3, NfcTag *nfc);

    
public:
    void setCurrentFolder(Folder &source);
    bool runDispatcher(InputManager::eCardState cardState, UserInput::UserRequest_e userInput);

private:
    // ----- Wrapper methods to call target object's methods -----
    // No action performed
    void none(){};
    // Play/pause
    void plPs(){ m_mp3->play_pause(); };
    // next track
    void next(){ m_mp3->next_track(); };
    // previous track
    void prev(){ m_mp3->prev_track(); };
    // increase volume
    void incV(){ m_mp3->volume_up(); };
    // decrease volume
    void decV(){ m_mp3->volume_down(); };
    // play help prompt
    void help() { m_mp3->play_specific_file(MSG_HELP); };
    // delete and unlink NFC card
    void delt() { m_mp3->play_specific_file(MSG_DELETETAG); }; // MORE??!! set state!
    // link NFC card to SD card folder
    void link(){ link_card_to_folder(); };
    // read and Play card's linked folder 
    void read(){ m_mp3.play_folder(m_currentFolder); };
    // play error prompt
    void erro() { m_mp3->play_specific_file(MSG_ERROR); };
    // play confirm prompt (press play...)
    void conf(){};
    // aborts current menu or process
    void abrt();

    // ----- Wrapper methods to call target object's methods -----
    // TODO: Start timeout for any menu we're going into
    void link_card_to_folder();
    void delete_card();


private:
    Mp3PlayerControl *m_mp3{};
    NfcTag *m_nfcTagReader{};
    Folder *m_currentFolder{};

    // dispatcher is a function pointer, belonging to this class
    typedef void (OutputManager::*dispatcher)();
    bool m_deleteCardRequested{false}; // TODO: local static?
};