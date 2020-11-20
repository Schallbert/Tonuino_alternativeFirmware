#ifndef MP3CONTROL_INTERFACE_H
#define MP3CONTROL_INTERFACE_H

#include "../Arduino/Arduino_interface/Arduino_types.h"

class Folder;

struct VoicePrompt
    {
        uint16_t promptId{0};
        bool allowSkip{true};
    };

// API to control the MP3 player hardware
class Mp3Control_interface
{
public:
    virtual ~Mp3Control_interface(){};

public:
    // Call in main loop. Listens for DFminiMp3 replies & autoplays next song
    virtual void loop() = 0;
    // Increases volume (<= VOLUME_MAX)
    virtual void volume_up() = 0;
    // Decreases volume (>= VOLUME_MIN)
    virtual void volume_down() = 0;
    // Gets next track from queue and starts playback
    virtual void next_track() = 0;
    // Gets previous track from queue and starts playback
    virtual void prev_track() = 0;
    // If playing, pauses track and vice versa
    virtual void play_pause() = 0;
    // Returns true if DFminiMp3 is currently playing
    virtual bool is_playing() const = 0;
    // Starts playback of specified folder (handles queueing and folder specific playmodes)
    virtual void play_folder(Folder *m_pCurrentFolder) = 0;
    // Plays specific file on sd:/advert/####fileId
    virtual void play_prompt(const VoicePrompt &prompt) const = 0;
    // Starts inquiry to player to return number of tracks in selected folder.
    virtual uint8_t get_trackCount_of_folder(uint8_t folderId) = 0;
};

#endif // MP3CONTROL_INTERFACE_H