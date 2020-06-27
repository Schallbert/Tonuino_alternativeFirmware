#include "Folder.h"

//Folders

Folder::Folder(uint8_t ui8FolderId, PlayMode ePlayMode, uint8_t ui8TrackCount, EEPROM_interface *pEeprom, uint32_t ui32RndmSeed)
{
    m_ui8FolderId = ui8FolderId;
    m_ePlayMode = ePlayMode;
    m_ui8TrackCount = ui8TrackCount;
    //m_pEeprom = new EEPROM_interface*;
    m_pEeprom = pEeprom;
    m_ui32RndmSeed = ui32RndmSeed;
    m_pTrackQueue = new uint8_t[ui8TrackCount + 1](); // () is to init contents with 0, new to allow dynamically sized array
    init_playmode_related_settings();
}
// Copy Constructor
Folder::Folder(const Folder &cpySrcFolder)
{
    m_ui8FolderId = cpySrcFolder.m_ui8FolderId;
    m_ePlayMode = cpySrcFolder.m_ePlayMode;
    m_ui8TrackCount = cpySrcFolder.m_ui8TrackCount;
    m_pTrackQueue = new uint8_t[cpySrcFolder.m_ui8TrackCount + 1]();
    m_pEeprom = cpySrcFolder.m_pEeprom;
    m_ui32RndmSeed = cpySrcFolder.m_ui32RndmSeed;
    // Deep copy queue 
    for (uint8_t i = 1; i <= cpySrcFolder.m_ui8TrackCount; ++i)
    {
        m_pTrackQueue[i] = cpySrcFolder.m_pTrackQueue[i];
    }
    // copy current track
    m_ui8CurrentQueueEntry = cpySrcFolder.m_ui8CurrentQueueEntry;
}
Folder &Folder::operator=(const Folder &cpySrcFolder)
{
    if (this == &cpySrcFolder)
    {
        return *this;
    }
    m_ui8FolderId = cpySrcFolder.m_ui8FolderId;
    m_ePlayMode = cpySrcFolder.m_ePlayMode;
    m_ui8TrackCount = cpySrcFolder.m_ui8TrackCount;
    m_pTrackQueue = new uint8_t[cpySrcFolder.m_ui8TrackCount + 1]();
    m_pEeprom = cpySrcFolder.m_pEeprom;
    m_ui32RndmSeed = cpySrcFolder.m_ui32RndmSeed;
    // Deep copy queue 
    for (uint8_t i = 0; i <= cpySrcFolder.m_ui8TrackCount; ++i)
    {
        m_pTrackQueue[i] = cpySrcFolder.m_pTrackQueue[i];
    }
    // copy current track
    m_ui8CurrentQueueEntry = cpySrcFolder.m_ui8CurrentQueueEntry;
    return *this;
}
Folder::~Folder()
{
    delete[] m_pTrackQueue;
    m_pEeprom = nullptr; // As this variable is "stack", delete command would call destructor twice --> segfault (6h debugging wasted)
}

bool Folder::is_valid()
{
    if (m_ui8FolderId && m_ui8TrackCount && m_ePlayMode != Folder::UNDEFINED && m_pTrackQueue != nullptr && m_pEeprom != nullptr)
    {
        return true;
    }
    return false;
}

uint8_t Folder::get_folder_id()
{
    return m_ui8FolderId;
}

uint8_t Folder::get_current_track()
{
    if (is_valid())
    {
        return m_pTrackQueue[m_ui8CurrentQueueEntry];
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
    if (m_ui8CurrentQueueEntry < m_ui8TrackCount)
    {
        ++m_ui8CurrentQueueEntry;
    }
    else
    {
        m_ui8CurrentQueueEntry = 1; // Reset queue pointer to first track [1]
    }
    if (m_ePlayMode == PlayMode::SAVEPROGRESS)
    {
#if DEBUGSERIAL
        Serial.print(F("SAVEPROGRESS -> saving track"));
        Serial.println(currentTrack);
#endif
        m_pEeprom->write(m_ui8FolderId, m_pTrackQueue[m_ui8CurrentQueueEntry]);
    }
    return m_pTrackQueue[m_ui8CurrentQueueEntry];
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
    if (m_ui8CurrentQueueEntry > 1)
    {
        --m_ui8CurrentQueueEntry;
    }
    else
    {
        // Reset queue pointer to last track [m_ui8TrackCount]
        m_ui8CurrentQueueEntry = m_ui8TrackCount;
    }
    if (m_ePlayMode == PlayMode::SAVEPROGRESS)
    {
#if DEBUGSERIAL
        Serial.print(F("SAVEPROGRESS -> saving track"));
        Serial.println(currentTrack);
#endif
        m_pEeprom->write(m_ui8FolderId, m_pTrackQueue[m_ui8CurrentQueueEntry]);
    }
    return m_pTrackQueue[m_ui8CurrentQueueEntry];
}

Folder::PlayMode Folder::get_play_mode()
{
    return m_ePlayMode;
}
uint8_t Folder::get_track_count()
{
    return m_ui8TrackCount;
}
// PRIVATE METHODS
void Folder::init_playmode_related_settings()
{
    m_ui8CurrentQueueEntry = 1;
    switch (m_ePlayMode)
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
        init_sorted_queue();
        m_ui8CurrentQueueEntry = m_pEeprom->read(m_ui8FolderId);
        if (m_ui8CurrentQueueEntry > m_ui8TrackCount || m_ui8CurrentQueueEntry == 0)
        {
            // m_pEeprom has never been written, contains some unknown value
            m_ui8CurrentQueueEntry = 1; // set to first track
            m_pEeprom->write(m_ui8FolderId, m_ui8CurrentQueueEntry);
        }
#if DEBUGSERIAL
        Serial.println(F("SAVEPROGRESS -> sorted queue, save current track"));
#endif
        break;
    }
    case PlayMode::ALBUM:
    {
        init_sorted_queue();
#if DEBUGSERIAL
        Serial.println(F("ALBUM -> sorted queue, endless, rollover"));
#endif
        break;
    }
    case PlayMode::LULLABYE:
    {
        init_sorted_queue();
#if DEBUGSERIAL
        Serial.println(F("LULLABYE -> stop playback after lullabye timeout"));
#endif
        break;
    }
    case PlayMode::ONELARGETRACK:
    {
        init_sorted_queue();
#if DEBUGSERIAL
        Serial.println(F("ONELARGETRACK -> sorted queue, stop playback after each track"));
#endif
        break;
    }
    case PlayMode::UNDEFINED:
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
    if (!is_valid())
    {
        // Error: Folder somehow invalid
        init_sorted_queue();
        return;
    }
    uint8_t i = 1; // start at queue[1], queue[0] is always 0!
    uint8_t j = 1;
    uint8_t rnd = 0;
    uint32_t lfsr = m_ui32RndmSeed | 0x01; //Avoid seed being 0, in this case generator will lock up
    bool alreadyInQueue = false;
    // Fill queue with non-repeating, random contents.
    while (i <= m_ui8TrackCount)
    {
        // Calculate pseudo random number based on a XOR'ed shift register
        // Number between 1 and m_ui8TrackCount is acceptable
        while (true)
        {
            lfsr = (lfsr >> 1) ^ ((lfsr & 1) ? 0xd0000001u : 0); /* taps 32 31 29 1 */
            rnd = (uint8_t)(lfsr & 0xFF);
            if ((rnd > 0) && (rnd <= m_ui8TrackCount))
            {
                break;
            }
        }
        j = 1;
        alreadyInQueue = false;
        while (j < i)
        {
            if (m_pTrackQueue[j] == rnd)
            {
                // Random number already used
                alreadyInQueue = true;
                break;
            }
            ++j;
        }
        if (!alreadyInQueue)
        {
            m_pTrackQueue[i] = rnd;
            ++i;
        }
    }
    m_ui8CurrentQueueEntry = 1; //reset m_ui8TrackCounter
}
void Folder::init_sorted_queue()
{
    m_ui8CurrentQueueEntry = 0; // Init: No track played yet.
    while (m_ui8CurrentQueueEntry <= m_ui8TrackCount)
    {
        // go through list and init with standard tracks 1-n; m_pTrackQueue[0] = 0, [1] = 1, etc.
        // 0th track does not exist!
        ++m_ui8CurrentQueueEntry;
        m_pTrackQueue[m_ui8CurrentQueueEntry] = m_ui8CurrentQueueEntry;
    }
    m_ui8CurrentQueueEntry = 1; //reset m_ui8TrackCounter
}