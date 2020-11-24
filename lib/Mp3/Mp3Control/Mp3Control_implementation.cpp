#include "Mp3Control/Mp3Control_implementation.h"

Mp3Control::Mp3Control(DfMiniMp3_interface *pDfMini,
                       Mp3Play_interface *pPlayer,
                       SimpleTimer *pLullabyeTimer,
                       ErrorHandler_interface *pError) : m_pDfMiniMp3(pDfMini),
                                                         m_pMp3Player(pPlayer),
                                                         m_pLullabyeTimer(pLullabyeTimer),
                                                         m_pErrorHandler(pError)
{

    // Init communication with module and setup
    m_pDfMiniMp3->setVolume(VOLUME_INIT);
}

void Mp3Control::loop()
{
    autoplay();
}

void Mp3Control::volumeUp()
{
    if (m_pDfMiniMp3->getVolume() < VOLUME_MAX)
    {
        m_pDfMiniMp3->increaseVolume();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::volumeUp);
    }
}

void Mp3Control::volumeDown()
{
    if (m_pDfMiniMp3->getVolume() > VOLUME_MIN)
    {
        m_pDfMiniMp3->decreaseVolume();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::volumeDown);
    }
}

void Mp3Control::play()
{
    m_pLullabyeTimer->start(LULLABYE_TIMEOUT_SECS); // start&reset shutdown timer
    m_pDfMiniMp3->start();                          // Only successful if a track is entered.
}

void Mp3Control::pause()
{
    m_pDfMiniMp3->pause();
}

void Mp3Control::togglePlayPause()
{
    if(m_pMp3Player->isPlaying())
    {
        pause();
    }
    else
    {
        play();
    }
}

// Routine to check playmode and select next track
void Mp3Control::autoplay()
{
    // Autoplay implementation
    if (m_pDfMiniMp3->checkTrackFinished())
    {
        Folder currentFolder = m_pMp3Player->getCurrentFolder();
        Folder::ePlayMode mode = currentFolder.get_play_mode();
        if (mode == Folder::ONELARGETRACK)
        {
            m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::autoplayOneLargeTrack);
            m_pDfMiniMp3->stop();
            return;
        }
        else if (mode == Folder::LULLABYE && m_pLullabyeTimer->isElapsed())
        {
            m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::autoplayLullabye);
            m_pDfMiniMp3->stop();
            return;
        }
        else
        {
            m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::autoplayNext);
            nextTrack();
        }
    }
}

void Mp3Control::nextTrack()
{
    Folder currentFolder = m_pMp3Player->getCurrentFolder();
    if (!currentFolder.is_valid())
    {
        m_pErrorHandler->setFolderError();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::next_noFolder);
        return; // Cannot play a track if folder is not fully defined.
    }

    m_pDfMiniMp3->playFolderTrack(currentFolder.get_folder_id(),
                                  currentFolder.get_next_track());
    m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::next);
}

void Mp3Control::prevTrack()
{
    Folder currentFolder = m_pMp3Player->getCurrentFolder();
    if (!currentFolder.is_valid())
    {
        m_pErrorHandler->setFolderError();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::prev_noFolder);
        return; // Cannot play a track if the card is not linked.
    }

    m_pDfMiniMp3->playFolderTrack(currentFolder.get_folder_id(),
                                  currentFolder.get_prev_track());
    m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::prev);
}