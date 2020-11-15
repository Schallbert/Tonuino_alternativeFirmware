#include "PlaybackControl.h"

void PlaybackControl::getTagState()
{
    m_tagState = m_pNfcControl->get_tag_presence();
}

void PlaybackControl::setUserInput(UserInput::eUserRequest userInput)
{
    m_eUserInput = userInput;
}

void PlaybackControl::loop()
{
    m_pSystemPower->set_playback(m_pMp3Ctrl->is_playing());
    dispatchInputs();
}

void PlaybackControl::dispatchInputs()
{
    // initialize 2D-array of function pointers to address state-event transitions
    // dispatch table contains function pointers
    // cardStates = ROWS, userInput = COLUMNS
    typedef PlaybackControl PC;
    static const dispatcher dispatchTable[Nfc_interface::NUMBER_OF_TAG_STATES]
                                         [UserInput::NUMBER_OF_REQUESTS] =
                                             {
                                                 //NOAC,     PL_PS,     PP_LP,     NEXT_,     PREV_,     INC_V,     DEC_V,
                                                 {&PC::none, &PC::plPs, &PC::help, &PC::next, &PC::prev, &PC::incV, &PC::decV}, // NO_TAG
                                                 {&PC::none, &PC::plPs, &PC::help, &PC::next, &PC::prev, &PC::incV, &PC::decV}, // ACTIVE_KNOWN_TAG,
                                                 {&PC::read, &PC::read, &PC::read, &PC::read, &PC::read, &PC::read, &PC::read}, // NEW_REGISTERED_TAG,
                                                 {&PC::none, &PC::none, &PC::help, &PC::none, &PC::none, &PC::none, &PC::none}, // NEW_UNKNOWN_TAG
                                             };
    dispatcher dispatchExecutor = dispatchTable[m_tagState][m_eUserInput];
    (this->*dispatchExecutor)();
}

// All the actual exectutions should not be here. Put downstream. but how?
void PlaybackControl::read()
{
    if (m_pNfcControl->read_folder_from_card(m_currentFolder))
    {
        updateFolderInformation();
        m_currentFolder.setup_dependencies(m_pArduinoHal); // TODO: SOLVE maybe on top level?
        m_pMp3Ctrl->play_folder(&m_currentFolder);
    }
    else
    {
       m_pErrorHandler->setCardReadError();
    }
}

// TODO: OUTSOURCE OR KILL!
void PlaybackControl::updateFolderInformation()
{
    // update trackCount (might change when folders on SD card are modified content-wise)
    uint8_t ui8SavedTrackCnt = m_currentFolder.get_track_count();
    uint8_t ui8RealTrackCnt = m_pMp3Ctrl->get_trackCount_of_folder(m_currentFolder.get_folder_id());
    if (ui8RealTrackCnt == 0)
    {
        m_pErrorHandler->setFolderError(); // folder without tracks on SD card. Error.
        return;
    }
    if (ui8SavedTrackCnt != ui8RealTrackCnt)
    {
        m_currentFolder = Folder(m_currentFolder.get_folder_id(),
                                 m_currentFolder.get_play_mode(),
                                 ui8RealTrackCnt);
        m_pNfcControl->write_folder_to_card(m_currentFolder); // update folder information on card
    }
}