#ifndef PLAYBACKCONTROL_H
#define PLAYBACKCONTROL_H

#include "../UserInput/UserInput_interface/UserInput_interface.h"
#include "../Mp3/Mp3Control/Mp3Control_interface.h"

class PlaybackControl
{
public:
    PlaybackControl(Mp3Control_interface *pMp3Ctrl) : m_pMp3Control(pMp3Ctrl){};
    // Dispatches cardState and userInput commands, calling downstream methods.
    void handleUserInput(UserInput::eUserRequest input);

private:
    // ----- Wrapper methods to call target object's methods -----
    // Reads NFC tag and starts playback
    // No action performed
    void none(){};
    // Play/pause
    void plPs() { m_pMp3Control->togglePlayPause(); };
    // next track
    void next() { m_pMp3Control->nextTrack(); };
    // previous track
    void prev() { m_pMp3Control->prevTrack(); };
    // increase volume
    void incV() { m_pMp3Control->volumeUp(); };
    // decrease volume
    void decV() { m_pMp3Control->volumeDown(); };
    // play help prompt
    void help() { m_pMp3Control->help(); };

private:
    typedef void (PlaybackControl::*dispatcher)(); // table of function pointers
    // members by dependency injection
    Mp3Control_interface *m_pMp3Control{nullptr};
};
#endif // PLAYBACKCONTROL_H
