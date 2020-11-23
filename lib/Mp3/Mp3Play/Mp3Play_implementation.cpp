#include "Mp3Play_implementation.h"

Mp3Play_implementation::Mp3Play_implementation(Arduino_DIcontainer_interface *pArduinoHal,
                                               DfMiniMp3_interface *pDfMini,
                                               SimpleTimer *pDfMiniMsgTimeout,
                                               ErrorHandler_interface *pError) : m_pArduinoHal(pArduinoHal),
                                                                                 m_pDfMiniMp3(pDfMini),
                                                                                 m_pDfMiniMsgTimeout(pDfMiniMsgTimeout),
                                                                                 m_pErrorHandler(pError)
{

    // Init communication with module and setup
    (m_pArduinoHal->getPins())->pin_mode(DFMINI_PIN_ISIDLE, INPUT);
    m_pDfMiniMp3->setEq(DFMINI_EQ_SETTING);
}

Folder Mp3Play_implementation::getCurrentFolder()
{
    return m_currentFolder;
}

void Mp3Play_implementation::playFolder(Folder &folder)
{
    if (prepareFolderToPlay(folder))
    {
        // Start playing folder: first track of current folder.
        m_pDfMiniMp3->playFolderTrack(m_currentFolder.get_folder_id(),
                                      m_currentFolder.get_current_track());
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::play);
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
        m_pErrorHandler->setFolderError();
        result = false;
    }
    return result;
}

void Mp3Play_implementation::playPrompt(const VoicePrompt &prompt) const
{
    static VoicePrompt currentPrompt;
    if (currentPrompt.promptId == prompt.promptId)
    {
        return; // no need to play already active/played prompt
    }

    m_pDfMiniMp3->playAdvertisement(prompt.promptId);
    if (!prompt.allowSkip)
    {
        dontSkipCurrentTrack();
    }
    currentPrompt = prompt;
}

void Mp3Play_implementation::dontSkipCurrentTrack() const
{
    //Blocker method to make feature wait until voice prompt has played
    //To ensure following voice prompts do not overwrite current
    m_pDfMiniMsgTimeout->start(WAIT_DFMINI_READY);
    while (!isPlaying() && !(m_pDfMiniMsgTimeout->isElapsed()))
    {
        m_pDfMiniMp3->loop(); //wait for track to start (until timeout kicks in)
    }
    m_pDfMiniMsgTimeout->stop();
    m_pDfMiniMsgTimeout->start(TIMEOUT_PROMPT_PLAYED);
    while (isPlaying() && !(m_pDfMiniMsgTimeout->isElapsed()))
    {
        m_pDfMiniMp3->loop(); //wait for track to finish
    }
    m_pDfMiniMsgTimeout->stop();
}

bool Mp3Play_implementation::isPlaying() const
{
    return !((m_pArduinoHal->getPins())->digital_read(DFMINI_PIN_ISIDLE));
}