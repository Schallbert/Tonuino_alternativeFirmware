#ifndef PROMPTPLAYER_MOCKS_H
#define PROMPTPLAYER_MOCKS_H

#include <gmock/gmock.h>

#include "../VoiceMenu/PromptPlayer/PromptPlayer_interface.h"

class Mock_PromptPlayer : public PromptPlayer_interface
{
    public:
    MOCK_METHOD(void, checkPlayPrompt, (VoicePrompt prompt), (override));
    MOCK_METHOD(void, checkPlayFolderPreview, (Folder previewFolder), (override));
};

#endif // PROMPTPLAYER_MOCKS_H
