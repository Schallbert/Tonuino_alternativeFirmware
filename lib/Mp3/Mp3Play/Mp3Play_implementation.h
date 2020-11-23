#ifndef MP3PLAY_IMPLEMENTATION_H
#define MP3PLAY_IMPLEMENTATION_H

#include "Folder.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"
#include "../ErrorHandler/ErrorHandler_interface.h"
#include "../Utilities/SimpleTimer.h"

#include "Tonuino_config.h"

#include "Mp3Play_interface.h"

class Mp3Play_implementation : public Mp3Play_interface
{

public:
    Mp3Play_implementation(Arduino_DIcontainer_interface *pArduinoHal,
                           DfMiniMp3_interface *pDfMini,
                           SimpleTimer *pDfMiniMsgTimeout,
                           ErrorHandler_interface *pError);

    void playPrompt(const VoicePrompt &prompt) const override;
    void playFolder(Folder &folder) override;
    bool isPlaying() const override;
    Folder getCurrentFolder() override;

private:
    void dontSkipCurrentTrack() const;

    bool prepareFolderToPlay(Folder &folder);
    bool isFolderNew(const Folder &folder) const;
    bool isFolderValid(Folder &folder);
    uint8_t getTrackCountOfFolderOnSdCard(const Folder &folder) const;

private:
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    DfMiniMp3_interface *m_pDfMiniMp3{nullptr};
    SimpleTimer *m_pDfMiniPromptTimer{nullptr};
    ErrorHandler_interface *m_pErrorHandler{nullptr};

    Folder m_currentFolder{};
};

#endif // MP3PLAY_IMPLEMENTATION_H