#include "Mp3PlayerControl.h"

Mp3PlayerControl::Mp3PlayerControl()
{
    pinMode(DFMINI_BUSY, INPUT);
    m_dfMiniMp3.begin(); // Init
    delay(WAIT_DFMINI_READY);
    m_dfMiniMp3.setEq(DFMINI_EQ_SETTING);
    m_dfMiniMp3.setVolume(VOLUME_INIT);
}
void Mp3PlayerControl::loop()
{
    m_dfMiniMp3.loop();
    autoplay();
}
void Mp3PlayerControl::volume_up()
{
    if (m_dfMiniMp3.getVolume() < VOLUME_MAX)
    {
        m_dfMiniMp3.increaseVolume();
    }
#if SERIAL
    Serial.println(F("=== volume_up()"));
    Serial.println(currVolume);
#endif
}
void Mp3PlayerControl::volume_down()
{
    if (m_dfMiniMp3.getVolume() > VOLUME_MIN)
    {
        m_dfMiniMp3.decreaseVolume();
    }
#if SERIAL
    Serial.println(F("=== volume_down()"));
    Serial.println(volume);
#endif
}
bool Mp3PlayerControl::is_playing()
{
    return !digitalRead(DFMINI_BUSY);
}
void Mp3PlayerControl::play_pause()
{
    if (is_playing())
    {
        m_dfMiniMp3.pause();
    }
    else if (m_pCurrentFolder->is_valid())
    {
        m_dfMiniMp3.start(); //can only restart playback if a valid folder is existing
    }
}
void Mp3PlayerControl::dont_skip_current_track()
{
    //Blocker method to make feature wait until voice prompt has played
    //To ensure following voice prompts do not overwrite current
    unsigned long currentTime = millis();

    while (!is_playing() && millis() < (currentTime + WAIT_DFMINI_READY))
    {
        m_dfMiniMp3.loop(); //wait for track to start (until timeout kicks in)
    }
    while (is_playing() && millis() < (currentTime + TIMEOUT_PROMPT_PLAYED))
    {
        m_dfMiniMp3.loop(); //wait for track to finish
    }
}
void Mp3PlayerControl::autoplay()
{
    // Autoplay implementation
    if (Mp3Notify::getTrackFinished(false))
    {
        Folder::ePlayMode mode = m_pCurrentFolder->get_play_mode();
        if (mode == Folder::ONELARGETRACK)
        {
#if DEBUGSERIAL
            Serial.println(F("autoplay: ONELARGETRACK mode active -> wait for power off"));
#endif
            m_dfMiniMp3.stop();
            return;
        }
        else if (mode == Folder::LULLABYE && check_lullabye_timeout())
        {
#if DEBUGSERIAL
            Serial.println(F("autoplay: LULLABYE timeout expired -> wait for power off"));
#endif
            m_dfMiniMp3.stop();
            return;
        }
        else
        {
#if DEBUGSERIAL
            Serial.println(F("autoplay: calling next_track"));
#endif
            next_track();
        }
    }
}
void Mp3PlayerControl::lullabye_timeout_tick1ms()
{
    static uint16_t ticks = 0;
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
    if (!m_pCurrentFolder->is_valid())
    {
#if DEBUGSERIAL
        Serial.println(F("next_track: Error: No card linked"));
#endif
        return; // Cannot play a track if the card is not linked.
    }
    uint8_t nextTrack = m_pCurrentFolder->get_next_track();
    m_dfMiniMp3.playFolderTrack(m_pCurrentFolder->get_folder_id(), nextTrack);
#if DEBUGSERIAL
    Serial.println(F("next_track():"));
    Serial.print(nextTrack);
#endif
}
void Mp3PlayerControl::prev_track()
{
    if (!m_pCurrentFolder->is_valid())
    {
#if DEBUGSERIAL
        Serial.println(F("prev_track: Error: No card linked"));
#endif
        return; // Cannot play a track if the card is not linked.
    }
    uint8_t prevTrack = m_pCurrentFolder->get_prev_track();
    m_dfMiniMp3.playFolderTrack(m_pCurrentFolder->get_folder_id(), prevTrack);
#if DEBUGSERIAL
    Serial.println(F("prev_track():"));
    Serial.print(prevTrack);
#endif
}
void Mp3PlayerControl::play_folder(Folder *m_pCurrentFolder)
{
#if DEBUGSERIAL
    Serial.println(F("play_folder"));
    Serial.print(F("folderId: "));
    Serial.println(m_pCurrentFolder->get_folder_id());
#endif
    // Start playing folder: first track of current folder.
    m_dfMiniMp3.playFolderTrack(m_pCurrentFolder->get_folder_id(), m_pCurrentFolder->get_current_track());
}
void Mp3PlayerControl::play_specific_file(uint16_t fileId)
{
    m_dfMiniMp3.playAdvertisement(fileId);
}
uint8_t Mp3PlayerControl::get_trackCount_of_folder(uint8_t folderId)
{
    uint16_t trackCnt = m_dfMiniMp3.getFolderTrackCount(static_cast<uint16_t>(folderId));
    if (trackCnt > 0xFF)
    {
        return 0; // Error: number of tracks too high
    }
    else
    {
        return static_cast<uint8_t>(trackCnt);
    }
}