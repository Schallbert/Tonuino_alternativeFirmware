#include "Tonuino_config.h"
#include "Mp3Prompt_implementation.h"

void Mp3Prompt::playPrompt(const VoicePrompt &prompt)
{
    if (!isNewPrompt(prompt))
    {
        return;
    }

    if (prompt.isResumePlayback())
    {
        m_rDfMiniMp3.playPrompt(prompt.getId());
        waitForPromptToStart();
        waitForPromptToFinish();
    }
    else
    {
        // play dummy silence track to make advertisement playable
        const uint8_t ZEROFOLDER{0};
        const uint8_t SILENCETRACK{1};
        m_rDfMiniMp3.playFolderTrack(ZEROFOLDER, SILENCETRACK);

        m_rDfMiniMp3.playPrompt(prompt.getId());
        waitForPromptToStart();

        if (prompt.isNoSkip())
        {
            waitForPromptToFinish();
        }
        // stop playback of silenceTrack
        m_rDfMiniMp3.stop();
    }

    m_rDfMiniMp3.printStatus();
}

void Mp3Prompt::stopPrompt()
{
    return m_rDfMiniMp3.stopPrompt();
}

bool Mp3Prompt::isNewPrompt(const VoicePrompt &prompt)
{
    bool result{false};
    if (prompt.getId() == 0)
    {
        return result;
    }

    if (m_lastPrompt != prompt)
    {
        m_lastPrompt = prompt;
        result = true;
    }
    return result;
}

void Mp3Prompt::waitForPromptToStart()
{
    m_rDfMiniPromptTimer.start(WAIT_DFMINI_READY);
    while (!m_rDfMiniMp3.isPlaying() && !m_rDfMiniPromptTimer.isElapsed())
    {
        m_rDfMiniMp3.loop(); //wait for track to start (until timeout kicks in)
    }
    m_rDfMiniPromptTimer.stop();
}

void Mp3Prompt::waitForPromptToFinish()
{
    m_rDfMiniPromptTimer.start(TIMEOUT_PROMPT_PLAYED);
    while (m_rDfMiniMp3.isPlaying() && !m_rDfMiniPromptTimer.isElapsed())
    {
        m_rDfMiniMp3.loop(); //wait for track to finish
    }
    m_rDfMiniPromptTimer.stop();
}