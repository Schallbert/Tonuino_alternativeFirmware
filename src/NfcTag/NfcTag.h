#ifndef NFCTAG_H
#define NFCTAG_H

#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Folder.h>
#include <Defines.h>

// this object stores nfc tag data
class NfcTag
{
public:
    NfcTag();

public:
    // Returns true if a tag is present in the reader's vicinity.
    bool is_card_present();
    // Returns true if an unknown (new) tag is present in the reader's vicinity.
    bool is_new_card_present();
    // takes a reference to an existing folder and copies the card's saved data into that object 
    // [folderId, playMode, trackCount]
    bool read_folder_from_card(Folder &targetFolder);
    // writes an existing source folder's data to card (by reference)
    bool write_folder_to_card(const Folder &sourceFolder);

private:
    // READ / WRITE
    // Reads card and packs data to internal folder object
    bool read_card();
    // Checks card type and reads contents to buffer
    bool read_card_to_buffer(byte* buffer, MFRC522::StatusCode& status, MFRC522::PICC_Type piccType);
    // Converts bytestream read from NFC tag to folder/ cookie data
    void buffer_to_folder(byte* buffer);
    // Writes data from internal folder object to card
    bool write_card();
    // Checks card type and writes prepared buffer to card
    bool write_buffer_to_card(byte* buffer, MFRC522::StatusCode& status, MFRC522::PICC_Type piccType);
    // copies folder information to buffer for card
    void folder_to_buffer(byte* buffer);
    // ADMIN
    // Opens serial and authenticates card
    bool set_card_online(MFRC522::PICC_Type &piccType);
    // Closes cryptographic session and shuts down serial
    void set_card_offline();
    // Authenticates card to reader with internal, static key.
    bool authenticate_card(MFRC522::PICC_Type piccType);

public:
private:
    static const uint32_t m_cui32MagicCookie {0x1337b437};   // Magic Id to tag all cards
    MFRC522 m_mfrc522{MFRC522(SS_PIN, RST_PIN)};             // Call constructor
    MFRC522::MIFARE_Key m_eKey {};
    const byte m_cui8BlockAddress {4};
    const byte m_cui8TrailerBlock {7};
    uint32_t m_cui32Cookie {0};
    Folder m_oFolder; //Uninitialized!
};

#endif //NFCTAG_H