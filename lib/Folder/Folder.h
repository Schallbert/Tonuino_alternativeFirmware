#ifndef FOLDER_H
#define FOLDER_H

#include "../Arduino/Arduino_DIcontainer_interface.h"
#include "../MessageHandler/MessageHandler_interface.h"

class Folder
{
public:
    enum ePlayMode
    {
        UNDEFINED = 0,     // Not implemented
        ALBUM = 1,         // Play tracks in order on SD card (1,2,3...), endless, rollover.
        RANDOM = 2,        // Like ALBUM but with shuffled non-repeating queue. TODO: RECORD VOICE OUTPUT
        SAVEPROGRESS = 3,  // like ALBUM but saves track that is currently active. TODO: RE-RECORD VOICE OUTPUT
        ONELARGETRACK = 4, // So-called Hörspielmodus. Queue like ALBUM but stops playback after finishing track. TODO: RE-RECORD VOICE OUTPUT.
        ENUM_COUNT = 4    // Last entry of enum to allow iteration (no value for content)
    };

public:
    Folder(){};
    Folder(uint8_t ui8FolderId, ePlayMode ePlayMode);
    Folder(const Folder &cpySrcFolder);
    Folder &operator=(const Folder &cpySrcFolder); // = operator
    ~Folder();

public:
    // Returns current track in queue
    uint8_t getCurrentTrack();
    // Increments queue pointer (rollover) and returns next track [number]
    uint8_t getNextTrack(); // External dependency: EEPROM
    // Decrements queue pointer (rollover) and returns previous track [number]
    uint8_t getPrevTrack(); // External dependency: EEPROM
    // Returns folder's play mode [enum]
    ePlayMode getPlayMode() const;
    // Returns folder's id [number]
    uint8_t getFolderId() const;
    // Returns track count of folder [number], yielded from MP3 player request
    uint8_t getTrackCount() const;
    // Tries to initiate the track queue by using injected dependencies depending on play mode
    void setupDependencies(Arduino_DIcontainer_interface *pArduinoHal,
                            MessageHander_interface *pMessageHandler); // Dependency injection: Random seed & eeprom
    // sets trackCount of folder
    void setTrackCount(uint8_t trackCount);

    // Returns true if the folder can be fully setup and is ready to be used in other modules
    bool isValid();
    // Returns true if folder is setup with relevant data (id, track count, playmode)
    bool isInitiated() const;

private:
    // Sets the queue pointer to first track and initializes the queue based on playMode
    void setup_track_queue();
    // Creates a sorted play queue (1= first track, 2= second track etc.)
    void init_sorted_queue();
    // Creates a 1:1 copy of the input track queue and saves in member variable
    void deep_copy_queue(uint8_t *pTrackQueue);
    // Creates a Pseudo random queue, each track only once in queue
    void shuffle_queue();
    // Returns true if folder is bound to necessary external dependencies (eeprom, random seed)
    bool is_dependency_set();
    // Returns true if folder's track queue has been initialized
    bool is_trackQueue_set();

private:
    uint8_t m_ui8FolderId{0};
    ePlayMode m_ePlayMode{Folder::UNDEFINED};
    uint8_t m_ui8TrackCount{0};

    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};

    uint8_t *m_pTrackQueue{nullptr};
    uint8_t m_ui8CurrentQueueEntry{0};
};

class FolderNotify
{
private:
    // Disallow creating an instance of this object
    FolderNotify(){};

public:
    static inline const char *toString(Folder::ePlayMode value)
    {
#if DEBUGSERIAL
        static const char *NOTIFY_STRING[] = {
            nullptr,
            "Playmode Lullabye",
            "Playmode Album",
            "PLaymode Random",
            "Playmode SaveProgress",
            "Playmode OneLargeTrack"};
        return NOTIFY_STRING[value];
#endif
        return nullptr;
    };
};

#endif // FOLDER_H