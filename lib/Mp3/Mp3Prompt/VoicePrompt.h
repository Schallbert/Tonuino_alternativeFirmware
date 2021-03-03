#ifndef PROMPTS_H
#define PROMPTS_H

#include "Arduino_types.h"

class VoicePrompt
{
public:
    //Tonuino mp3 prompt IDs, MUST BE KEPT IN SYNC WITH FILE NAMES on SD CARD!
    enum ePromptIds
    {
        MSG_ERROR_CARDWRITE = 410,
        MSG_ERROR_CARDREAD = 411,
        MSG_ERROR_FOLDER = 412,
        MSG_ERROR_TOOMANYTRACKS = 413,
        MSG_ERROR_NO_TAG = 414, // TODO: Record!
        MSG_BUTTONLOCK = 426,
        MSG_BUTTONFREE = 427,
        MSG_STARTUP = 500,
        MSG_SHUTDOWN = 501,
        MSG_SELECT_FOLDERID = 300,
        MSG_SELECT_PLAYMODE = 400,
        MSG_HELP = 420, // TODO HOW TO SELECT UserInput variant?
        MSG_TAGCONFSUCCESS = 702,
        MSG_DELETETAG = 700,
        MSG_CONFIRM_DELETION = 701,
        MSG_ABORTED = 802
    };

public:
    VoicePrompt() = default;
    VoicePrompt(uint16_t promptId,
                bool allowSkip) : m_promptId(static_cast<uint16_t>(promptId)),
                                  m_allowSkip(allowSkip){};
    VoicePrompt(ePromptIds promptId,
                bool allowSkip) : m_promptId(static_cast<uint16_t>(promptId)),
                                  m_allowSkip(allowSkip){};
    ~VoicePrompt() = default;
    VoicePrompt(const VoicePrompt &cpyPrompt)
    {
        m_promptId = cpyPrompt.m_promptId;
        m_allowSkip = cpyPrompt.m_allowSkip;
    }

    VoicePrompt &operator=(const VoicePrompt &cpyPrompt)
    {
        m_promptId = cpyPrompt.m_promptId;
        m_allowSkip = cpyPrompt.m_allowSkip;
        return *this;
    }

    bool operator==(VoicePrompt const &prompt) const
    {
        return ((m_promptId == prompt.m_promptId) && (m_allowSkip == prompt.m_allowSkip));
    }

    bool operator!=(VoicePrompt const &prompt) const
    {
        return (!operator==(prompt));
    }

    void reset(uint16_t promptId, bool allowSkip)
    {
        m_promptId = promptId;
        m_allowSkip = allowSkip;
    }

    uint16_t getId() const { return m_promptId; }
    bool getSkip() const { return m_allowSkip; }

private:
    uint16_t m_promptId{0};
    bool m_allowSkip{true};
};

#endif // PROMPTS_H