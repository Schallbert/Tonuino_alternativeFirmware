#ifndef NFCCONTROL_INTERFACE_H
#define NFCCONTROL_INTERFACE_H

#include "../Nfc/Nfc_interface.h"

class Folder;

// this object stores nfc tag data
class NfcControl_interface
{
public:
    virtual ~NfcControl_interface(){};

public:
    virtual Nfc_interface::eTagState getTagPresence() = 0;
    virtual bool readFolderFromTag(Folder &targetFolder) = 0;
    virtual bool writeFolderToTag(const Folder &sourceFolder) = 0;
    virtual bool eraseTag() = 0;
#if DEBUGSERIAL
    virtual void print_debug_message() = 0;
#endif

};

#endif //NFCCONTROL_INTERFACE_H