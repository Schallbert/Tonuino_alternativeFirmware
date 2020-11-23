#ifndef MP3PLAY_INTERFACE_H
#define MP3PLAY_INTERFACE_H

#include "Mp3Control/Mp3Control_interface.h"

class Mp3Play_interface
{
public:
    // Plays specific file
    virtual void playPrompt(const VoicePrompt &prompt) const = 0;
    // Starts inquiry to player to return number of tracks in selected folder.
    virtual void playFolder(Folder &folder) = 0;
    // Returns true if currently playing
    virtual bool isPlaying() const = 0;
    // Returns the folder that is currently playing
    virtual Folder getCurrentFolder() = 0;
};

#endif // MP3PLAY_INTERFACE_H