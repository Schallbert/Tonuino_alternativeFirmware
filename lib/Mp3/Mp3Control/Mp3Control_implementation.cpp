#include "Mp3Control/Mp3Control_implementation.h"

#include "Tonuino_config.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"
#include "../Folder/Folder.h"

Mp3Control::Mp3Control(DfMiniMp3_interface &rDfMini,
                       Mp3Play_interface &rPlayer,
                       NfcControl_interface &rNfcControl,
                       MessageHander_interface &rMsgHandler) : m_rDfMiniMp3(rDfMini),
                                                               m_rMp3Player(rPlayer),
                                                               m_rNfcControl(rNfcControl),
                                                               m_rMessageHandler(rMsgHandler)
{

    // Init communication with module and setup
    m_rDfMiniMp3.setVolume(VOLUME_INIT);
    if(m_rDfMiniMp3.getVolume() == VOLUME_INIT)
    {
        m_rMessageHandler.printMessage("DFMini COM established. Volume set.");
    }
}

void Mp3Control::setUserInput(UserInput_interface::eUserRequest input)
{
    m_userInput = input;
}

void Mp3Control::setTagState(NfcControl_interface::eTagState input)
{
    m_tagState = input;
}

void Mp3Control::setBlocked(bool isBlocked)
{
    m_blocked = isBlocked;
}

void Mp3Control::loop()
{
    if(m_blocked)
    {
        return;
    }

    handleCardInput();
    handleUserInput();
    m_rMp3Player.autoplay();
}

void Mp3Control::handleCardInput()
{
    if (m_tagState == NfcControl_interface::NEW_REGISTERED_TAG)
    {
        Folder readFolder;
        if (m_rNfcControl.readFolderFromTag(readFolder))
        {
            m_rMp3Player.playFolder(readFolder);
        }
    }
}

void Mp3Control::handleUserInput()
{
    // initialize array of function pointers to address state-event transitions
    typedef Mp3Control PC;
    static const dispatcher dispatchTable[UserInput_interface::NUMBER_OF_REQUESTS] =
        {
            //NOAC,     PL_PS,     PP_LP,     NEXT_,     PREV_,     INC_V,     DEC_V,
            &PC::none, &PC::plPs, &PC::help, &PC::next, &PC::prev, &PC::incV, &PC::decV // NO_TAG / ACTIVE_KNOWN_TAG
        };
    dispatcher dispatchExecutor = dispatchTable[m_userInput];
    (this->*dispatchExecutor)();
}

void Mp3Control::plPs()
{
    if (m_rDfMiniMp3.isPlaying())
    {
        pause();
    }
    else
    {
        play();
    }
}

void Mp3Control::play()
{
    m_rDfMiniMp3.start(); // Only successful if a track is entered.
    m_rMessageHandler.printMessage(Mp3ControlNotify::toString(Mp3ControlNotify::play));
}

void Mp3Control::pause()
{
    m_rDfMiniMp3.pause();
    m_rMessageHandler.printMessage(Mp3ControlNotify::toString(Mp3ControlNotify::pause));
}

void Mp3Control::next()
{
    m_rMp3Player.playNext();
    m_rMessageHandler.printMessage(Mp3ControlNotify::toString(Mp3ControlNotify::next));
}

void Mp3Control::prev()
{
    m_rMp3Player.playPrev();
    m_rMessageHandler.printMessage(Mp3ControlNotify::toString(Mp3ControlNotify::prev));
}

void Mp3Control::incV()
{
    if (m_rDfMiniMp3.getVolume() < VOLUME_MAX)
    {
        m_rDfMiniMp3.increaseVolume();
        m_rMessageHandler.printMessage(Mp3ControlNotify::toString(Mp3ControlNotify::volumeUp));
    }
}

void Mp3Control::decV()
{
    if (m_rDfMiniMp3.getVolume() > VOLUME_MIN)
    {
        m_rDfMiniMp3.decreaseVolume();
        m_rMessageHandler.printMessage(Mp3ControlNotify::toString(Mp3ControlNotify::volumeDown));
    }
}

void Mp3Control::help()
{
    VoicePrompt helpMessage;
    helpMessage.promptId = MSG_HELP;
    helpMessage.allowSkip = true;
    m_rMessageHandler.promptMessage(helpMessage);
}