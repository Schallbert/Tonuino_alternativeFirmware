#include "Mp3Control_implementation.h"

Mp3Control::Mp3Control(Arduino_DIcontainer_interface *pArduinoHal,
                                   DfMiniMp3_interface *pPlayer,
                                   SimpleTimer *pLullabyeTimer,
                                   SimpleTimer *pDfMiniMsgTimeout) : m_pArduinoHal(pArduinoHal),
                                                                     m_pDfMiniMp3(pPlayer),
                                                                     m_pLullabyeTimer(pLullabyeTimer),
                                                                     m_pDfMiniMsgTimeout(pDfMiniMsgTimeout)
{

    // Init communication with module and setup
    m_pArduinoHal->getPins()->pin_mode(DFMINI_PIN_ISIDLE, INPUT);
    m_pDfMiniMp3->begin(); // Init
    waitForPlayerReady();
    m_pDfMiniMp3->setEq(DFMINI_EQ_SETTING);
    m_pDfMiniMp3->setVolume(VOLUME_INIT);
    m_debugMessage = noMessage;
}
void Mp3Control::loop()
{
    waitForPlayerReady();
    autoplay();
}
void Mp3Control::volume_up()
{
    waitForPlayerReady();
    if (m_pDfMiniMp3->getVolume() < VOLUME_MAX)
    {
        waitForPlayerReady();
        m_pDfMiniMp3->increaseVolume();
    }
    m_debugMessage = volumeUp;
}
void Mp3Control::volume_down()
{
    waitForPlayerReady();
    if (m_pDfMiniMp3->getVolume() > VOLUME_MIN)
    {
        waitForPlayerReady();
        m_pDfMiniMp3->decreaseVolume();
    }
    m_debugMessage = volumeDown;
}
bool Mp3Control::is_playing() const
{
    return !(m_pArduinoHal->getPins()->digital_read(DFMINI_PIN_ISIDLE));
}
void Mp3Control::play_pause()
{
    waitForPlayerReady();
    if (is_playing())
    {
        m_pDfMiniMp3->pause();
    }
    else
    {
        m_pLullabyeTimer->start(LULLABYE_TIMEOUT_SECS); // start shutdown timer
        waitForPlayerReady();
        m_pDfMiniMp3->start(); // Only successful if a track is entered.
    }
}

// Routine to check playmode and select next track
void Mp3Control::autoplay()
{
    // Autoplay implementation
    if (m_pDfMiniMp3->checkTrackFinished())
    {
        Folder::ePlayMode mode = m_pCurrentFolder->get_play_mode();
        if (mode == Folder::ONELARGETRACK)
        {
            m_debugMessage = autoplayOneLargeTrack;
            waitForPlayerReady();
            m_pDfMiniMp3->stop();
            return;
        }
        else if (mode == Folder::LULLABYE && m_pLullabyeTimer->isElapsed())
        {
            m_debugMessage = autoplayLullabye;
            waitForPlayerReady();
            m_pDfMiniMp3->stop();
            return;
        }
        else
        {
            m_debugMessage = autoplayNext;
            next_track();
        }
    }
}

void Mp3Control::next_track()
{
    if (!isFolderValid())
    {
        m_debugMessage = next_noFolder;
        waitForPlayerReady();
        m_pDfMiniMp3->playAdvertisement(MSG_ERROR_FOLDER);
        return; // Cannot play a track if the card is not linked.
    }
    waitForPlayerReady();
    m_pDfMiniMp3->playFolderTrack(m_pCurrentFolder->get_folder_id(),
                                  m_pCurrentFolder->get_next_track());

    m_debugMessage = next;
}

void Mp3Control::prev_track()
{
    if (!isFolderValid())
    {
        m_debugMessage = prev_noFolder;
        waitForPlayerReady();
        m_pDfMiniMp3->playAdvertisement(MSG_ERROR_FOLDER);
        return; // Cannot play a track if the card is not linked.
    }
    waitForPlayerReady();
    m_pDfMiniMp3->playFolderTrack(m_pCurrentFolder->get_folder_id(),
                                  m_pCurrentFolder->get_prev_track());
    m_debugMessage = prev;
}

void Mp3Control::play_folder(Folder *currentFolder)
{
    m_pCurrentFolder = currentFolder;
    if (!isFolderValid())
    {
        waitForPlayerReady();
        m_pDfMiniMp3->playAdvertisement(MSG_ERROR_FOLDER);
        m_debugMessage = play_noFolder;
        return; // Cannot play a track if the card is not linked.
    }
    // Start playing folder: first track of current folder.
    waitForPlayerReady();
    m_pDfMiniMp3->playFolderTrack(m_pCurrentFolder->get_folder_id(),
                                  m_pCurrentFolder->get_current_track());
    m_debugMessage = play;
}

void Mp3Control::play_prompt(const VoicePrompt &prompt) const
{
    m_pDfMiniMp3->playAdvertisement(prompt.promptId);
    if(!prompt.allowSkip)
    {
        dontSkipCurrentTrack();
    }
}

void Mp3Control::dontSkipCurrentTrack() const
{
    //Blocker method to make feature wait until voice prompt has played
    //To ensure following voice prompts do not overwrite current
    m_pDfMiniMsgTimeout->start(WAIT_DFMINI_READY);
    while (!is_playing() && !(m_pDfMiniMsgTimeout->isElapsed()))
    {
        waitForPlayerReady(); //wait for track to start (until timeout kicks in)
    }
    m_pDfMiniMsgTimeout->stop();
    m_pDfMiniMsgTimeout->start(TIMEOUT_PROMPT_PLAYED);
    while (is_playing() && !(m_pDfMiniMsgTimeout->isElapsed()))
    {
        waitForPlayerReady(); //wait for track to finish
    }
    m_pDfMiniMsgTimeout->stop();
}

// Waits for DfMiniMp3 player's serial connection to be ready for new commands
void Mp3Control::waitForPlayerReady() const
{
    // command will wait for complete serial
    // communication message to be received from Player
    // assuming that the player is ready to process new input
    m_pDfMiniMp3->loop();
}

uint8_t Mp3Control::get_trackCount_of_folder(uint8_t folderId)
{
    waitForPlayerReady();
    uint16_t trackCnt = m_pDfMiniMp3->getFolderTrackCount(static_cast<uint16_t>(folderId));
    if (trackCnt > 0xFF)
    {
        return 0; // Error: number of tracks too high
    }
    else
    {
        return static_cast<uint8_t>(trackCnt);
    }
}

bool Mp3Control::isFolderValid()
{
    if (m_pCurrentFolder != nullptr)
    {
        if (m_pCurrentFolder->is_valid())
        {
            return true;
        }
    }
    return false;
}