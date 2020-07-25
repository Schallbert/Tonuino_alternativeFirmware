#include "Mp3PlayerControl.h"

Mp3PlayerControl::Mp3PlayerControl(DfMiniMp3_interface *player,
                                   Arduino_interface_pins *pinCtrl,
                                   Arduino_interface_com *usb,
                                   Arduino_interface_delay *delayCtrl)
{
    m_pDfMiniMp3 = player;
    m_pPinCtrl = pinCtrl;
    m_pUsb = usb;
    m_pDelayCtrl = delayCtrl;
    // Init communication with module and setup
    m_pPinCtrl->pin_mode(DFMINI_BUSY, INPUT);
    m_pDfMiniMp3->begin(); // Init
    m_pDelayCtrl->delay_ms(WAIT_DFMINI_READY);
    m_pDfMiniMp3->setEq(DFMINI_EQ_SETTING);
    m_pDfMiniMp3->setVolume(VOLUME_INIT);
}
void Mp3PlayerControl::loop()
{
    m_pDfMiniMp3->loop();
    autoplay();
}
void Mp3PlayerControl::volume_up()
{
    if (m_pDfMiniMp3->getVolume() < VOLUME_MAX)
    {
        m_pDfMiniMp3->increaseVolume();
    }
#if DEBUGSERIAL
    m_pUsb->com_println("volume_up()");
    m_pUsb->com_print(m_pDfMiniMp3->getVolume());
#endif
}
void Mp3PlayerControl::volume_down()
{
    if (m_pDfMiniMp3->getVolume() > VOLUME_MIN)
    {
        m_pDfMiniMp3->decreaseVolume();
    }
#if DEBUGSERIAL
    m_pUsb->com_println("volume_down()");
    m_pUsb->com_print(m_pDfMiniMp3->getVolume());
#endif
}
bool Mp3PlayerControl::is_playing()
{
    return !(m_pPinCtrl->digital_read(DFMINI_BUSY));
}
void Mp3PlayerControl::play_pause()
{
    if (is_playing())
    {
        m_pDfMiniMp3->pause();
    }
    else
    {
        m_pDfMiniMp3->start(); // Only successful if a track is entered.
    }
}
void Mp3PlayerControl::dont_skip_current_track()
{
    //Blocker method to make feature wait until voice prompt has played
    //To ensure following voice prompts do not overwrite current
    unsigned long currentTime = m_pDelayCtrl->milli_seconds();

    while (!is_playing() && m_pDelayCtrl->milli_seconds() < (currentTime + WAIT_DFMINI_READY))
    {
        m_pDfMiniMp3->loop(); //wait for track to start (until timeout kicks in)
    }
    while (is_playing() && m_pDelayCtrl->milli_seconds() < (currentTime + TIMEOUT_PROMPT_PLAYED))
    {
        m_pDfMiniMp3->loop(); //wait for track to finish
    }
}
void Mp3PlayerControl::autoplay()
{
    // Autoplay implementation
    if (m_pDfMiniMp3->checkTrackFinished())
    {
        Folder::ePlayMode mode = m_pCurrentFolder->get_play_mode();
        if (mode == Folder::ONELARGETRACK)
        {
#if DEBUGSERIAL
            m_pUsb->com_println("autoplay: ONELARGETRACK active: pause");
#endif
            m_pDfMiniMp3->stop();
            return;
        }
        else if (mode == Folder::LULLABYE && check_lullabye_timeout())
        {
#if DEBUGSERIAL
            m_pUsb->com_println("autoplay: LULLABYE timeout expired: pause");
#endif
            m_pDfMiniMp3->stop();
            return;
        }
        else
        {
#if DEBUGSERIAL
            m_pUsb->com_println("autoplay: calling next_track");
#endif
            next_track();
        }
    }
}

void Mp3PlayerControl::lullabye_timeout_tick1ms()
{
    static volatile uint16_t ticks = 0;
    ++ticks;
#define SEC_MSEC 1000
    if (ticks >= SEC_MSEC)
    {
        ticks = 0;
        ++m_ui32LullabyeTimeActiveSecs;
    }
}

bool Mp3PlayerControl::check_lullabye_timeout()
{
    if (m_ui32LullabyeTimeActiveSecs >= LULLABYE_TIMER_SECS)
    {
        // Allow KeepAlive timer to kick in by pausing the track.
        m_ui32LullabyeTimeActiveSecs = 0;
        return true;
    }
    return false;
}

void Mp3PlayerControl::next_track()
{
    if (!check_folder())
    {
#if DEBUGSERIAL
        m_pUsb->com_println("next_track: Error: No folder linked");
#endif
        return; // Cannot play a track if the card is not linked.
    }
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
        m_pUsb->com_println("prev_track: Error: No folder linked");
#endif
        return; // Cannot play a track if the card is not linked.
    }
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
#if DEBUGSERIAL
        m_pUsb->com_println("play_folder: Error: No folder linked");
#endif
        return; // Cannot play a track if the card is not linked.
    }
    // Start playing folder: first track of current folder.
    m_pDfMiniMp3->playFolderTrack(m_pCurrentFolder->get_folder_id(),
                                  m_pCurrentFolder->get_current_track());
#if DEBUGSERIAL
    m_pUsb->com_println("play_folder: folderId: ");
    m_pUsb->com_print(m_pCurrentFolder->get_folder_id());
#endif
}

void Mp3PlayerControl::play_specific_file(uint16_t fileId)
{
    m_pDfMiniMp3->playAdvertisement(fileId);
}

uint8_t Mp3PlayerControl::get_trackCount_of_folder(uint8_t folderId)
{
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