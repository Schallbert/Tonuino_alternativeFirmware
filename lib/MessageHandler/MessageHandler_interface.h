#ifndef MESSAGEHANDLER_INTERFACE_H
#define MESSAGEHANDLER_INTERFACE_H

#include "../Mp3/Mp3Play/Mp3Play_interface.h"

// MUST be kept synchronized with s_group of messages.cpp, numbers are the offsets of eMessageContent
enum eMessageGroup
{
    system = 0,
    mp3Control = 3,
    mp3Playback = 9,
    mp3Player = 13,
    nfcControl = 18,
    nfcReader = 22,
    playlist = 28,
    groupCount = 7
};

// MUST be kept synchronized with s_content of messages.cpp
enum eMessageContent
{
    up = 0,
    halt,
    promptTimeout,
    volUp,
    volDn,
    next,
    prev,
    play,
    pause,
    folderErr,
    folderOk,
    autoPause,
    autoNext,
    trackFin,
    sdOnline,
    sdInsert,
    sdRemove,
    comErr,
    noTag,
    actTag,
    newTag,
    unknTag,
    rdrInit,
    writeOK,
    readOk,
    writeErr,
    readErr,
    typeErr,
    modeLull,
    modeAlbum,
    modeRand,
    modeSave,
    modeOne
};

struct Message
{
    Message(eMessageGroup group, eMessageContent contents) : m_group(group), m_contents(contents){};
    Message(eMessageGroup group, uint8_t offset) : m_group(group), m_contents(static_cast<eMessageContent>(static_cast<uint8_t>(group) + offset)){};
    ~Message() = default;

    eMessageGroup m_group{};
    eMessageContent m_contents{};
};

struct VoicePrompt
{
    uint16_t promptId{0};
    bool allowSkip{true};
};

class MessageHander_interface
{
public:
    virtual ~MessageHander_interface(){};

    virtual void printMessage(const Message &message) = 0;
    virtual void promptMessage(const VoicePrompt &message) = 0;
};

#endif // MESSAGEHANDLER_INTERFACE_H
