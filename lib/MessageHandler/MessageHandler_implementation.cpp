#include "Arduino_DIcontainer_interface.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"

#include "Tonuino_config.h"
#include "SimpleTimer/SimpleTimer.h"

#include "MessageHandler_implementation.h"

const char* messageTimeout = "Prompt timeout";

MessageHandler::MessageHandler(Arduino_interface_com *pSerial,

                               DfMiniMp3_interface *pDfMini,
                               SimpleTimer *pDfMiniPromptTimer) : m_pSerial(pSerial),
                                                                  m_pDfMiniMp3(pDfMini),
                                                                  m_pDfMiniPromptTimer(pDfMiniPromptTimer){};

void MessageHandler::printMessage(const char *message)
{
    if (message != nullptr)
    {
        if (*message != 0)
        {
            m_pSerial->com_println(message);
        }
    }
}

void MessageHandler::promptMessage(const VoicePrompt &message)
{
    m_pDfMiniMp3->stop();

    if (isNewPrompt(message))
    {
        m_pDfMiniMp3->playMp3FolderTrack(message.promptId);
        waitForPromptToStart();

        if (!message.allowSkip)
        {
            waitForPromptToFinish();
        }
    }
    m_pDfMiniMp3->loop();
}

bool MessageHandler::isNewPrompt(const VoicePrompt &message)
{
    bool result{(message.promptId != 0 &&
                 message.promptId != m_lastPrompt.promptId)};
    m_lastPrompt.promptId = message.promptId;
    m_lastPrompt.allowSkip = message.allowSkip;

    return (result);
}

void MessageHandler::waitForPromptToStart()
{
    m_pDfMiniPromptTimer->start(WAIT_DFMINI_READY);
    while (!m_pDfMiniMp3->isPlaying())
        {
            m_pDfMiniMp3->loop(); //wait for track to start (until timeout kicks in)
            if (m_pDfMiniPromptTimer->isElapsed())
            {
                printMessage(messageTimeout);
                break;
            }
        }
    m_pDfMiniPromptTimer->stop();
}

void MessageHandler::waitForPromptToFinish()
{
    m_pDfMiniPromptTimer->start(TIMEOUT_PROMPT_PLAYED);
    while (m_pDfMiniMp3->isPlaying())
    {
        m_pDfMiniMp3->loop(); //wait for track to finish
        if (m_pDfMiniPromptTimer->isElapsed())
        {
            printMessage(messageTimeout);
            break;
        }
    }
    m_pDfMiniPromptTimer->stop();
}