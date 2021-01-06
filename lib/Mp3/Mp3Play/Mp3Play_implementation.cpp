#include "Mp3Play_implementation.h"

#include "Arduino_config.h"

void Mp3Play_implementation::init()
{
    // Init communication with module and setup
    m_rArduinoHal.getPins().pin_mode(DFMINI_PIN_ISIDLE, INPUT);
    m_rDfMiniMp3.setEq(DFMINI_EQ_SETTING);
    m_rDfMiniMp3.setVolume(VOLUME_INIT);
    if(m_rDfMiniMp3.getVolume() == VOLUME_INIT)
    {
        m_rMessageHandler.printMessage("Mp3 player: Volume & EQ set.");
    }
    else{
        m_rMessageHandler.printMessage("Mp3 player: Com ERROR");
    }
}

void Mp3Play_implementation::playFolder(Folder &folder)
{
    if (prepareFolderToPlay(folder))
    {
        // Start playing folder: first track of current folder.
        m_rDfMiniMp3.playFolderTrack(m_currentFolder.getFolderId(),
                                      m_currentFolder.getCurrentTrack());
        m_rMessageHandler.printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::playFolder));
    }
    restartLullabyeTimer();
}

bool Mp3Play_implementation::prepareFolderToPlay(Folder &folder)
{
    bool check{true};
    check &= isFolderNew(folder);

    folder.setupDependencies(&m_rArduinoHal, &m_rMessageHandler);
    folder.setTrackCount(getTrackCountOfFolderOnSdCard(folder));

    check &= isFolderValid(folder);
    return check;
}

bool Mp3Play_implementation::isFolderNew(const Folder &folder) const
{
    return (m_currentFolder.getFolderId() != folder.getFolderId());
}

uint8_t Mp3Play_implementation::getTrackCountOfFolderOnSdCard(const Folder &folder) const
{
    return m_rDfMiniMp3.getFolderTrackCount(folder.getFolderId());
}

void Mp3Play_implementation::restartLullabyeTimer()
{
    m_rLullabyeTimer.stop();
    m_rLullabyeTimer.start(LULLABYE_TIMEOUT_SECS);
}

bool Mp3Play_implementation::isFolderValid(Folder &folder)
{
    bool result{true};
    if (folder.isValid())
    {
        m_currentFolder = folder;
    }
    else
    {
        VoicePrompt folderError;
        folderError.promptId = MSG_ERROR_FOLDER;
        folderError.allowSkip = false;
        m_rMessageHandler.printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::noFolder));
        m_rMessageHandler.promptMessage(folderError);
        result = false;
    }
    return result;
}

void Mp3Play_implementation::autoplay()
{
    if (m_rDfMiniMp3.isTrackFinished())
    {
        if (shouldPlaybackStop())
        {
            m_rMessageHandler.printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::autoplayStop));
            m_rDfMiniMp3.stop();
            restartLullabyeTimer();
        }
        else
        {
            m_rMessageHandler.printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::autoplayNext));
            playNext();
        }
    }
}

bool Mp3Play_implementation::shouldPlaybackStop() const
{
    Folder::ePlayMode mode = m_currentFolder.getPlayMode();
    bool shouldStop{false};
    if (LULLABYE_TIMEOUT_ACTIVE && m_rLullabyeTimer.isElapsed())
    {
        shouldStop = true;
    }
    else if (mode == Folder::ONELARGETRACK)
    {
        shouldStop = true;
    }
    return shouldStop;
}

void Mp3Play_implementation::playNext()
{
    if (isFolderValid(m_currentFolder))
    {
        m_rDfMiniMp3.playFolderTrack(m_currentFolder.getFolderId(),
                                      m_currentFolder.getNextTrack());
    }
}

void Mp3Play_implementation::playPrev()
{
    if (isFolderValid(m_currentFolder))
    {
        m_rDfMiniMp3.playFolderTrack(m_currentFolder.getFolderId(),
                                      m_currentFolder.getPrevTrack());
    }
}