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
        trackFinished = true;
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
    static bool getTrackFinished()
    {
        if (trackFinished)
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
    void loop();
    void volume_up();
    void volume_down();
    void next_track();
    void prev_track();
    void play_pause();
    bool is_playing();
    void play_folder(Folder* currentFolder);
    void play_specific_file(uint16_t fileId);
    void set_card_detected(bool cardDetected);
    void dont_skip_current_track();
    uint8_t get_trackCount_of_folder(uint8_t folderId);

private:
    void autoplay();
    bool check_lullabye_timeout();

private:
    SoftwareSerial dfMiniMp3SoftwareSerial;
    static DFMiniMp3<SoftwareSerial, Mp3Notify> dfMiniMp3;
    bool cardDetected;
    Folder *currentFolder;
};

#endif // MP3PLAYERCONTROL_H