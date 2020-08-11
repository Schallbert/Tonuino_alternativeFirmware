#include "OutputManager.h"

// TODO: Refactor (switch?)
void OutputManager::setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput)
{
    // set_state to input values, modify if currently in menu
    bool isPlaying = m_pMp3->is_playing();
    m_eCardState = cardState;
    m_eUserInput = userInput;

    m_pSysPwr->set_playback(isPlaying);

    // lock state in menu, waiting for card placing that shall be deleted
    if ((m_deleteMenu.get_state(DeleteMenu::DELETE_MENU)) &&
        (cardState == InputManager::NEW_KNOWN_CARD))
    {
        m_deleteMenu.set_state(DeleteMenu::DELETE_READY);
    }

    if (!m_deleteMenu.get_state(DeleteMenu::NO_MENU))
    {
        m_eCardState = InputManager::DELETE_CARD_MENU; // delete menu entered
        m_pSysPwr->set_delMenu();
        m_pMenuTimer->start(MENU_TIMEOUT_SECS);
    };

    if (cardState == InputManager::UNKNOWN_CARD_MENU)
    {
        m_linkMenu.set_state(true); // runs card link method on UNKNOWN_CARD detected
        m_pMenuTimer->start(MENU_TIMEOUT_SECS);
        m_pSysPwr->set_linkMenu();
    }

    if (m_linkMenu.get_state())
    {
        m_eCardState = InputManager::UNKNOWN_CARD_MENU; // keeps in link menu
    }
}

bool OutputManager::runDispatcher()
{
    // Check for index out of bounds
    if ((InputManager::NO_CARD > m_eCardState) ||
        (m_eCardState >= InputManager::NUMBER_OF_CARD_STATES) ||
        (UserInput::NO_ACTION > m_eUserInput) ||
        (m_eUserInput >= UserInput::NUMBER_OF_REQUESTS))
    {
        return false;
    }
    // to not clutter dispatcher
    handleErrors(); // TODO: refactor?

    // initialize 2D-array of function pointers to address state-event transitions
    // dispatch table contains function pointers
    // cardStates = ROWS, userInput = COLUMNS
    static const dispatcher dispatchTable[InputManager::NUMBER_OF_CARD_STATES]
                                         [UserInput::NUMBER_OF_REQUESTS - 1] =
                                             {
                                                 //NOAC, PL_PS, PP_LP, NEXT_, PREV_, INC_V, DEC_V, ERROR
                                                 {&none, &plPs, &help, &next, &prev, &incV, &decV}, // NO_CARD
                                                 {&none, &plPs, &delt, &next, &prev, &incV, &decV}, // ACTIVE_KNOWN_CARD,
                                                 {&none, &plPs, &none, &next, &prev, &incV, &decV}, // NEW_KNOWN_CARD,
                                                 {&none, &linC, &abrt, &linN, &linP, &none, &none}, // UNKNOWN_CARD_MENU,
                                                 {&none, &delC, &abrt, &none, &none, &none, &none}, // DELETE_CARD_MENU,
                                             };
    dispatcher dispatchExecutor = dispatchTable[m_eCardState][m_eUserInput];
    (this->*dispatchExecutor)();
    // TODO: optimize dispatcher (link, erro!)
    return true;
}

void OutputManager::handleErrors()
{
    // TODO: refactor?
    if (m_eUserInput == UserInput::ERROR)
    {
        m_pMp3->play_specific_file(MSG_ERROR_USERINPUT);
        m_pMp3->dont_skip_current_track();
        m_eUserInput = UserInput::NO_ACTION;
    }
}

void OutputManager::read()
{
    if (m_pNfcTagReader->read_folder_from_card(m_currentFolder))
    {
        m_currentFolder.setup_dependencies(m_pEeprom, m_ui32RandomSeed); // TODO: SOLVE maybe on top level?
        m_pMp3->play_folder(&m_currentFolder);
    }
    else
    {
        // TODO: Add Debug Output?
        m_pMp3->play_specific_file(MSG_ERROR_CARDREAD);
        m_pMp3->dont_skip_current_track();
        m_eCardState = InputManager::NO_CARD;
    }
}

void OutputManager::delt()
{
    m_pMp3->play_specific_file(MSG_DELETETAG);
    m_pMp3->dont_skip_current_track();
    m_deleteMenu.set_state(DeleteMenu::DELETE_MENU); // keep in delete menu
}

void OutputManager::delC()
{
    if (m_deleteMenu.get_state(DeleteMenu::DELETE_READY))
    {
        m_pMenuTimer->stop();
        // Do delete the card.
        m_pMp3->play_specific_file(MSG_CONFIRMED);
        m_deleteMenu.set_state(DeleteMenu::NO_MENU);
        m_pNfcTagReader->erase_card();
    }
    else
    {
        m_pMp3->play_specific_file(MSG_DELETETAG);
    }
}

void OutputManager::abrt()
{
    m_pMenuTimer->stop();
    m_deleteMenu.set_state(DeleteMenu::NO_MENU); // reset menu state
    m_linkMenu.set_state(false);
    m_pMp3->play_specific_file(MSG_ABORTEED);
}

void OutputManager::linC()
{
    if (m_linkMenu.select_confirm())
    {
        m_pMenuTimer->stop();
        // link folder information complete! Obtain folder and save to card.
        m_currentFolder = m_linkMenu.get_folder();
        m_linkMenu.set_state(false);
        if (m_pNfcTagReader->write_folder_to_card(m_currentFolder))
        {
            m_pMp3->play_specific_file(MSG_TAGCONFSUCCESS);
            m_pMp3->dont_skip_current_track();
            read();
        }
        else
        {
            m_pMp3->play_specific_file(MSG_ERROR);
            m_pMp3->dont_skip_current_track();
            abrt();
        }
    }
}

void OutputManager::linN()
{
    m_linkMenu.select_next();
}

void OutputManager::linP()
{
    m_linkMenu.select_prev();
}

void KeepAlive_StatusLed::setup()
{
    m_keep.keep_alive(); //Activate KeepAlive to maintain power supply to circuits
    m_led.set_led_behavior(StatusLed::solid);
}

void KeepAlive_StatusLed::request_shutdown()
{
    m_led.set_led_behavior(StatusLed::off);
    m_keep.request_shutdown();
}

bool KeepAlive_StatusLed::get_shutdown_request()
{
    return m_keep.get_shutdown_request();
}

void KeepAlive_StatusLed::allow_shutdown()
{
    m_keep.allow_shutdown();
}

void KeepAlive_StatusLed::set_playback(bool isPlaying)
{
    if (isPlaying)
    {
        m_led.set_led_behavior(StatusLed::solid);
        m_pIdleTimer->stop();
    }
    else
    {

        m_pIdleTimer->start(IDLE_TIMEOUT_SECS);
        m_led.set_led_behavior(StatusLed::flash_slow);
    }
}

void KeepAlive_StatusLed::set_delMenu()
{
    m_pIdleTimer->stop();
    m_led.set_led_behavior(StatusLed::flash_quick); // Delete Menu
}

void KeepAlive_StatusLed::set_linkMenu()
{
    m_pIdleTimer->stop();
    m_led.set_led_behavior(StatusLed::flash_slow); // Link Menu
}

void KeepAlive_StatusLed::notify_timer_tick()
{
    m_pIdleTimer->timer_tick();
    if (m_pIdleTimer->is_elapsed())
    {
        m_pIdleTimer->stop();
        request_shutdown();
    }
    m_led.led_service();
}