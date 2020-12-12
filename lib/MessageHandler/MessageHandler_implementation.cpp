#include "Tonuino_config.h"
#include "MessageHandler_implementation.h"
#include "SimpleTimer/SimpleTimer.h"

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
        m_pDfMiniMp3->playAdvertisement(message.promptId);
        waitForPromptToStart();

        if (!message.allowSkip)
        {
            waitForPromptToFinish();
        }
    }
}

bool MessageHandler::isNewPrompt(const VoicePrompt &message)
{
    bool result{(message.promptId != 0 &&
                 message.promptId != m_lastPrompt.promptId)};
    m_lastPrompt.promptId = message.promptId;
    m_lastPrompt.allowSkip = message.allowSkip;

    return (result);
}

void MessageHandler::waitForPromptToStart() const
{
    m_pDfMiniPromptTimer->start(WAIT_DFMINI_READY);
    while (!m_pDfMiniMp3->isPlaying() && !(m_pDfMiniPromptTimer->isElapsed()))
    {
        m_pDfMiniMp3->loop(); //wait for track to start (until timeout kicks in)
    }
    m_pDfMiniPromptTimer->stop();
}

void MessageHandler::waitForPromptToFinish() const
{
    m_pDfMiniPromptTimer->start(TIMEOUT_PROMPT_PLAYED);
    while (m_pDfMiniMp3->isPlaying() && !(m_pDfMiniPromptTimer->isElapsed()))
    {
        m_pDfMiniMp3->loop(); //wait for track to finish
    }
    m_pDfMiniPromptTimer->stop();
}