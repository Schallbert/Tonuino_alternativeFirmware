#include "Folder.h"

//Folders
Folder::Folder(uint8_t ui8FolderId,
               ePlayMode ePlayMode,
               uint8_t ui8TrackCount) : m_ui8FolderId(ui8FolderId),
                                        m_ePlayMode(ePlayMode),
                                        m_ui8TrackCount(ui8TrackCount) {}
// Copy Constructor

Folder::Folder(const Folder &cpySrcFolder) : m_ui8FolderId(cpySrcFolder.m_ui8FolderId),
                                             m_ePlayMode(cpySrcFolder.m_ePlayMode),
                                             m_ui8TrackCount(cpySrcFolder.m_ui8TrackCount),
                                             m_pTrackQueue(new uint8_t[cpySrcFolder.m_ui8TrackCount + 1]()),
                                             m_pArduinoHal(cpySrcFolder.m_pArduinoHal)
{
    if (deep_copy_queue(cpySrcFolder.m_pTrackQueue))
    {
        m_ui8CurrentQueueEntry = cpySrcFolder.m_ui8CurrentQueueEntry;
    }
}
// overload assignment operator
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
    m_pArduinoHal = cpySrcFolder.m_pArduinoHal;
    if (deep_copy_queue(cpySrcFolder.m_pTrackQueue))
    {
        m_ui8CurrentQueueEntry = cpySrcFolder.m_ui8CurrentQueueEntry;
    }
    return *this;
}
// Destructor
Folder::~Folder()
{
    if (is_trackQueue_set())
    {
        // only delete if it has been set with new!
        delete[] m_pTrackQueue;
    }
}

bool Folder::deep_copy_queue(uint8_t *pTrackQueue)
{
    // Deep copy queue if initialized
    if (pTrackQueue != nullptr)
    {
        for (uint8_t i = 1; i <= m_ui8TrackCount; ++i)
        {
            m_pTrackQueue[i] = pTrackQueue[i];
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool Folder::is_valid()
{
    if (is_initiated())
    {
        if (is_trackQueue_set())
        {
            return true;
        }
        else if (is_dependency_set())
        {
            setup_track_queue();
            return true;
        }
    }
    return false;
}
bool Folder::is_initiated()
{
    return (m_ui8FolderId && m_ui8TrackCount && m_ePlayMode != Folder::UNDEFINED);
}
bool Folder::is_dependency_set()
{
    // Dependencies only strictly necessary for certain playmodes
    if (m_ePlayMode == Folder::SAVEPROGRESS)
    {
        return (m_pArduinoHal != nullptr);
    }
    else if (m_ePlayMode == Folder::RANDOM)
    {
        return (m_pArduinoHal != nullptr);
    }
    else
        return true; // no dependencies needed
}
bool Folder::is_trackQueue_set()
{
    return (m_pTrackQueue != nullptr);
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

void Folder::setup_dependencies(Arduino_DIcontainer_interface *pArduinoHal)
{
    m_pArduinoHal = pArduinoHal;
    m_pArduinoHal->getRandom()->random_generateSeed(PINANALOG_RNDMGEN);
    is_valid(); // Call to setup play queue in case dependencies are correctly linked
}

uint8_t Folder::get_next_track()
{
    if (!is_valid())
    {
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
    if (m_ePlayMode == ePlayMode::SAVEPROGRESS)
    {
        m_pArduinoHal->getEeprom()->write(m_ui8FolderId, m_pTrackQueue[m_ui8CurrentQueueEntry]);
    }
    return m_pTrackQueue[m_ui8CurrentQueueEntry];
}

uint8_t Folder::get_prev_track()
{
    if (!is_valid())
    {
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
    if (m_ePlayMode == ePlayMode::SAVEPROGRESS)
    {
        m_pArduinoHal->getEeprom()->write(m_ui8FolderId, m_pTrackQueue[m_ui8CurrentQueueEntry]);
    }
    return m_pTrackQueue[m_ui8CurrentQueueEntry];
}

Folder::ePlayMode Folder::get_play_mode()
{
    return m_ePlayMode;
}
uint8_t Folder::get_track_count()
{
    return m_ui8TrackCount;
}
// PRIVATE METHODS
void Folder::setup_track_queue()
{
    m_ui8CurrentQueueEntry = 1;
    m_pTrackQueue = new uint8_t[m_ui8TrackCount + 1](); // () is to init contents with 0, new to allow dynamically sized array
    switch (m_ePlayMode)
    {
    case ePlayMode::RANDOM:
    {
        shuffle_queue();
#if DEBUGSERIAL
        Serial.println(F("SHUFFLE -> random queue"));
#endif
        break;
    }
    case ePlayMode::SAVEPROGRESS:
        init_sorted_queue();
        m_ui8CurrentQueueEntry = m_pArduinoHal->getEeprom()->read(m_ui8FolderId);
        if (m_ui8CurrentQueueEntry > m_ui8TrackCount || m_ui8CurrentQueueEntry == 0)
        {
            //  m_pArduinoHal->getEeprom() has never been written, contains some unknown value
            m_ui8CurrentQueueEntry = 1; // set to first track
            m_pArduinoHal->getEeprom()->write(m_ui8FolderId, m_ui8CurrentQueueEntry);
        }
        break;

    case ePlayMode::ALBUM:
        init_sorted_queue();
        break;

    case ePlayMode::LULLABYE:
        init_sorted_queue();
        break;

    case ePlayMode::ONELARGETRACK:
        init_sorted_queue();
        break;

    case ePlayMode::UNDEFINED:
        break;
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
    Arduino_interface_random *pRnd = m_pArduinoHal->getRandom();
    bool alreadyInQueue = false;
    // Fill queue with non-repeating, random contents.
    while (i <= m_ui8TrackCount)
    {
        // Calculate pseudo random number
        // Number between 1 and m_ui8TrackCount is acceptable
        while (true)
        {
            rnd = pRnd->random_generateUi8();
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
                alreadyInQueue = true; // Random number already used
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