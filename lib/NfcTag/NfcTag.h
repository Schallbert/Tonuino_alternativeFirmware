#ifndef NFCTAG_H
#define NFCTAG_H

#include <Arduino_types.h>
#include <MFRC522_interface.h>
#include <Folder.h>


// this object stores nfc tag data
class NfcTag
{
public:
    // Create NfcTag object with dependency-injected NfcReader object
    NfcTag(MFRC522_interface *pMfrc522);
    ~NfcTag();

public:
    // Returns true if a tag is present in the reader's vicinity.
    bool is_card_present();
    // Returns true if a not currently active tag is present in the reader's vicinity.
    bool is_new_card_present();
    // takes a reference to an existing folder and copies the card's saved data into that object
    // [cookie, folderId, playMode, trackCount]
    bool read_folder_from_card(Folder &targetFolder);
    // writes an existing source folder's data to card (by reference)
    // writes cookie to card
    bool write_folder_to_card(const Folder &sourceFolder);
    // Returns true if the current card is known to the system
    // if it has the "magic cookie" equal to system's
    bool is_known_card();
    // Sets tag contents the system writes to to 0
    bool erase_card();

private:
    // Handle Read: converts bytestream from NFC tag to folder/ cookie data
    // Converts 16Byte package
    void buffer_to_folder();
    // Prepare Write: copies folder and cookie information to buffer for card
    // Converts 16Byte package
    void folder_to_buffer();

public:
    static const uint32_t cui32MagicCookie{0x1337b437}; // Magic Id to tag all cards
private:
    uint32_t m_ui32CardCookie{0};                    //Cookie read from card to compare against magic ID
    static const uint8_t blockAddressToReadWrite{4}; // sector 1 block 0 for Mini1k4k, page 4-7 for UltraLight
    MFRC522_interface *m_pMfrc522{nullptr};          // NfcReader object to interact with
    uint8_t *m_pBuffer{nullptr};                     // Buffer to read/write from/to tag reader
    Folder m_oFolder{};                              //Uninitialized!
};

#endif //NFCTAG_H