#ifndef MESSAGES_INTERFACE_H
#define MESSAGES_INTERFACE_H

#include "Arduino_types.h"

constexpr uint8_t IN_REQUEST_OPTIONS = 8; // number of possible USER input messages, 0x70-0x77

class Message
{
public:
    // 16 groups (First nibble 0xX0), 16 possible messages per group (second nibble 0x0X)
    enum eMessageGroup
    {
        SYSTEM = 0x00,
        MP3CONTROL = 0x10,
        MP3PLAYBACK = 0x20,
        MP3PLAYER = 0x30,
        NFCCONTROL = 0x40,
        NFCREADER = 0x50,
        PLAYLIST = 0x60,
        GROUPCOUNT = 7
    };

    // MUST be kept synchronized with s_content of messages.cpp
    enum eMessageContent
    {
        NONE = 0x00,
        STARTUP = 0x01,
        HALT = 0x02,
        PROMPTTIMEOUT = 0x03,
        VOLUP = 0x10,
        VOLDN = 0x11,
        NEXT = 0x12,
        PREV = 0x13,
        PLAY = 0x14,
        PAUSE = 0x15,
        ERRORFOLDER = 0x20,
        FOLDEROKPLAY = 0x21,
        AUTOPLAYPAUSE = 0x22,
        AUTOPLAYNEXT = 0x23,
        TRACKFINISHED = 0x30,
        SDONLINE = 0x31,
        SDINSERT = 0x32,
        SDREMOVE = 0x33,
        ERRORCOM = 0x34,
        PLAYERONLINE = 0x35,
        NOTAG = 0x40,
        ACTIVETAG = 0x41,
        NEWKNOWNTAG = 0x42,
        UNKNOWNTAG = 0x43,
        TAGREADERINIT = 0x50,
        TAGWRITEOK = 0x51,
        TAGREADOK = 0x52,
        TAGERRORWRITE = 0x53,
        TAGERRORREAD = 0x54,
        TAGERRORTYPE = 0x55,
        TAGONLINE = 0x56,
        MODELULLABYE = 0x60,
        MODEALBUM = 0x61,
        MODERANDOM = 0x62,
        MODESAVEPROGRESS = 0x63,
        MODEONETRACKONLY = 0x64,
        INPUTNONE = 0x70,
        INPUTPLPS = 0x71,
        INPUTPLPSLP = 0x72,
        INPUTNEXT = 0x73,
        INPUTNEXTLP = 0x74,
        INPUTPREV = 0x75,
        INPUTPREVLP = 0x76,
        INPUTLOCK = 0x77,
        INPUTONLINE = 0x78,
        INPUTERROR = 0x79,
        LASTVALIDMESSAGE = 0x7F
    };

public:
    // Constructors convert bitcoded types into base values: Groups 0x00 - 0x0F, Messages 0x00 - 0xFF
    explicit Message(eMessageContent contents) : m_contents(static_cast<uint8_t>(contents)){};
    Message(eMessageGroup group, uint8_t offset) : m_contents(group | offset){};
    ~Message() = default;

    bool operator==(Message const &message) const
    {
        return (message.getContentInt() == m_contents);
    }

    uint8_t getGroupIdInt() const { return (m_contents >> 4); }
    uint8_t getContentInt() const { return m_contents; }
    eMessageContent getContent() const { return static_cast<eMessageContent>(m_contents); }

private:
    uint8_t m_contents{0};
};

class MessageToString_interface
{
public:
    virtual ~MessageToString_interface(){};
    virtual char *getStringFromMessage(const Message::eMessageContent message) = 0;
};

#endif // MESSAGES_INTERFACE_H