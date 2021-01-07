#ifndef MESSAGEHANDLER_INTERFACE_H
#define MESSAGEHANDLER_INTERFACE_H

#include "../Mp3/Mp3Play/Mp3Play_interface.h"

// MUST be kept synchronized with s_group of messages.cpp
enum eMessageGroup
{
    system = 0,
    mp3Control,
    mp3Playback,
    mp3Player,
    nfcControl,
    nfcReader,
    playlist,
    groupCount
};

// MUST be kept synchronized with s_content of messages.cpp
enum eMessageContent
{
    up = 0,
    halt,
    sleep,
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
