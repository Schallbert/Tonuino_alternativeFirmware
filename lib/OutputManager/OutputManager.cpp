#include "OutputManager.h"

void OutputManager::setInputStates(Nfc_interface::eTagState tagState, UserInput::UserRequest_e userInput)
{
    // set_state to input values, modify if currently in menu
    m_eTagState = tagState;
    m_eUserInput = userInput;

    m_pSysPwr->set_playback(m_pMp3Ctrl->is_playing());

    handleDeleteMenu();
    handleLinkMenu();

    if (m_pMenuTimer->is_elapsed())
    {
        abrt(); // Timer elapsed, reset menu state.
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
    // NUMBER_OF_REQUESTS -1 as state "error" is caught in above handleInputErrors
    static const dispatcher dispatchTable[Nfc_interface::NUMBER_OF_TAG_STATES - 1]
                                         [UserInput::NUMBER_OF_REQUESTS - 1] =
                                             {
                                                 //NOAC,     PL_PS,     PP_LP,     NEXT_,     PREV_,     INC_V,     DEC_V,
                                                 {&OM::none, &OM::plPs, &OM::help, &OM::next, &OM::prev, &OM::incV, &OM::decV}, // NO_TAG
                                                 {&OM::none, &OM::plPs, &OM::delt, &OM::next, &OM::prev, &OM::incV, &OM::decV}, // ACTIVE_KNOWN_TAG,
                                                 {&OM::read, &OM::read, &OM::read, &OM::read, &OM::read, &OM::read, &OM::read}, // NEW_KNOWN_TAG,
                                                 {&OM::none, &OM::linC, &OM::abrt, &OM::linN, &OM::linP, &OM::none, &OM::none}, // NEW_UNKNOWN_TAG,
                                                 {&OM::none, &OM::delC, &OM::abrt, &OM::none, &OM::none, &OM::none, &OM::none}, // DELETE_TAG_MENU,
                                             };

    dispatcher dispatchExecutor = dispatchTable[m_eTagState][m_eUserInput];
    (this->*dispatchExecutor)();
}

void OutputManager::handleInputErrors()
{
    bool bError{false};
    // Check for index out of bounds
    if ((m_eTagState < Nfc_interface::NO_TAG) ||
        (m_eTagState >= Nfc_interface::NUMBER_OF_TAG_STATES))
    {
        bError = true;
#ifdef DEBUGSERIAL
        m_pArduinoHal->getSerial()->com_println("Dispatcher: cardState out of range!");
#endif
    }
    else if (m_eTagState == Nfc_interface::ERROR)
    {
        bError = true;
#ifdef DEBUGSERIAL
        m_pArduinoHal->getSerial()->com_println("NfcReader internal error!");
#endif
    }

    if ((m_eUserInput < UserInput::NO_ACTION) ||
        (m_eUserInput >= UserInput::NUMBER_OF_REQUESTS))
    {
        bError = true;
#ifdef DEBUGSERIAL
        m_pArduinoHal->getSerial()->com_println("Dispatcher: userInput out of range!");
#endif
    }
    else if (m_eUserInput == UserInput::ERROR)
    {
        bError = true;
#ifdef DEBUGSERIAL
        m_pArduinoHal->getSerial()->com_println("userInput internal error!");
#endif
    }

    if (bError)
    {
        m_pMp3Ctrl->play_specific_file(MSG_ERROR);
        m_pMp3Ctrl->dont_skip_current_track();
        m_eUserInput = UserInput::NO_ACTION;
        m_eTagState = Nfc_interface::NO_TAG;
    }
}

void OutputManager::handleDeleteMenu()
{
    // order of these two condition statements is CRITICAL!
    if ((m_deleteMenu.is_state(DeleteMenu::DELETE_MENU)) &&
        (m_eTagState == Nfc_interface::NEW_KNOWN_TAG))
    {
        m_deleteMenu.set_ready();
    }

    // lock state in menu, waiting for card to be placed that shall be deleted
    if (!m_deleteMenu.is_state(DeleteMenu::NO_MENU))
    {
        m_eTagState = Nfc_interface::DELETE_TAG_MENU; // delete menu entered
        m_pSysPwr->set_delMenu();
    }
}

void OutputManager::handleLinkMenu()
{
    if (m_eTagState == Nfc_interface::NEW_UNKNOWN_TAG)
    {
        if (m_linkMenu.get_state() == LinkMenu::NO_MENU)
        {
            linC();
        }
    }
    else if (!m_linkMenu.get_state() == LinkMenu::NO_MENU)
    {
        m_eTagState = Nfc_interface::NEW_UNKNOWN_TAG; // keeps in link menu
        m_pSysPwr->set_linkMenu();
    }
}

void OutputManager::read()
{
    if (m_pNfcCtrl->read_folder_from_card(m_currentFolder))
    {
        updateFolderInformation();
        m_currentFolder.setup_dependencies(m_pArduinoHal); // TODO: SOLVE maybe on top level?

        m_pMp3Ctrl->play_folder(&m_currentFolder);
    }
    else
    {
        m_pMp3Ctrl->play_specific_file(MSG_ERROR_CARDREAD);
        m_pMp3Ctrl->dont_skip_current_track();
        m_eTagState = Nfc_interface::NO_TAG;
    }
}

void OutputManager::delt()
{
    m_pMp3Ctrl->play_specific_file(MSG_DELETETAG);
    m_pMp3Ctrl->dont_skip_current_track();
    m_pMenuTimer->start(MENU_TIMEOUT_SECS);
    m_deleteMenu.init(); // keep in delete menu
}

void OutputManager::delC()
{
    if (m_deleteMenu.is_state(DeleteMenu::DELETE_READY))
    { // Do delete the card.
        m_pMenuTimer->stop();
        m_pMp3Ctrl->play_specific_file(MSG_CONFIRMED);
        m_pMp3Ctrl->dont_skip_current_track();
        if (!m_pNfcCtrl->erase_card())
        {
            m_pMp3Ctrl->play_specific_file(MSG_ERROR_CARDREAD);
            m_pMp3Ctrl->dont_skip_current_track();
        }
        m_deleteMenu.leave();
        m_pSysPwr->set_playback(false);
    }
    else
    {
        m_pMp3Ctrl->play_specific_file(MSG_DELETETAG);
        m_pMenuTimer->stop(); // restart menu timer
        m_pMenuTimer->start(MENU_TIMEOUT_SECS);
    }
}

void OutputManager::abrt()
{
    m_pMenuTimer->stop();
    m_deleteMenu.leave();
    m_linkMenu.select_abort();
    m_pMp3Ctrl->play_specific_file(MSG_ABORTED);
}

void OutputManager::linC()
{
    switch (m_linkMenu.get_state())
    {
    case LinkMenu::NO_MENU:
        m_linkMenu.init(); // runs card link method on UNKNOWN_CARD detected
        m_pSysPwr->set_linkMenu();
        m_pMp3Ctrl->play_specific_file(MSG_SELECT_FOLDERID); // prompts user to select folder ID
        m_pMp3Ctrl->dont_skip_current_track();
        break;
    case LinkMenu::FOLDER_SELECT:
        m_linkMenu.select_confirm();
        m_pMp3Ctrl->play_specific_file(MSG_SELECT_PLAYMODE);
        m_pMp3Ctrl->dont_skip_current_track();
        break;
    case LinkMenu::PLAYMODE_SELECT:
        m_linkMenu.select_confirm(); // done!
        m_pMenuTimer->stop();
        m_pSysPwr->set_playback(false); // indicate link menu is complete
        // link folder information complete! Obtain folder and save to card.
        uint8_t folderId = m_linkMenu.get_folderId();
        Folder::ePlayMode playMode = m_linkMenu.get_playMode();
        uint8_t trackCount = m_pMp3Ctrl->get_trackCount_of_folder(folderId);
        // Create new folder object and copy to main's folder object
        m_currentFolder = Folder(folderId, playMode, trackCount);
        if (!m_currentFolder.is_initiated())
        {
            m_pMp3Ctrl->play_specific_file(MSG_ERROR_FOLDER);
            m_pMp3Ctrl->dont_skip_current_track();
            abrt();
        }
        else
        {
            m_pMp3Ctrl->play_specific_file(MSG_TAGCONFSUCCESS);
            m_pMp3Ctrl->dont_skip_current_track();
            if (m_pNfcCtrl->write_folder_to_card(m_currentFolder))
            {
                read();
            }
            else // Couldn't write to card due to folder setup error.
            {

                m_pMp3Ctrl->play_specific_file(MSG_ERROR_CARDREAD);
                m_pMp3Ctrl->dont_skip_current_track();
                abrt();
            }
        }
        m_linkMenu.select_abort(); // reset menu state
        return;                    // do not restart menu timer
    }

    // Restart timeout
    m_pMenuTimer->stop();
    m_pMenuTimer->start(MENU_TIMEOUT_SECS);
}

void OutputManager::changeOption(uint16_t option)
{
    // play folderId of current choice, e.g. "one".
    m_pMp3Ctrl->play_specific_file(option);
    m_pMp3Ctrl->dont_skip_current_track();
    if (m_linkMenu.get_state() == LinkMenu::FOLDER_SELECT)
    {
        // play preview of selected folder's contents
        Folder previewFolder = Folder(static_cast<uint8_t>(option), Folder::ONELARGETRACK, 1);
        m_pMp3Ctrl->play_folder(&previewFolder);
    }
}

void OutputManager::updateFolderInformation()
{
    // update trackCount (might change when folders on SD card are modified content-wise)
    uint8_t ui8SavedTrackCnt = m_currentFolder.get_track_count();
    uint8_t ui8RealTrackCnt = m_pMp3Ctrl->get_trackCount_of_folder(m_currentFolder.get_folder_id());
    if (ui8RealTrackCnt == 0)
    {
        m_pMp3Ctrl->play_specific_file(MSG_ERROR_FOLDER); // folder without tracks on SD card. Error.
        return;
    }
    if (ui8SavedTrackCnt != ui8RealTrackCnt)
    {
        m_currentFolder = Folder(m_currentFolder.get_folder_id(),
                                 m_currentFolder.get_play_mode(),
                                 ui8RealTrackCnt);
        m_pNfcCtrl->write_folder_to_card(m_currentFolder); // update folder information on card
    }
}