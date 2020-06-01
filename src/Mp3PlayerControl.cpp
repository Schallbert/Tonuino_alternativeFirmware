#include "Mp3PlayerControl.h"

Mp3PlayerControl::Mp3PlayerControl()
{
    pinMode(DFMINI_BUSY, INPUT);
    dfMiniMp3.begin(); // Init
    delay(WAIT_DFMINI_READY);
    dfMiniMp3.setEq(DfMp3_Eq::DfMp3_Eq_Normal);
    dfMiniMp3.setVolume(VOLUME_INIT);
}
void Mp3PlayerControl::loop()
{
    dfMiniMp3.loop();
    autoplay();
}
void Mp3PlayerControl::volume_up()
{
    if (dfMiniMp3.getVolume() < VOLUME_MAX)
    {
        dfMiniMp3.increaseVolume();
    }
#if SERIAL
    Serial.println(F("=== volume_up()"));
    Serial.println(currVolume);
#endif
}
void Mp3PlayerControl::volume_down()
{
    if (dfMiniMp3.getVolume() > VOLUME_MIN)
    {
        dfMiniMp3.decreaseVolume();
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
        dfMiniMp3.pause();
    }
    else if (currentFolder->is_valid())
    {
        dfMiniMp3.start(); //can only restart playback if a valid folder is existing
    }
}
void Mp3PlayerControl::dont_skip_current_track()
{
    //Blocker function to make feature wait until voice prompt has played
    //To ensure following voice prompts do not overwrite current
    //Better solution would be a queue ;)
    unsigned long currentTime = millis();

    while (!is_playing() && millis() < (currentTime + WAIT_DFMINI_READY))
    {
        dfMiniMp3.loop(); //wait for track to start (until timeout kicks in)
    }
    while (is_playing())
    {
        dfMiniMp3.loop(); //wait for track to finish
    }
}
void Mp3PlayerControl::autoplay()
{
    // Autoplay implementation
    if (Mp3Notify::getTrackFinished(false))
    {
        Folder::PlayMode mode = currentFolder->get_play_mode();
        if (mode == Folder::ONELARGETRACK)
        {
#if DEBUGSERIAL
            Serial.println(F("ONELARGETRACK mode active -> wait for power off"));
#endif
            dfMiniMp3.stop();
            return;
        }
        else if (mode == Folder::LULLABYE && check_lullabye_timeout())
        {
#if DEBUGSERIAL
            Serial.println(F("LULLABYE timeout expired -> wait for power off"));
#endif
            dfMiniMp3.stop();
            return;
        }
        else
        {
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
        ++lullabyeTimeActiveSecs;
    }
}
bool Mp3PlayerControl::check_lullabye_timeout()
{
    if (lullabyeTimeActiveSecs >= LULLABYE_TIMER_SECS)
    {
        // Allow KeepAlive timer to kick in by pausing the track.
        lullabyeTimeActiveSecs = 0;
        return true;
    }
    return false;
}
void Mp3PlayerControl::next_track()
{
    if (!currentFolder->is_valid())
    {
#if DEBUGSERIAL
        Serial.println(F("=== in next_track: Error: No card linked"));
#endif
        return; // Cannot play a track if the card is not linked.
    }
    uint8_t nextTrack = currentFolder->get_next_track();
    dfMiniMp3.playFolderTrack(currentFolder->get_folder_id(), nextTrack);
#if DEBUGSERIAL
    Serial.println(F("=== next_track()"));
    Serial.print(nextTrack);
#endif
}
void Mp3PlayerControl::prev_track()
{
    if (!currentFolder->is_valid())
    {
#if DEBUGSERIAL
        Serial.println(F("=== in prev_track: Error: No card linked"));
#endif
        return; // Cannot play a track if the card is not linked.
    }
    uint8_t prevTrack = currentFolder->get_prev_track();
    dfMiniMp3.playFolderTrack(currentFolder->get_folder_id(), prevTrack);
#if DEBUGSERIAL
    Serial.println(F("=== prev_track()"));
    Serial.print(prevTrack);
#endif
}
void Mp3PlayerControl::play_folder(Folder *currentFolder)
{
#if DEBUGSERIAL
    Serial.println(F("== play_folder"));
    Serial.print(F(" folderId: "));
    Serial.println(currentFolder->get_folder_id());
#endif
    // Start playing folder: first track of current folder.
    dfMiniMp3.playFolderTrack(currentFolder->get_folder_id(), currentFolder->get_current_track());
}
void Mp3PlayerControl::play_specific_file(uint16_t fileId)
{
    dfMiniMp3.playAdvertisement(fileId);
}
uint8_t Mp3PlayerControl::get_trackCount_of_folder(uint8_t folderId)
{
    uint16_t trackCnt = dfMiniMp3.getFolderTrackCount(static_cast<uint16_t>(folderId));
    if (trackCnt > 0xFF)
    {
        return 0; // Error: number of tracks too high
    }
    else
    {
        return static_cast<uint8_t>(trackCnt);
    }
}