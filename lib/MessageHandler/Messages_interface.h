#ifndef MESSAGES_INTERFACE_H
#define MESSAGES_INTERFACE_H

#include "Arduino_types.h"

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
        FOLDEROK = 0x21,
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
        READERINIT = 0x50,
        WRITEOK = 0x51,
        READOK = 0x52,
        ERRORWRITE = 0x53,
        ERRORREAD = 0x54,
        ERRORTYPE = 0x55,
        ONLINE = 0x56,
        MODELULLABYE = 0x60,
        MODEALBUM = 0x61,
        MODERANDOM = 0x62,
        MODESAVEPROGRESS = 0x63,
        MODEONETRACKONLY = 0x64,
        LASTVALIDMESSAGE = 0x6F
    };

public:
    // Constructors convert bitcoded types into base values: Groups 0x00 - 0x0F, Messages 0x00 - 0xFF
    explicit Message(eMessageContent contents) : m_contents(static_cast<uint8_t>(contents)){};
    Message(eMessageGroup group, uint8_t offset) : m_contents(group | offset){};
    ~Message() = default;
    
    bool operator==(Message const &message) const
    {
        return (message.getContents() == m_contents);
    }

    void setContents(eMessageContent contents)
    {
        m_contents = static_cast<uint8_t>(contents);
    }

    void reset()
    {
        m_contents = 0;
    }

    uint8_t getGroupId() const { return (m_contents >> 4); }
    uint8_t getContents() const { return m_contents; }

private:
    uint8_t m_contents{0};
};

class MessageToString_interface
{
public:
    virtual ~MessageToString_interface(){};
    virtual char *getStringFromMessage(const Message &message) = 0;
};

#endif // MESSAGES_INTERFACE_H