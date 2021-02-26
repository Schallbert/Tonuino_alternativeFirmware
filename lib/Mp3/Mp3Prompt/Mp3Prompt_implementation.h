#ifndef MP3PROMPT_IMPLEMENTATION_H
#define MP3PROMPT_IMPLEMENTATION_H

#include "Mp3Prompt_interface.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "SimpleTimer/SimpleTimer.h"

class Mp3Prompt : public Mp3Prompt_interface
{
public:
    Mp3Prompt(DfMiniMp3_interface &rDfMini,
              SimpleTimer &rDfMiniPromptTimer) : m_rDfMiniMp3(rDfMini),
                                                 m_rDfMiniPromptTimer(rDfMiniPromptTimer){};
    ~Mp3Prompt() = default;
    Mp3Prompt(const Mp3Prompt &cpy) = delete;

    void playPrompt(const VoicePrompt &prompt) override;

private:
    bool isNewPrompt(const VoicePrompt &prompt);
    void waitForPromptToStart();
    void waitForPromptToFinish();

private:
    DfMiniMp3_interface &m_rDfMiniMp3;
    SimpleTimer &m_rDfMiniPromptTimer;

    VoicePrompt m_lastPrompt;
};

#endif // MP3PROMPT_IMPLEMENTATION_H