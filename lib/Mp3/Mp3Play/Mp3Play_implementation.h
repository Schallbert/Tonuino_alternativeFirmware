#ifndef MP3PLAY_IMPLEMENTATION_H
#define MP3PLAY_IMPLEMENTATION_H

#include "Folder.h"
#include "../Arduino_HardwareAbstraction/Arduino_DIcontainer_interface.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "Mp3Prompt/Mp3Prompt_interface.h"
#include "../MessageHandler/MessageHandler_interface.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

#include "Tonuino_config.h"
#include "Mp3Play_interface.h"

class Mp3Play_implementation : public Mp3Play_interface
{

public:
    Mp3Play_implementation(Arduino_DIcontainer_interface &rArduinoHal,
                           DfMiniMp3_interface &rDfMini,
                           Mp3Prompt_interface &rPrompt,
                           SimpleTimer &rLullabyeTimer,
                           MessageHander_interface &rMessage) : m_rArduinoHal(rArduinoHal),
                                                                m_rDfMiniMp3(rDfMini),
                                                                m_rMp3Prompt(rPrompt),
                                                                m_rLullabyeTimer(rLullabyeTimer),
                                                                m_rMessageHandler(rMessage){};
    ~Mp3Play_implementation() = default;

    void init();
    void playFolder(Folder &folder) override;
    void playPrev() override;
    void playNext() override;
    void autoplay() override;

private:
    bool shouldPlaybackStop() const;
    void restartLullabyeTimer();

    bool prepareFolderToPlay(Folder &folder);
    bool isFolderNew(const Folder &folder) const;
    bool isFolderValid(Folder &folder);
    uint8_t getTrackCountOfFolderOnSdCard(const Folder &folder) const;

private:
    Arduino_DIcontainer_interface &m_rArduinoHal;
    DfMiniMp3_interface &m_rDfMiniMp3;
    Mp3Prompt_interface &m_rMp3Prompt;
    SimpleTimer &m_rLullabyeTimer;
    MessageHander_interface &m_rMessageHandler;

    Folder m_currentFolder{};
};

#endif // MP3PLAY_IMPLEMENTATION_H