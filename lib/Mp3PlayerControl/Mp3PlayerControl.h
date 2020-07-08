#ifndef MP3PLAYERCONTROL_H
#define MP3PLAYERCONTROL_H

#ifndef ARDUINO_H
#include "Arduino.h"
#endif
#include <Folder.h>
#include <Defines.h>
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>
#include <util/delay.h>
// TODO: Includes Swserial, dfmini etc

// Classes
// implement a DFMiniMp3 Serial notification class,
// its member methods will get called
class Mp3Notify
{

    private:
  // Disallow creating an instance of this object
  Mp3Notify() {}
public:
    static void OnError(uint16_t errorCode)
    {
// see DfMp3_Error for code meaning
#if DEBUGSERIAL
        Serial.println();
        Serial.print("DFMiniMp3 Com Error ");
        Serial.println(errorCode);
#endif
    }
    static void OnPlayFinished(uint16_t track)
    {
        getTrackFinished(true);
    }
    static void OnCardOnline(uint16_t code)
    {
#if DEBUGSERIAL
        Serial.println(F("SD Karte online "));
#endif
    }
    static void OnCardInserted(uint16_t code)
    {
#if DEBUGSERIAL
        Serial.println(F("SD Karte ready "));
#endif
    }
    static void OnCardRemoved(uint16_t code)
    {
#if DEBUGSERIAL
        Serial.println(F("SD Karte removed "));
#endif
    }
    static void OnUsbOnline(uint16_t code)
    {
#if DEBUGSERIAL
        Serial.println(F("USB online "));
#endif
    }
    static void OnUsbInserted(uint16_t code)
    {
#if DEBUGSERIAL
        Serial.println(F("USB ready "));
#endif
    }
    static void OnUsbRemoved(uint16_t code)
    {
#if DEBUGSERIAL
        Serial.println(F("USB removed "));
#endif
    }
    // returns true if track has been set finished
    // to set state call method setFinished = true
    // to get state call method setFinished = false
    static bool getTrackFinished(bool setFinished)
    {
        static bool trackFinished = false;
        if (setFinished)
        {
            trackFinished = true;
        }
        else if (trackFinished)
        {
            trackFinished = false;
            return true;
        }
        return false;

    }

private:
    static bool trackFinished;
};

class Mp3PlayerControl
{
public:
    Mp3PlayerControl();

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
    void play_folder(Folder* m_pCurrentFolder);
    // Plays specific file sd:/advert/####fileId
    void play_specific_file(uint16_t fileId);
    // Tells controller to not allow skipping the track that is currently played (e.g. for advertisements)
    void dont_skip_current_track();
    // Starts inquiry to player to return number of tracks in selected folder.
    uint8_t get_trackCount_of_folder(uint8_t folderId);
    // Timed routine for lullabye NFC card configuration. To be called in main's 1ms timer interrupt.
    void lullabye_timeout_tick1ms();

private:
    // Routine to check playmode and select next track
    void autoplay();
    // Routine to check if lullabye time has reached to enable KeepAlive to switch system off after timeout.
    bool check_lullabye_timeout();

private:
// Solution for constructor error found here: https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)}; // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX) because compiler interprets this as a class method call
    DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
    Folder *m_pCurrentFolder;
    uint16_t m_ui32LullabyeTimeActiveSecs;
};

#endif // MP3PLAYERCONTROL_H