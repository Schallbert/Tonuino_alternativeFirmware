#ifndef MP3PLAYERCONTROL_H
#define MP3PLAYERCONTROL_H

#include <Arduino_types.h>

#include <Folder.h>
#include <Defines.h>
#include <Arduino_interface.h>
#include <DFMiniMp3_interface.h>
#include <SimpleTimer.h>

class Mp3PlayerControl
{
public:
    Mp3PlayerControl(DfMiniMp3_interface *pPlayer,
                     Arduino_interface_pins *pPinCtrl,
                     Arduino_interface_com *pUsb,
                     Arduino_interface_delay *pDelayCtrl,
                     SimpleTimer *pLullabyeTimer);

public:
    // Listen for DFminiMp3 replies, call autoplay routine
    void loop();
    // Increases volume (<= VOLUME_MAX)
    void volume_up();
    // Decreases volume (>= VOLUME_MIN)
    void volume_down();
    // Gets next track from queue and starts playback
    void next_track();
    // Gets previous track from queue and starts playback
    void prev_track();
    // If playing, pauses track and vice versa
    void play_pause();
    // Returns true if DFminiMp3 is currently playing
    bool is_playing();
    // Starts playback of specified folder (handles queueing and folder specific playmodes)
    void play_folder(Folder *m_pCurrentFolder);
    // Plays specific file sd:/advert/####fileId
    void play_specific_file(uint16_t fileId);
    // Tells controller to not allow skipping the track that is currently played (e.g. for advertisements)
    void dont_skip_current_track();
    // Starts inquiry to player to return number of tracks in selected folder.
    uint8_t get_trackCount_of_folder(uint8_t folderId);

private:
    // Routine to check playmode and select next track
    void autoplay();
    // Routine to check if lullabye time has reached to enable KeepAlive to switch system off after timeout.
    bool check_lullabye_timeout();
    // Checks if the folder we're working with is correctly defined
    bool check_folder();

private:
    // Solution for constructor error found here: https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    //SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)}; // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX) because compiler interprets this as a class method call
    //DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
    DfMiniMp3_interface *m_pDfMiniMp3{nullptr};
    Arduino_interface_pins *m_pPinCtrl{nullptr};
    Arduino_interface_com *m_pUsb{nullptr};
    Arduino_interface_delay *m_pDelayCtrl{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    Folder *m_pCurrentFolder{nullptr};
};

#endif // MP3PLAYERCONTROL_H