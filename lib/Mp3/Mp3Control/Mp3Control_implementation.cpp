#include "Mp3Control/Mp3Control_implementation.h"

Mp3Control::Mp3Control(DfMiniMp3_interface *pDfMini,
                       Mp3Play_interface *pPlayer,
                       ErrorHandler_interface *pError) : m_pDfMiniMp3(pDfMini),
                                                         m_pMp3Player(pPlayer),
                                                         m_pErrorHandler(pError)
{

    // Init communication with module and setup
    m_pDfMiniMp3->setVolume(VOLUME_INIT);
}

void Mp3Control::loop() const
{
    m_pMp3Player->autoplay(); // TODO: HOW TO RESET LULLABYE TIMER?
}

void Mp3Control::play() const
{
    m_pDfMiniMp3->start();                          // Only successful if a track is entered.
}

void Mp3Control::pause() const
{
    m_pDfMiniMp3->pause();
}

void Mp3Control::togglePlayPause() const
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

void Mp3Control::nextTrack() const
{
    m_pMp3Player->playNext();
}

void Mp3Control::prevTrack() const
{
    m_pMp3Player->playPrev();
}

void Mp3Control::volumeUp() const
{
    if (m_pDfMiniMp3->getVolume() < VOLUME_MAX)
    {
        m_pDfMiniMp3->increaseVolume();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::volumeUp);
    }
}

void Mp3Control::volumeDown() const
{
    if (m_pDfMiniMp3->getVolume() > VOLUME_MIN)
    {
        m_pDfMiniMp3->decreaseVolume();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::volumeDown);
    }
}