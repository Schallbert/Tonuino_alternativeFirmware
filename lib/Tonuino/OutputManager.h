#ifndef OUTPUTMANAGER_H
#define OUTPUTMANAGER_H

#include "interface_KeepAlive/interface_KeepAlive.h"
#include "InputManager.h"
#include <StatusLed.h>
#include <NfcTag.h>
#include <Mp3PlayerControl.h>
#include <VoiceMenu.h>

class OutputManager
{
public:
    OutputManager(KeepAlive_StatusLed *pPwrCtrl,
                  NfcTag *pNfcReader,
                  Mp3PlayerControl *pMp3,
                  Timer *pMenuTimer,
                  EEPROM_interface *pEeprom,
                  uint32_t ui32Seed) : m_pSysPwr(pPwrCtrl),
                                       m_pNfcTagReader(pNfcReader),
                                       m_pMp3(pMp3),
                                       m_pMenuTimer(pMenuTimer),
                                       m_pEeprom(pEeprom),
                                       m_ui32RandomSeed(ui32Seed){};

public:
    // Sets input states from card and buttons, and determines internal state.
    void setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput);
    // Runs desicion table that calls functions depending on user input
    bool runDispatcher();
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
    // TODO: Start timeout for any menu we're going into
    void handleErrors();

private:
    typedef void (OutputManager::*dispatcher)(); // table of function pointers
    // members by dependency injection
    KeepAlive_StatusLed *m_pSysPwr{nullptr};
    Mp3PlayerControl *m_pMp3{nullptr};
    NfcTag *m_pNfcTagReader{nullptr};
    Timer *m_pMenuTimer{nullptr};
    uint32_t m_ui32RandomSeed{0};
    EEPROM_interface *m_pEeprom{nullptr};
    // Member objects
    Folder m_currentFolder{};
    LinkMenu m_linkMenu{m_pMp3, m_pEeprom};
    DeleteMenu m_deleteMenu{};
    InputManager::eCardState m_eCardState{InputManager::NO_CARD};
    UserInput::UserRequest_e m_eUserInput{UserInput::NO_ACTION};
};

// controls keepalive relay and status LED output.
class KeepAlive_StatusLed
{
public:
    KeepAlive_StatusLed(Timer *pIdleTimer) : m_pIdleTimer(pIdleTimer){};

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
    void request_shutdown();
    // returns current shutdown status
    bool get_shutdown_request();
    // shuts system down if shutdown has been requested and bAllow is TRUE
    void allow_shutdown();
    // notifies classes that another timer interval passed
    void notify_timer_tick();

private:
    // Dependency object
    Timer *m_pIdleTimer{nullptr};
    // Member objects
    StatusLed m_led{StatusLed(LED_PIN, FLASHSLOWMS, FLASHQUICKMS, HIGH)};
    KeepAlive m_keep{KeepAlive(KEEPALIVE, false)};
};

#endif // OUTPUTMANAGER_H