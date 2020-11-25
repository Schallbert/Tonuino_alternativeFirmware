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
    virtual void loop() const = 0;
    // Increases volume (<= VOLUME_MAX)
    virtual void volumeUp() const  = 0;
    // Decreases volume (>= VOLUME_MIN)
    virtual void volumeDown() const = 0;
    // Gets next track from queue and starts playback
    virtual void nextTrack() const = 0;
    // Gets previous track from queue and starts playback
    virtual void prevTrack() const = 0;
    virtual void play() const = 0;
    virtual void pause() const = 0;
    virtual void help() const = 0;
    virtual void togglePlayPause() const = 0;
};

#endif // MP3CONTROL_INTERFACE_H