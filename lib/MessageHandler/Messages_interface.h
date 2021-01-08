#ifndef MESSAGES_INTERFACE_H
#define MESSAGES_INTERFACE_H

// MUST be kept synchronized with s_group of messages.cpp, numbers are the offsets of eMessageContent
namespace Messages_interface
{
    enum eMessageGroup
    {
        SYSTEM = 0,
        MP3CONTROL = 3,
        MP3PLAYBACK = 9,
        MP3PLAYER = 13,
        NFCCONTROL = 18,
        NFCREADER = 22,
        PLAYLIST = 28,
        GROUPCOUNT = 7
    };

    // MUST be kept synchronized with s_content of messages.cpp
    enum eMessageContent
    {
        STARTUP = 0,
        HALT,
        PROMPTTIMEOUT,
        VOLUP,
        VOLDN,
        NEXT,
        PREV,
        PLAY,
        PAUSE,
        ERRORFOLDER,
        FOLDEROK,
        AUTOPLAYPAUSE,
        AUTOPLAYNEXT,
        TRACKFINISHED,
        SDONLINE,
        SDINSERT,
        SDREMOVE,
        ERRORCOM,
        NOTAG,
        ACTIVETAG,
        NEWTAG,
        UNKNOWNTAG,
        READERINIT,
        WRITEOK,
        READOK,
        ERRORWRITE,
        ERRORREAD,
        ERRORTYPE,
        MODELULLABYE,
        MODEALBUM,
        MODERANDOM,
        MODESAVEPROGRESS,
        MODEONETRACKONLY
    };
} // namespace Messages_interface

struct Message
{
    Message(Messages_interface::eMessageGroup group, Messages_interface::eMessageContent contents) : m_group(group), m_contents(contents){};
    Message(Messages_interface::eMessageGroup group, uint8_t offset) : m_group(group), m_contents(static_cast<Messages_interface::eMessageContent>(static_cast<uint8_t>(group) + offset)){};
    ~Message() = default;

    Messages_interface::eMessageGroup m_group{};
    Messages_interface::eMessageContent m_contents{};
};

class Message_String
{
public:
    virtual ~Message_String(){};
    virtual char *getStringFromMessage(const Message &message) = 0;
};

#endif // MESSAGES_INTERFACE_H