#include "Folder.h"

//Folders
Folder::Folder()
{
    folderId = 0;
    playMode = Folder::UNDEFINED;
    trackCount = 0;
    trackQueue = nullptr;
    eeprom = nullptr;
    rndmSeed = 0;
}

Folder::Folder(uint8_t folderId, PlayMode playMode, uint8_t trackCount
               , EEPROM_interface* eeprom, uint32_t rndmSeed)
{
    this->folderId = folderId;
    this->playMode = playMode;
    this->trackCount = trackCount;
    this->eeprom = eeprom;
    this->rndmSeed = rndmSeed;
    trackQueue = new uint8_t[trackCount + 1](); // () is to init contents with 0, new to allow dynamically sized array
    currentQueueEntry = 1;
    init_playmode_related_settings();
}
// Copy Constructor
Folder::Folder(const Folder& cpySrcFolder) 
{
    folderId = 0;
    playMode = Folder::UNDEFINED;
    trackCount = 0;
    trackQueue = nullptr;
    eeprom = nullptr;
    rndmSeed = 0;
    /*
    folderId = cpySrcFolder.folderId;
    playMode =cpySrcFolder.playMode;
    trackCount = cpySrcFolder.trackCount;
    trackQueue = new uint8_t[cpySrcFolder.trackCount + 1]();
    eeprom = cpySrcFolder.eeprom;
    rndmSeed = cpySrcFolder.rndmSeed;
    
    for(uint8_t i = 0; i <= cpySrcFolder.trackCount; ++i)
    {
        trackQueue[i] = cpySrcFolder.trackQueue[i];
    } 
    */
} 
Folder& Folder::operator=(const Folder &cpySrcFolder)
{
    //folderId = 0;
    //playMode = Folder::UNDEFINED;
    //trackCount = 0;
    //trackQueue = nullptr;
    //eeprom = nullptr;
    
    folderId = cpySrcFolder.folderId;
    playMode =cpySrcFolder.playMode;
    trackCount = cpySrcFolder.trackCount;
    
    trackQueue = new uint8_t[cpySrcFolder.trackCount + 1]();
    //eeprom = cpySrcFolder.eeprom; // SEGMENTATION FAULT
    rndmSeed = cpySrcFolder.rndmSeed;
    
    for(uint8_t i = 0; i <= cpySrcFolder.trackCount; ++i)
    {
        trackQueue[i] = i;//cpySrcFolder.trackQueue[i];
    }
    return *this;
    
}

Folder::~Folder()
{
    delete[] trackQueue;
    delete eeprom;
}

bool Folder::is_valid()
{
    if (folderId && trackCount && playMode != Folder::UNDEFINED && trackQueue != nullptr && eeprom != nullptr)
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
    if (is_valid())
    {
        return trackQueue[currentQueueEntry];
    }
    else
    {
        return 0;
    } 
}

uint8_t Folder::get_next_track()
{
    if (!is_valid())
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
        eeprom->write(folderId, trackQueue[currentQueueEntry]);
    }
    return trackQueue[currentQueueEntry];
}

uint8_t Folder::get_prev_track()
{
    if (!is_valid())
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
        eeprom->write(folderId,  trackQueue[currentQueueEntry]);
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
// PRIVATE METHODS
void Folder::init_playmode_related_settings()
{
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
        init_sorted_queue();
        currentQueueEntry = eeprom->read(folderId);
        if(currentQueueEntry > trackCount || currentQueueEntry == 0)
        {
            // eeprom has never been written, contains some unknown value
            currentQueueEntry = 1; // set to first track
            eeprom->write(folderId, currentQueueEntry);
        }
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
void Folder::shuffle_queue()
{
    if(!is_valid())
    {
        // Error: Folder somehow invalid
        init_sorted_queue();
        return;
    }
    uint8_t i = 1; // start at queue[1], queue[0] is always 0!
    uint8_t j = 1;
    uint8_t rnd = 0;
    uint32_t lfsr = rndmSeed | 0x01; //Avoid seed being 0, in this case generator will lock up
    bool alreadyInQueue = false;
    // Fill queue with non-repeating, random contents.
    while (i <= trackCount)
    {
        // Calculate pseudo random number based on a XOR'ed shift register
        // Number between 1 and trackCount is acceptable
        while(true)
        {
            lfsr = (lfsr >> 1) ^ ((lfsr & 1)? 0xd0000001u: 0); /* taps 32 31 29 1 */
            rnd = (uint8_t)(lfsr & 0xFF);
            if((rnd > 0) && (rnd <= trackCount))
            {
                break;
            }
        }
        j = 1;
        alreadyInQueue = false;
        while (j < i)
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
    currentQueueEntry = 1; //reset trackCounter
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