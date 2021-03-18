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
    folder.setupDependencies(&m_rArduinoHal, &m_rMessageHandler);
    bool countValid = folder.setTrackCount(getTrackCountOfFolderOnSdCard(folder));

    if (!countValid)
    {
        VoicePrompt tooManyTracks(VoicePrompt::MSG_ERROR_TOOMANYTRACKS, VoicePrompt::PROMPT_ALLOWSKIP);
        m_rMp3Prompt.playPrompt(tooManyTracks);
    }

    if (!checkFolder(folder))
    {
        return false;
    }

    // Folder already cached?
    if (folder.getFolderId() == m_currentFolder.getFolderId())
    {
        return false;
    }

    m_currentFolder = folder;

    return true;
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

bool Mp3Play_implementation::checkFolder(Folder &folder)
{
    if (!folder.isValid())
    {
        m_rMessageHandler.printMessage(Message::ERRORFOLDER);
        VoicePrompt folderErrorPrompt(VoicePrompt::MSG_ERROR_FOLDER, VoicePrompt::PROMPT_NOSKIP);
        m_rMp3Prompt.playPrompt(folderErrorPrompt);
        return false;
    }
    return true;
}

void Mp3Play_implementation::autoplay()
{
    if (!m_rDfMiniMp3.isTrackFinished())
    {
        return;
    }

    Message::eMessageContent autoplayInfo{Message::AUTOPLAYPAUSE};
    if (shouldPlaybackStop())
    {
        m_rMessageHandler.printMessage(autoplayInfo);
        m_rDfMiniMp3.pause();
        restartLullabyeTimer(); // Lullabye timer gets restarted until a track is playing.
    }
    else
    {
        autoplayInfo = Message::AUTOPLAYNEXT;
        m_rMessageHandler.printMessage(autoplayInfo);
        playNext();
    }
}

bool Mp3Play_implementation::shouldPlaybackStop() const
{
    if (!m_currentFolder.isInitiated() ||
        (LULLABYE_TIMEOUT_ACTIVE && m_rLullabyeTimer.isElapsed()) ||
        (m_currentFolder.getPlayMode() == Folder::ONELARGETRACK))
    {
        return true;
    }

    return false;
}

void Mp3Play_implementation::playNext()
{
    if (m_currentFolder.isValid())
    {
        m_rDfMiniMp3.playFolderTrack(m_currentFolder.getFolderId(),
                                     m_currentFolder.getNextTrack());
        restartLullabyeTimer();
    }
}

void Mp3Play_implementation::playPrev()
{
    if (m_currentFolder.isValid())
    {
        m_rDfMiniMp3.playFolderTrack(m_currentFolder.getFolderId(),
                                     m_currentFolder.getPrevTrack());
        restartLullabyeTimer();
    }
}