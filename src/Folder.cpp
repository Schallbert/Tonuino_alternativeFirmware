#include "Folder.h"

//Folders
Folder::Folder()
{
    folderId = 0;
    playMode = Folder::UNDEFINED;
    trackCount = 0;
    trackQueue = new uint8_t();
}

Folder::Folder(uint8_t folderId, PlayMode playMode, uint8_t trackCount)
{
    this->folderId = folderId;
    this->playMode = playMode;
    this->trackCount = trackCount;
    trackQueue = new uint8_t[trackCount + 1](); // () is to init contents with 0, new to allow dynamically sized array
    switch (playMode)
    {
    case PlayMode::RANDOM :
    {
        shuffle_queue();
#if DEBUGSERIAL
        Serial.println(F("SHUFFLE -> random queue"));
#endif
        break;
    }
    case PlayMode::SAVEPROGRESS :
    {
        currentQueueEntry = EEPROM.read(folderId);
#if DEBUGSERIAL
        Serial.println(F("SAVEPROGRESS -> sorted queue, save current track"));
#endif
        break;
    }
    case PlayMode::ALBUM :
    {
        init_sorted_queue();
#if DEBUGSERIAL
        Serial.println(F("ALBUM -> sorted queue, endless, rollover"));
#endif
        break;
    }
    case PlayMode::LULLABYE :
    {
        init_sorted_queue();
#if DEBUGSERIAL
        Serial.println(F("LULLABYE -> stop playback after lullabye timeout"));
#endif
        break;
    }
    case PlayMode::ONELARGETRACK :
    {
        init_sorted_queue();
#if DEBUGSERIAL
        Serial.println(F("ONELARGETRACK -> sorted queue, stop playback after each track"));
#endif
        break;
    }
    case PlayMode::UNDEFINED :
    {
        #if DEBUGSERIAL
        Serial.println(F("UNDEFINED -> playmode not correctly configured"));
#endif
        break;
    }
    }
}

Folder::~Folder()
{
    delete[] trackQueue;
}

bool Folder::is_valid()
{
    if (folderId && trackCount && playMode != Folder::UNDEFINED)
    {
        return true;
    }
    return false;
}

uint8_t Folder::get_folder_id()
{
    return folderId;
}

uint8_t Folder::get_current_track()
{
    return trackQueue[currentQueueEntry];
}
uint8_t Folder::get_next_track()
{
    if (trackCount == 0)
    {
#if DEBUGSERIAL
        Serial.println(F("get_next_track: Error: folder not initialized"));
#endif
        return 0; //Error: folder not initialized
    }
    if (currentQueueEntry < trackCount)
    {
        ++currentQueueEntry;
    }
    else
    {
        currentQueueEntry = 1; // Reset queue pointer to first track [1]
    }
    if (playMode == PlayMode::SAVEPROGRESS)
    {
#if DEBUGSERIAL
        Serial.print(F("SAVEPROGRESS -> saving track"));
        Serial.println(currentTrack);
#endif
        EEPROM.update(folderId, trackQueue[currentQueueEntry]);
    }
    return trackQueue[currentQueueEntry];
}
uint8_t Folder::get_prev_track()
{
    if (trackCount == 0)
    {
#if DEBUGSERIAL
        Serial.println(F("get_next_track: Error: folder not initialized"));
#endif
        return 0; //Error: folder not initialized
    }
    if (currentQueueEntry > 1)
    {
        --currentQueueEntry;
    }
    else
    {
        // Reset queue pointer to last track [trackCount]
        currentQueueEntry = trackCount;
    }
    if (playMode == PlayMode::SAVEPROGRESS)
    {
#if DEBUGSERIAL
        Serial.print(F("SAVEPROGRESS -> saving track"));
        Serial.println(currentTrack);
#endif
        EEPROM.update(folderId,  trackQueue[currentQueueEntry]);
    }
    return  trackQueue[currentQueueEntry];
}
Folder::PlayMode Folder::get_play_mode()
{
    return playMode;
}

uint8_t Folder::get_track_count()
{
    return trackCount;
}

void Folder::shuffle_queue()
{
    init_random_generator(); // For random playlists
    uint8_t i = 1; // start at queue[1], queue[0] is always 0!
    uint8_t j = 1;
    uint8_t rnd = 0;
    bool alreadyInQueue = false;
    // Fill queue with non-repeating, random contents.
    while (i < trackCount)
    {
        rnd = random(1, trackCount); //get random number between 1 and trackCount
        j = 1;
        alreadyInQueue = false;
        while (j <= i)
        {
            if (trackQueue[j] == rnd)
            {
                // Random number already used
                alreadyInQueue = true;
                break;
            }
            ++j;
        }
        if (!alreadyInQueue)
        {
            trackQueue[i] = rnd;
            ++i;
        }
    }
    currentQueueEntry = 1;
}
void Folder::init_random_generator()
{
    uint32_t ADC_LSB;
    uint32_t ADCSeed;
    for (uint8_t i = 0; i < 128; i++)
    {
        ADC_LSB = analogRead(PINANALOG_RNDMGEN) & 0x1;
        ADCSeed ^= ADC_LSB << (i % 32);
    }
    randomSeed(ADCSeed); // Init Arduino random generator
}

void Folder::init_sorted_queue()
{
    currentQueueEntry = 0; // Init: No track played yet.
    while (currentQueueEntry <= trackCount)
    {
        // go through list and init with standard tracks 1-n; trackQueue[0] = 0, [1] = 1, etc.
        // 0th track does not exist!
        ++currentQueueEntry;
        trackQueue[currentQueueEntry] = currentQueueEntry;
    }
    currentQueueEntry = 1; //reset trackCounter
}