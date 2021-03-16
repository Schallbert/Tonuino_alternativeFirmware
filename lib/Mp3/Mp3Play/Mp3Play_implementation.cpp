#include "Mp3Play_implementation.h"

#include "Arduino_config.h"

void Mp3Play_implementation::init()
{
    // Init communication with module and setup
    m_rArduinoHal.getPins().pin_mode(DFMINI_PIN_ISIDLE, INPUT);
}

void Mp3Play_implementation::playFolder(Folder &folder)
{
    if (prepareFolderToPlay(folder))
    {
        // Start playing folder: first track of current folder.
        m_rDfMiniMp3.playFolderTrack(m_currentFolder.getFolderId(),
                                     m_currentFolder.getCurrentTrack());
        m_rMessageHandler.printMessage(Message::FOLDEROKPLAY);
    }
    restartLullabyeTimer();
}

bool Mp3Play_implementation::prepareFolderToPlay(Folder &folder)
{
    bool check{true};
    bool countValid{true};
    check &= isFolderNew(folder);

    folder.setupDependencies(&m_rArduinoHal, &m_rMessageHandler);
    countValid = folder.setTrackCount(getTrackCountOfFolderOnSdCard(folder));
    if (!countValid)
    {
        VoicePrompt tooManyTracks(VoicePrompt::MSG_ERROR_TOOMANYTRACKS, VoicePrompt::PROMPT_ALLOWSKIP);
        m_rMp3Prompt.playPrompt(tooManyTracks);
    }
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
        m_rMessageHandler.printMessage(Message::ERRORFOLDER);
         VoicePrompt folderErrorPrompt(VoicePrompt::MSG_ERROR_FOLDER, VoicePrompt::PROMPT_NOSKIP);
        m_rMp3Prompt.playPrompt(folderErrorPrompt);
        result = false;
    }
    return result;
}

void Mp3Play_implementation::autoplay()
{
    if (m_rDfMiniMp3.isTrackFinished())
    {
        Message::eMessageContent autoplayInfo{Message::AUTOPLAYPAUSE};
        if (shouldPlaybackStop())
        {
            m_rMessageHandler.printMessage(autoplayInfo);
            m_rDfMiniMp3.stop();
            restartLullabyeTimer(); // Lullabye timer gets restarted until a track is playing.
        }
        else
        {
            autoplayInfo = Message::AUTOPLAYNEXT;
            m_rMessageHandler.printMessage(autoplayInfo);
            playNext();
        }
    }
}

bool Mp3Play_implementation::shouldPlaybackStop() const
{
    bool shouldStop{false};

    if (!m_currentFolder.isInitiated())
    {
        shouldStop = true;
    }

    if (LULLABYE_TIMEOUT_ACTIVE && m_rLullabyeTimer.isElapsed())
    {
        shouldStop = true;
    }

    if (m_currentFolder.getPlayMode() == Folder::ONELARGETRACK)
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