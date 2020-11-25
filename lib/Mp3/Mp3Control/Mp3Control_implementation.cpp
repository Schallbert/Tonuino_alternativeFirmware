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

void Mp3Control::setUserInput(UserInput::eUserRequest input)
{
    m_userInput = input;
}

void Mp3Control::handleUserInput()
{
    // initialize array of function pointers to address state-event transitions
    typedef Mp3Control PC;
    static const dispatcher dispatchTable[UserInput::NUMBER_OF_REQUESTS] =
        {
            //NOAC,     PL_PS,     PP_LP,     NEXT_,     PREV_,     INC_V,     DEC_V,
            &PC::none, &PC::plPs, &PC::help, &PC::next, &PC::prev, &PC::incV, &PC::decV // NO_TAG / ACTIVE_KNOWN_TAG
        };
    dispatcher dispatchExecutor = dispatchTable[m_userInput];
    (this->*dispatchExecutor)();
}

void Mp3Control::loop()
{
    m_pMp3Player->autoplay();
    handleUserInput();
}

void Mp3Control::play()
{
    m_pDfMiniMp3->start(); // Only successful if a track is entered.
}

void Mp3Control::pause()
{
    m_pDfMiniMp3->pause();
}

void Mp3Control::plPs()
{
    if (m_pMp3Player->isPlaying())
    {
        pause();
    }
    else
    {
        play();
    }
}

void Mp3Control::next()
{
    m_pMp3Player->playNext();
}

void Mp3Control::prev()
{
    m_pMp3Player->playPrev();
}

void Mp3Control::incV()
{
    if (m_pDfMiniMp3->getVolume() < VOLUME_MAX)
    {
        m_pDfMiniMp3->increaseVolume();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::volumeUp);
    }
}

void Mp3Control::decV()
{
    if (m_pDfMiniMp3->getVolume() > VOLUME_MIN)
    {
        m_pDfMiniMp3->decreaseVolume();
        m_pErrorHandler->setMp3ControlNotify(Mp3ControlNotify::volumeDown);
    }
}

void Mp3Control::help()
{
    VoicePrompt helpMessage;
    helpMessage.promptId = MSG_HELP;
    helpMessage.allowSkip = true;
    m_pMp3Player->playPrompt(helpMessage);
}