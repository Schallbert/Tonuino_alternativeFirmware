#include "PlayError.h"

void PlayError::promptHelpRequested()
{
    VoicePrompt prompt;
    prompt.promptId = MSG_HELP;
    prompt.allowSkip = true;

    m_pMp3Control->play_prompt(prompt);
}

void PlayError::promptTagReadError()
{
    VoicePrompt prompt;
    prompt.promptId = MSG_ERROR_CARDREAD;
    prompt.allowSkip = false;

    m_pMp3Control->play_prompt(prompt);
}

void PlayError::promptFolderError()
{
    VoicePrompt prompt;
    prompt.promptId = MSG_ERROR_FOLDER;
    prompt.allowSkip = false;

    m_pMp3Control->play_prompt(prompt);
}
