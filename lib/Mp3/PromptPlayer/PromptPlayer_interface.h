#ifndef PROMPTPLAYER_H
#define PROMPTPLAYER_H

#include "Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"


class PromptPlayer_interface
{
public:
    virtual void playPrompt(VoicePrompt &prompt) = 0;
    virtual void playFolderPreview(Folder previewFolder) = 0;
};

#endif // PROMPTPLAYER_H