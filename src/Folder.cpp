#include "Folder.h"

//Folders
Folder::Folder()
{
    folderId = 0;
    playMode = Folder::UNDEFINED;
    trackCount = 0;
    trackQueue = uint8_t();
}
Folder::Folder(uint8_t folderId, PlayMode playMode, uint8_t trackCount)
{
    this->folderId = folderId;
    this->playMode = playMode;
    this->trackCount = trackCount;
    trackQueue = new uint8_t[trackCount + 1](); // () is to init contents with 0
    init_sorted_queue();
    switch (playMode)
    {
    case PlayMode::RANDOM:
    {
        shuffle_queue();
#if DEBUGSERIAL
        Serial.println(F("SHUFFLE -> random queue"));
#endif
        break;
    }
    case PlayMode::SAVEPROGRESS:
    {
        currentQueueEntry = EEPROM.read(folderId);
        // TODO: Complete
#if DEBUGSERIAL
        Serial.println(F("SAVEPROGRESS -> sorted queue, save current track"));
#endif
        break;
    }
    case PlayMode::ALBUM:
    {
#if DEBUGSERIAL
        Serial.println(F("ALBUM -> sorted queue, endless, rollover"));
#endif
        break;
    }
    case PlayMode::LULLABYE:
    {
#if DEBUGSERIAL
        Serial.println(F("LULLABYE -> stop playback after lullabye timeout"));
#endif
        break;
    }
    case PlayMode::ONELARGETRACK:
    {
#if DEBUGSERIAL
        Serial.println(F("ONELARGETRACK -> sorted queue, save current track"));
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
    if(folderId && trackCount && playMode != Folder::UNDEFINED)
    {
        return true;
    }
    return false;
}

uint8_t Folder::get_folder_id()
{
    return folderId;
}

void Folder::init_sorted_queue()
{
    currentQueueEntry = 0; // Init: No track played yet.
    while (currentQueueEntry <= trackCount)
    {
        // go through list and init with standard tracks 1-n; trackQueue[0] = 0, [1] = 1, etc.
        // 0th track does not exist!
        ++currentQueueEntry;
        ++trackQueue;
        *trackQueue = currentQueueEntry;
    }
    //reset trackQueue to 0th element
    trackQueue = trackQueue - currentQueueEntry;
    currentQueueEntry = 0;
}
void Folder::save_progress()
{
    // TODO: IMPLEMENT!
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
        ++trackQueue;
    }
    else
    {
        // Reset queue pointer to first track [1]
        currentQueueEntry = 1;
        trackQueue = trackQueue - (trackCount - currentQueueEntry);
    }
    if (playMode == PlayMode::SAVEPROGRESS)
    {
#if DEBUGSERIAL
        Serial.print(F("SAVEPROGRESS -> saving track"));
        Serial.println(currentTrack);
#endif
        EEPROM.update(folderId, *trackQueue);
    }
    return *trackQueue;
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
        --trackQueue;
    }
    else
    {
        // Reset queue pointer to last track [trackCount]
        currentQueueEntry = trackCount;
        trackQueue = trackQueue + (currentQueueEntry - 1);
    }
    if (playMode == PlayMode::SAVEPROGRESS)
    {
#if DEBUGSERIAL
        Serial.print(F("SAVEPROGRESS -> saving track"));
        Serial.println(currentTrack);
#endif
        EEPROM.update(folderId, *trackQueue);
    }
    return *trackQueue;
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
    init_random_generator();          // For random playlists
    long rnd = random(1, trackCount); //get random number between 1 and trackCount
}
/* TODO: Implement!
        uint8_t randomNumber = getRandom(); // exclude 0 and >trackcount!
        uint8_t i = 0;
        uint8_t j = 0;
        while(i < )

        char result[...];
memcpy(result, chars, sizeof(chars)); 
for (unsigned i = 0; i < (sizeof(chars)-1); ++i) {
    unsigned j = rand() % sizeof(chars);
    char tmp = result[j];
    result[j] = result[i];
    result[i] = tmp;
}
*/
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