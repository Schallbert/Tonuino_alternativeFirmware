#ifndef NFCCONTROL_INTERFACE_H
#define NFCCONTROL_INTERFACE_H

#include "../Nfc_interface/Nfc_interface.h"

class Folder;

// this object stores nfc tag data
class NfcControl_interface
{
public:
    virtual ~NfcControl_interface(){};

public:
    virtual Nfc_interface::eTagState get_tag_presence() = 0;
    virtual bool read_folder_from_card(Folder &targetFolder) = 0;
    virtual bool write_folder_to_card(const Folder &sourceFolder) = 0;
    virtual bool erase_card() = 0;
#if DEBUGSERIAL
    virtual void print_debug_message() = 0;
#endif

};

#endif //NFCCONTROL_INTERFACE_H