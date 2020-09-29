#ifndef NFCCONTROL_H
#define NFCCONTROL_H

#include "Tonuino_config.h"
#include "Nfc_interface.h"
#include "Arduino_interface.h"
#include "Folder.h"

// this object stores nfc tag data
class NfcControl
{
public:
    // Create NfcControl object with dependency-injected NfcReader object
    NfcControl(Nfc_interface *pNfc,
               Arduino_interface_com *pUsb);
    ~NfcControl();

public:
    // Returns tag state of presence to requesting entity.
    Nfc_interface::eTagState get_tag_presence();
    // takes a reference to an existing folder and copies the card's saved data into that object
    // [cookie, folderId, playMode, trackCount]
    bool read_folder_from_card(Folder &targetFolder);
    // writes an existing source folder's data to card (by reference)
    // writes cookie to card
    bool write_folder_to_card(const Folder &sourceFolder);
    // Sets tag contents the system writes to to 0
    bool erase_card();
// Gets notification message from card reader
#if DEBUGSERIAL
    // Prints message from player periphery or player controller to Serial.
    void print_debug_message();
#endif

private:
    // Handle Read: converts bytestream from NFC tag to folder/ cookie data
    // Converts 16Byte package
    void buffer_to_folder();
    // Prepare Write: copies folder and cookie information to buffer for card
    // Converts 16Byte package
    void folder_to_buffer();
    // Returns true if the current card is known to the system
    // if it has the "magic cookie" equal to system's
    bool is_known_card();
    // string interpretation of this class's Tag State (defined in Nfc_interface.h)
    static inline const char *stringFromNfcTagNotify(Nfc_interface::eTagState value);

public:
    static const uint32_t cui32MagicCookie{0x1337b437}; // Magic Id to tag all cards
private:
    Nfc_interface *m_pNfc{nullptr};                  // NfcReader object to interact with
    Arduino_interface_com *m_pUsb{nullptr};          // USB_COM serial debug interface
    uint32_t m_ui32CardCookie{0};                    //Cookie read from card to compare against magic ID
    static const uint8_t blockAddressToReadWrite{4}; // sector 1 block 0 for Mini1k4k, page 4-7 for UltraLight
    uint8_t *m_pBuffer{nullptr};                     // Buffer to read/write from/to tag reader
    Folder m_oFolder{};                              //Uninitialized!
};

#endif //NFCCONTROL_H