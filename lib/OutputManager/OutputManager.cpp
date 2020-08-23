#include "OutputManager.h"

// TODO: Refactor (switch?)
void OutputManager::setInputStates(InputManager::eCardState cardState, UserInput::UserRequest_e userInput)
{
    // set_state to input values, modify if currently in menu
    bool isPlaying = m_pMp3->is_playing();
    m_eCardState = cardState;
    m_eUserInput = userInput;

    m_pSysPwr->set_playback(isPlaying);

    handleDeleteMenu();
    handleLinkMenu();
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

void OutputManager::handleDeleteMenu()
{
    // lock state in menu, waiting for card to be placed that shall be deleted
    if ((m_deleteMenu.is_state(DeleteMenu::DELETE_MENU)) &&
        (m_eCardState == InputManager::NEW_KNOWN_CARD))
    {
        m_deleteMenu.set_state(DeleteMenu::DELETE_READY);
    }

    if (!m_deleteMenu.is_state(DeleteMenu::NO_MENU))
    {
        m_eCardState = InputManager::DELETE_CARD_MENU; // delete menu entered
        m_pSysPwr->set_delMenu();
        m_pMenuTimer->start(MENU_TIMEOUT_SECS);
    };
}

void OutputManager::handleLinkMenu()
{
    if (m_eCardState == InputManager::UNKNOWN_CARD_MENU)
    {
        if (m_linkMenu.is_state(LinkMenu::NO_MENU))
        {
            m_linkMenu.init(); // runs card link method on UNKNOWN_CARD detected
            m_pMenuTimer->start(MENU_TIMEOUT_SECS);
            m_pSysPwr->set_linkMenu();

            //mp3.loop();
            if (m_pMp3->is_playing())
            {
                m_pMp3->play_pause();
            }
            m_pMp3->play_specific_file(MSG_UNKNOWNTAG); // prompts user to select folder ID
        }
        else
        {
            m_eCardState = InputManager::UNKNOWN_CARD_MENU; // keeps in link menu
        }
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
    m_deleteMenu.init(); // keep in delete menu
}

void OutputManager::delC()
{
    if (m_deleteMenu.is_state(DeleteMenu::DELETE_READY))
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
    m_linkMenu.select_abort();
    m_pMp3->play_specific_file(MSG_ABORTEED);
}

void OutputManager::linC()
{
    // Restart timeout
    m_pMenuTimer->stop();
    m_pMenuTimer->start(MENU_TIMEOUT_SECS);
    m_linkMenu.select_confirm();
    if (m_linkMenu.is_state(LinkMenu::COMPLETE))
    {
        m_pMenuTimer->stop(); // cancel timeout
        // link folder information complete! Obtain folder and save to card.
        uint8_t folderId = m_linkMenu.get_folderId();
        Folder::ePlayMode playMode = m_linkMenu.get_playMode();
        uint8_t trackCount = m_pMp3->get_trackCount_of_folder(folderId);
        // Create new folder object and copy to main's folder object
        m_currentFolder = Folder(folderId, playMode, trackCount);

        if (m_pNfcTagReader->write_folder_to_card(m_currentFolder))
        {
            m_pMp3->play_specific_file(MSG_TAGCONFSUCCESS);
            m_pMp3->dont_skip_current_track();
            read();
        }
        else
        {
            // Couldn't write to card due to folder setup error.
            m_pMp3->play_specific_file(MSG_ERROR);
            m_pMp3->dont_skip_current_track();
            abrt();
        }
    }
}

void OutputManager::linN()
{
    changeOption(m_linkMenu.select_next());
}

void OutputManager::linP()
{
    changeOption(m_linkMenu.select_prev());
}

void OutputManager::changeOption(uint16_t option)
{
    // play folderId of current choice, e.g. "one".
    m_pMp3->play_specific_file(option);
    m_pMp3->dont_skip_current_track();
    if (m_linkMenu.is_state(LinkMenu::FOLDER_SELECT))
    {
        // play preview of selected folder's contents
        Folder previewFolder = Folder(static_cast<uint8_t>(option), Folder::ONELARGETRACK, 1);
        m_pMp3->play_folder(&previewFolder);
    }
}
