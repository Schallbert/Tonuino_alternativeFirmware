#ifndef FOLDER_H
#define FOLDER_H
 
#include <EEPROM_interface.h>

class Folder
{
public:
    /*
        Implementation details:
        LULLABYE: Mp3PlayerControl's autoplay method
        ALBUM:  folder's constructor
        RANDOM: folder's shuffle method
        SAVEPROGRESS: folder's next/prev methods
        ONELARGETRACK: Mp3PlayerControl's autoplay method
    */
    enum ePlayMode
    {
        UNDEFINED = 0,      // Not implemented
        LULLABYE = 4,       // like ALBUM but stops playback after timeout TODO: RECORD VOICE OUTPUT.
        ALBUM = 2,          // Play tracks in order on SD card (1,2,3...), endless, rollover.
        RANDOM = 3,         // Like ALBUM but with shuffled non-repeating queue. TODO: RECORD VOICE OUTPUT
        SAVEPROGRESS = 5,   // like ALBUM but saves track that is currently active. TODO: RE-RECORD VOICE OUTPUT
        ONELARGETRACK = 1,  // So-called Hörspielmodus. Queue like ALBUM but stops playback after finishing track. TODO: RE-RECORD VOICE OUTPUT.
        ENUM_COUNT = 5,     // Last entry of enum to allow iteration (no value for content)
    };

public:
    Folder(){};
    Folder(uint8_t ui8FolderId, ePlayMode ePlayMode, uint8_t ui8TrackCount);
    Folder(const Folder &cpySrcFolder);
    Folder& operator=(const Folder &cpySrcFolder); // = operator
    ~Folder();

public:
    // Returns current track in queue
    uint8_t get_current_track();
    // Increments queue pointer (rollover) and returns next track [number]
    uint8_t get_next_track(); // External dependency: EEPROM
    // Decrements queue pointer (rollover) and returns previous track [number]
    uint8_t get_prev_track(); // External dependency: EEPROM
    // Returns folder's play mode [enum]
    ePlayMode get_play_mode();
    // Returns folder's id [number]
    uint8_t get_folder_id();
    // Returns track count of folder [number], yielded from MP3 player request
    uint8_t get_track_count();
    // Tries to initiate the track queue by using injected dependencies depending on play mode
    void setup_dependencies(EEPROM_interface* pEeprom, uint32_t ui32RndmSeed); // Dependency injection: Random seed & eeprom
    // Returns true if the folder can be fully setup and is ready to be used in other modules
    bool is_valid();
    // Returns true if folder is setup with relevant data (id, track count, playmode)
    bool is_initiated();

private:
    // Sets the queue pointer to first track and initializes the queue based on playMode
    void setup_track_queue();
    // Creates a sorted play queue (1= first track, 2= second track etc.)
    void init_sorted_queue();
    // Creates a Pseudo random queue without repeat
    void shuffle_queue();
    // Returns true if folder is bound to necessary external dependencies (eeprom, random seed)
    bool is_dependency_set();
    // Returns true if folder's track queue has been initialized
    bool is_trackQueue_set();

private:
    uint8_t* m_pTrackQueue {nullptr};
    uint8_t m_ui8TrackCount {0};
    uint8_t m_ui8CurrentQueueEntry {0};
    uint8_t m_ui8FolderId {0};
    ePlayMode m_ePlayMode {Folder::UNDEFINED};
    EEPROM_interface* m_pEeprom {nullptr};
    uint32_t m_ui32RndmSeed {0};
};

#endif // FOLDER_H