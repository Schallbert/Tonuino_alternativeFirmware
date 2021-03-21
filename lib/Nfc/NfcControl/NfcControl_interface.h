#ifndef NFCCONTROL_INTERFACE_H
#define NFCCONTROL_INTERFACE_H

#include "MessageHandler_interface.h"
class Folder;

// this object stores nfc tag data
class NfcControl_interface
{
public:
    virtual ~NfcControl_interface(){};

public:
    virtual void init() = 0;
    virtual void sleep() = 0;
    virtual void wakeup() = 0;
    virtual Message::eMessageContent getTagPresence() = 0;
    virtual bool readFolderFromTag(Folder &targetFolder) = 0;
    virtual bool writeFolderToTag(const Folder &sourceFolder) = 0;
    virtual bool eraseTag() = 0;
};

#endif //NFCCONTROL_INTERFACE_H