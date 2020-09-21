#include "Mp3PlayerControl.h"

Mp3PlayerControl::Mp3PlayerControl(DfMiniMp3_interface *pPlayer,
                                   Arduino_interface_pins *pPinCtrl,
                                   Arduino_interface_com *pUsb,
                                   SimpleTimer *pLullabyeTimer,
                                   SimpleTimer *pDfMiniMsgTimeout) : m_pDfMiniMp3(pPlayer),
                                                                     m_pPinCtrl(pPinCtrl),
                                                                     m_pUsb(pUsb),
                                                                     m_pLullabyeTimer(pLullabyeTimer),
                                                                     m_pDfMiniMsgTimeout(pDfMiniMsgTimeout)
{

    // Init communication with module and setup
    m_pPinCtrl->pin_mode(DFMINI_PIN_ISIDLE, INPUT);
    m_pDfMiniMp3->begin(); // Init
    wait_player_ready();
    m_pDfMiniMp3->setEq(DFMINI_EQ_SETTING);
    m_pDfMiniMp3->setVolume(VOLUME_INIT);
    m_pDfMiniMp3->checkPlayerNotification(); // INIT (static) message system to "no_message"
}
void Mp3PlayerControl::loop()
{
    wait_player_ready();
    autoplay();
}
void Mp3PlayerControl::volume_up()
{
    wait_player_ready();
    if (m_pDfMiniMp3->getVolume() < VOLUME_MAX)
    {
        wait_player_ready();
        m_pDfMiniMp3->increaseVolume();
    }
#if DEBUGSERIAL
    m_pUsb->com_println("volume_up()");
    m_pUsb->com_print(m_pDfMiniMp3->getVolume());
#endif
}
void Mp3PlayerControl::volume_down()
{
    wait_player_ready();
    if (m_pDfMiniMp3->getVolume() > VOLUME_MIN)
    {
        wait_player_ready();
        m_pDfMiniMp3->decreaseVolume();
    }
#if DEBUGSERIAL
    m_pUsb->com_println("volume_down()");
    m_pUsb->com_print(m_pDfMiniMp3->getVolume());
#endif
}
bool Mp3PlayerControl::is_playing()
{
    return !(m_pPinCtrl->digital_read(DFMINI_PIN_ISIDLE));
}
void Mp3PlayerControl::play_pause()
{
    wait_player_ready();
    if (is_playing())
    {
        m_pDfMiniMp3->pause();
    }
    else
    {
        m_pLullabyeTimer->start(LULLABYE_TIMEOUT_SECS); // start shutdown timer
        wait_player_ready();
        m_pDfMiniMp3->start(); // Only successful if a track is entered.
    }
}
void Mp3PlayerControl::dont_skip_current_track()
{
    //Blocker method to make feature wait until voice prompt has played
    //To ensure following voice prompts do not overwrite current
    m_pDfMiniMsgTimeout->start(WAIT_DFMINI_READY);
    while (!is_playing() && !(m_pDfMiniMsgTimeout->is_elapsed()))
    {
        wait_player_ready(); //wait for track to start (until timeout kicks in)
    }
    m_pDfMiniMsgTimeout->stop();
    m_pDfMiniMsgTimeout->start(TIMEOUT_PROMPT_PLAYED);
    while (is_playing() && !(m_pDfMiniMsgTimeout->is_elapsed()))
    {
        wait_player_ready(); //wait for track to finish
    }
    m_pDfMiniMsgTimeout->stop();
}

void Mp3PlayerControl::autoplay()
{
// Autoplay implementation
#if DEBUGSERIAL
    m_pUsb->com_println(
        m_pDfMiniMp3->stringFromDfMiniNotify(
            m_pDfMiniMp3->checkPlayerNotification()));
#endif
    if (m_pDfMiniMp3->checkTrackFinished())
    {
        Folder::ePlayMode mode = m_pCurrentFolder->get_play_mode();
        if (mode == Folder::ONELARGETRACK)
        {
#if DEBUGSERIAL
            m_pUsb->com_println("autoplay: ONELARGETRACK active: pause");
#endif
            wait_player_ready();
            m_pDfMiniMp3->stop();
            return;
        }
        else if (mode == Folder::LULLABYE && m_pLullabyeTimer->is_elapsed())
        {
#if DEBUGSERIAL
            m_pUsb->com_println("autoplay: LULLABYE timeout expired: pause");
#endif
            wait_player_ready();
            m_pDfMiniMp3->stop();
            return;
        }
        else
        {
#if DEBUGSERIAL
            m_pUsb->com_println("autoplay: next_track");
#endif
            next_track();
        }
    }
}

void Mp3PlayerControl::next_track()
{
    if (!check_folder())
    {
#if DEBUGSERIAL
        m_pUsb->com_println("next_track: No folder linked");
#endif
        wait_player_ready();
        m_pDfMiniMp3->playAdvertisement(MSG_ERROR_FOLDER);
        return; // Cannot play a track if the card is not linked.
    }
    wait_player_ready();
    m_pDfMiniMp3->playFolderTrack(m_pCurrentFolder->get_folder_id(),
                                  m_pCurrentFolder->get_next_track());
#if DEBUGSERIAL
    m_pUsb->com_println("next_track:");
    m_pUsb->com_print(m_pCurrentFolder->get_current_track());
#endif
}

void Mp3PlayerControl::prev_track()
{
    if (!check_folder())
    {
#if DEBUGSERIAL
        m_pUsb->com_println("prev_track: No folder linked");
#endif
        wait_player_ready();
        m_pDfMiniMp3->playAdvertisement(MSG_ERROR_FOLDER);
        return; // Cannot play a track if the card is not linked.
    }
    wait_player_ready();
    m_pDfMiniMp3->playFolderTrack(m_pCurrentFolder->get_folder_id(),
                                  m_pCurrentFolder->get_prev_track());
#if DEBUGSERIAL
    m_pUsb->com_println("prev_track():");
    m_pUsb->com_print(m_pCurrentFolder->get_current_track());
#endif
}

void Mp3PlayerControl::play_folder(Folder *currentFolder)
{
    m_pCurrentFolder = currentFolder;
    if (!check_folder())
    {
        wait_player_ready();
        m_pDfMiniMp3->playAdvertisement(MSG_ERROR_FOLDER);
#if DEBUGSERIAL
        m_pUsb->com_println("play_folder: No folder linked");
#endif
        return; // Cannot play a track if the card is not linked.
    }
    // Start playing folder: first track of current folder.
    wait_player_ready();
    m_pDfMiniMp3->playFolderTrack(m_pCurrentFolder->get_folder_id(),
                                  m_pCurrentFolder->get_current_track());
#if DEBUGSERIAL
    m_pUsb->com_println("play_folder: folderId: ");
    m_pUsb->com_print(m_pCurrentFolder->get_folder_id());
#endif
}

void Mp3PlayerControl::play_specific_file(uint16_t fileId)
{
    wait_player_ready();
    m_pDfMiniMp3->playAdvertisement(fileId);
}

uint8_t Mp3PlayerControl::get_trackCount_of_folder(uint8_t folderId)
{
    wait_player_ready();
    uint16_t trackCnt = m_pDfMiniMp3->getFolderTrackCount(static_cast<uint16_t>(folderId));
    if (trackCnt > 0xFF)
    {
        return 0; // Error: number of tracks too high
    }
    else
    {
        return static_cast<uint8_t>(trackCnt);
    }
}

bool Mp3PlayerControl::check_folder()
{
    if (m_pCurrentFolder != nullptr)
    {
        if (m_pCurrentFolder->is_valid())
        {
            return true;
        }
    }
    return false;
}

void Mp3PlayerControl::wait_player_ready()
{
    // command will wait for complete serial
    // communication message to be received from Player
    // assuming that the player is ready to process new input
    m_pDfMiniMp3->loop();
}