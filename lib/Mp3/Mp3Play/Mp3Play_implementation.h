#ifndef MP3PLAY_IMPLEMENTATION_H
#define MP3PLAY_IMPLEMENTATION_H

#include "Folder.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"
#include "../ErrorHandler/ErrorHandler_interface.h"
#include "../Utilities/SimpleTimer.h"

#include "Tonuino_config.h"
#include "Mp3Play_interface.h"

class Mp3PlayNotify
{
public:
    enum eDebugMessage
    {
        noMessage = 0,
        noFolder,
        playFolder,
        autoplayStop,
        autoplayNext,
    };

#if DEBUGSERIAL
    static const char *toString(eDebugMessage message)
    {
        static const char *NOTIFY_STRING[] = {
            nullptr,
            "Error: folder invalid!",
            "Folder detected. Play"
            "autoplay complete. Pause",
            "autoplay next"};

        return NOTIFY_STRING[message];
    };

#else
    static const char *toString(eDebugMessage message)
    {
        return nullptr;
    }
#endif
};

class Mp3Play_implementation : public Mp3Play_interface
{

public:
    Mp3Play_implementation(Arduino_DIcontainer_interface *pArduinoHal,
                           DfMiniMp3_interface *pDfMini,
                           SimpleTimer *pLullabyeTimer,
                           SimpleTimer *pDfMiniMsgTimeout,
                           MessageHander_interface *pMsgHandler);

    void playPrompt(const VoicePrompt &prompt) const override;

    void playFolder(Folder &folder) override;
    void playPrev() override;
    void playNext() override;
    void autoplay() override;

    bool isPlaying() const override;

private:
    bool shouldPlaybackStop() const;

    bool isPromptNew(const VoicePrompt &prompt) const;
    void waitForPromptToStart() const;
    void waitForPromptToFinish() const;

    bool prepareFolderToPlay(Folder &folder);
    bool isFolderNew(const Folder &folder) const;
    bool isFolderValid(Folder &folder);
    uint8_t getTrackCountOfFolderOnSdCard(const Folder &folder) const;

private:
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    DfMiniMp3_interface *m_pDfMiniMp3{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pDfMiniPromptTimer{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};

    Folder m_currentFolder{};
};

#endif // MP3PLAY_IMPLEMENTATION_H