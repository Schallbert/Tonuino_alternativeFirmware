#ifndef DFMINIMP3_IMPLEMENTATION_H
#define DFMINIMP3_IMPLEMENTATION_H
#include <DfMiniMp3_interface.h>

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

// Wrapper class to interface DfMiniMp3 hardware
// No business logic in here!
class DfMini : public DfMiniMp3_interface
{
public:
    ~DfMini(){};

public:
    void begin() override                   {m_dfMiniMp3.begin()};
    void setEq(DfMp3_Eq eq) override        {m_dfMiniMp3.setEq(eq)};
    void setVolume(uint8_t volume) override {m_dfMiniMp3.setVolum(volume)};
    void loop() override                    {m_dfMiniMp3.loop()};
    uint8_t getVolume() override            {return m_dfMiniMp3.getVolume()};
    void increaseVolume() override          {m_dfMiniMp3.increaseVolume()};
    void decreaseVolume() override          {m_dfMiniMp3.decreaseVolume()};
    void pause() override                   {m_dfMiniMp3.pause()};
    void start() override                   {m_dfMiniMp3.start()};
    void stop() override                    {m_dfMiniMp3.stop()};
    void playFolderTrack(uint8_t folderId, uint8_t trackId) override 
                                            {m_dfMiniMp3.playFolderTrack(folderId, trackId)};
    void playAdvertisement(uint16_t trackId) override 
                                            {m_dfMiniMp3.playAdvertisement(trackId)};
    uint8_t getFolderTrackCount(uint8_t folderID) override 
                {return static_cast<uint8_t>(m_dfMiniMp3.getFolderTrackCount(static_cast<uint16_t>(folderId)))};

    private:
    // Solution for constructor error found here: https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)}; // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX) because compiler interprets this as a class method call
    DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
};
#endif // DFMINIMP3_IMPLEMENTATION_H