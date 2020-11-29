#include "Mp3Play_implementation.h"

Mp3Play_implementation::Mp3Play_implementation(Arduino_DIcontainer_interface *pArduinoHal,
                                               DfMiniMp3_interface *pDfMini,
                                               SimpleTimer *pLullabyeTimer,
                                               SimpleTimer *pDfMiniMsgTimeout,
                                               MessageHander_interface *pMessage) : m_pArduinoHal(pArduinoHal),
                                                                                 m_pDfMiniMp3(pDfMini),
                                                                                 m_pLullabyeTimer(pLullabyeTimer),
                                                                                 m_pDfMiniPromptTimer(pDfMiniMsgTimeout),
                                                                                 m_pMessageHandler(pMessage)
{

    // Init communication with module and setup
    (m_pArduinoHal->getPins())->pin_mode(DFMINI_PIN_ISIDLE, INPUT);
    m_pDfMiniMp3->setEq(DFMINI_EQ_SETTING);
}

void Mp3Play_implementation::playFolder(Folder &folder) // TODO: Start lullabye timer here?
{
    if (prepareFolderToPlay(folder))
    {
        // Start playing folder: first track of current folder.
        m_pDfMiniMp3->playFolderTrack(m_currentFolder.get_folder_id(),
                                      m_currentFolder.get_current_track());
        m_pMessageHandler->printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::playFolder));
    }
}

bool Mp3Play_implementation::prepareFolderToPlay(Folder &folder)
{
    bool check{true};
    check &= isFolderNew(folder);

    folder.setup_dependencies(m_pArduinoHal);
    folder.setTrackCount(getTrackCountOfFolderOnSdCard(folder));

    check &= isFolderValid(folder);
    return check;
}

bool Mp3Play_implementation::isFolderNew(const Folder &folder) const
{
    return (m_currentFolder.get_folder_id() != folder.get_folder_id());
}

uint8_t Mp3Play_implementation::getTrackCountOfFolderOnSdCard(const Folder &folder) const
{
    return m_pDfMiniMp3->getFolderTrackCount(folder.get_folder_id());
}

bool Mp3Play_implementation::isFolderValid(Folder &folder)
{
    bool result{true};
    if (folder.is_valid())
    {
        m_currentFolder = folder;
    }
    else
    {
        VoicePrompt folderError;
        folderError.promptId = MSG_ERROR_FOLDER;
        folderError.allowSkip = false;
        m_pMessageHandler->printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::noFolder));
        m_pMessageHandler->setPromptMessage(folderError);
        result = false;
    }
    return result;
}

void Mp3Play_implementation::autoplay()
{
    if (m_pDfMiniMp3->isTrackFinished())
    {
        if (shouldPlaybackStop())
        {
            m_pMessageHandler->printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::autoplayStop));
            m_pDfMiniMp3->stop();
        }
        else
        {
            m_pMessageHandler->printMessage(Mp3PlayNotify::toString(Mp3PlayNotify::autoplayNext));
            playNext();
        }
    }
}

bool Mp3Play_implementation::shouldPlaybackStop() const
{
    Folder::ePlayMode mode = m_currentFolder.get_play_mode();
    bool shouldStop{false};
    if (mode == Folder::LULLABYE && m_pLullabyeTimer->isElapsed())
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
        m_pDfMiniMp3->playFolderTrack(m_currentFolder.get_folder_id(),
                                      m_currentFolder.get_next_track());
    }
}

void Mp3Play_implementation::playPrev()
{
    if (isFolderValid(m_currentFolder))
    {
        m_pDfMiniMp3->playFolderTrack(m_currentFolder.get_folder_id(),
                                      m_currentFolder.get_prev_track());
    }
}

void Mp3Play_implementation::playPrompt(const VoicePrompt &prompt) const
{
    m_pDfMiniMp3->stop();

    if (isPromptNew(prompt))
    {
        m_pDfMiniMp3->playAdvertisement(prompt.promptId);
        waitForPromptToStart();

        if (!prompt.allowSkip)
        {
            waitForPromptToFinish();
        }
    }
}

bool Mp3Play_implementation::isPromptNew(const VoicePrompt &prompt) const
{
    bool result{false}; // no need to play already active/played prompt
    static VoicePrompt currentPrompt;
    if (currentPrompt.promptId != prompt.promptId)
    {
        result = true;
        currentPrompt = prompt;
    }
    return result;
}

void Mp3Play_implementation::waitForPromptToStart() const
{
    m_pDfMiniPromptTimer->start(WAIT_DFMINI_READY);
    while (!isPlaying() && !(m_pDfMiniPromptTimer->isElapsed()))
    {
        m_pDfMiniMp3->loop(); //wait for track to start (until timeout kicks in)
    }
    m_pDfMiniPromptTimer->stop();
}

void Mp3Play_implementation::waitForPromptToFinish() const
{
    m_pDfMiniPromptTimer->start(TIMEOUT_PROMPT_PLAYED);
    while (isPlaying() && !(m_pDfMiniPromptTimer->isElapsed()))
    {
        m_pDfMiniMp3->loop(); //wait for track to finish
    }
    m_pDfMiniPromptTimer->stop();
}

bool Mp3Play_implementation::isPlaying() const
{
    return !((m_pArduinoHal->getPins())->digital_read(DFMINI_PIN_ISIDLE));
}