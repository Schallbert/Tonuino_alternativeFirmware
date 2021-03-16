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

    enum ePlayback
    {
        PROMPT_ALLOWSKIP = 0,
        PROMPT_NOSKIP,
        ANNOUNCEMENT
    };

public:
    VoicePrompt() = default;
    VoicePrompt(uint16_t promptId,
                ePlayback playback) : m_promptId(static_cast<uint16_t>(promptId)),
                                      m_playback(static_cast<uint8_t>(playback)){};
    VoicePrompt(ePromptIds promptId,
                ePlayback playback) : m_promptId(static_cast<uint16_t>(promptId)),
                                      m_playback(static_cast<uint8_t>(playback)){};
    ~VoicePrompt() = default;
    VoicePrompt(const VoicePrompt &cpyPrompt)
    {
        m_promptId = cpyPrompt.m_promptId;
        m_playback = cpyPrompt.m_playback;
    }

    VoicePrompt &operator=(const VoicePrompt &cpyPrompt)
    {
        m_promptId = cpyPrompt.m_promptId;
        m_playback = cpyPrompt.m_playback;
        return *this;
    }

    bool operator==(VoicePrompt const &prompt) const
    {
        return ((m_promptId == prompt.m_promptId) && (m_playback == prompt.m_playback));
    }

    bool operator!=(VoicePrompt const &prompt) const
    {
        return (!operator==(prompt));
    }

    void reset(uint16_t promptId, ePlayback playback)
    {
        m_promptId = promptId;
        m_playback = static_cast<uint8_t>(playback);
    }

    uint16_t getId() const { return m_promptId; }
    bool isAnnouncement() const { return (static_cast<uint8_t>(m_playback) == ANNOUNCEMENT); }
    bool isNoSkip() const { return (static_cast<uint8_t>(m_playback) == PROMPT_NOSKIP); }

private:
    uint16_t m_promptId{0};
    uint8_t m_playback{0};
};

#endif // PROMPTS_H