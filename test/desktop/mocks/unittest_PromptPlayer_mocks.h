#ifndef PROMPTPLAYER_MOCKS_H
#define PROMPTPLAYER_MOCKS_H

#include <gmock/gmock.h>

#include "Folder.h"
#include "../Mp3/PromptPlayer/PromptPlayer_interface.h"

class Mock_Mp3Play : public Mp3Play_interface
{
    public:
    MOCK_METHOD(void, playPrompt, (const VoicePrompt &prompt), (const, override));
    MOCK_METHOD(void, playFolder, (Folder &previewFolder), (override));
};

#endif // PROMPTPLAYER_MOCKS_H
