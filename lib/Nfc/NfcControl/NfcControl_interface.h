#ifndef NFCCONTROL_INTERFACE_H
#define NFCCONTROL_INTERFACE_H

class Folder;

// this object stores nfc tag data
class NfcControl_interface
{
public:
    enum eTagState
    {
        NO_TAG = 0,         // 0
        ACTIVE_KNOWN_TAG,   // 1 full playback
        NEW_REGISTERED_TAG, // 2 read card, get folder, full playback
        NEW_UNKNOWN_TAG,    // 3 play voice menu, link folder to card
        NUMBER_OF_TAG_STATES = 4
    };

public:
    virtual ~NfcControl_interface(){};

public:
    virtual NfcControl_interface::eTagState getTagPresence() = 0;
    virtual bool readFolderFromTag(Folder &targetFolder) = 0;
    virtual bool writeFolderToTag(const Folder &sourceFolder) = 0;
    virtual bool eraseTag() = 0;
};

#endif //NFCCONTROL_INTERFACE_H