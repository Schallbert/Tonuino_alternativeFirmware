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

void OutputManager::runDispatcher()
{
    // to not clutter dispatcher
    handleInputErrors();

    // initialize 2D-array of function pointers to address state-event transitions
    // dispatch table contains function pointers
    // cardStates = ROWS, userInput = COLUMNS
    typedef OutputManager OM;
    static const dispatcher dispatchTable[InputManager::NUMBER_OF_CARD_STATES]
                                         [UserInput::NUMBER_OF_REQUESTS - 1] =
        {
            //NOAC,     PL_PS,     PP_LP,     NEXT_,     PREV_,     INC_V,     DEC_V, 
            {&OM::none, &OM::plPs, &OM::help, &OM::next, &OM::prev, &OM::incV, &OM::decV}, // NO_CARD
            {&OM::none, &OM::plPs, &OM::delt, &OM::next, &OM::prev, &OM::incV, &OM::decV}, // ACTIVE_KNOWN_CARD,
            {&OM::none, &OM::plPs, &OM::none, &OM::next, &OM::prev, &OM::incV, &OM::decV}, // NEW_KNOWN_CARD,
            {&OM::none, &OM::linC, &OM::abrt, &OM::linN, &OM::linP, &OM::none, &OM::none}, // UNKNOWN_CARD_MENU,
            {&OM::none, &OM::delC, &OM::abrt, &OM::none, &OM::none, &OM::none, &OM::none}, // DELETE_CARD_MENU,
        };
    dispatcher dispatchExecutor = dispatchTable[m_eCardState][m_eUserInput];
    (this->*dispatchExecutor)();
}

void OutputManager::handleInputErrors()
{
    bool bError = false;
    // Check for index out of bounds
    if ((InputManager::NO_CARD > m_eCardState) ||
        (m_eCardState >= InputManager::NUMBER_OF_CARD_STATES))
    {
        bError = true;
#ifdef DEBUGSERIAL
        m_pUsb->com_println("runDispatcher(): cardState out of range!");
#endif
    }
    else if ((UserInput::NO_ACTION > m_eUserInput) ||
             (m_eUserInput >= UserInput::NUMBER_OF_REQUESTS))
    {
        bError = true;
#ifdef DEBUGSERIAL
        m_pUsb->com_println("runDispatcher(): userInput out of range!");
#endif
    }
    else if (m_eUserInput == UserInput::ERROR)
    {
        bError = true;
#ifdef DEBUGSERIAL
        m_pUsb->com_println("runDispatcher(): userInput internal error!");
#endif
    }

    if (bError)
    {
        m_pMp3->play_specific_file(MSG_ERROR);
        m_pMp3->dont_skip_current_track();
        m_eUserInput = UserInput::NO_ACTION;
        m_eCardState = InputManager::NO_CARD;
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