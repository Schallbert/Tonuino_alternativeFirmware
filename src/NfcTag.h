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
    bool is_card_present();
    bool is_new_card_present();
    bool get_folder(Folder &targetFolder);
    bool set_folder(Folder targetFolder);

private:
    bool read_card();
    bool write_card();
    bool set_card_online(MFRC522::PICC_Type &piccType);
    void dump_byte_array(byte *buffer, byte bufferSize);
    bool authenticate_card(MFRC522::PICC_Type piccType);

public:
private:
    static const uint32_t m_cui32CardCookie {0x1337b437};        // Magic Id to tag all cards
    MFRC522 m_mfrc522{MFRC522(SS_PIN, RST_PIN)};            // Call constructor
    MFRC522::MIFARE_Key m_eKey;
    byte sector {1};
    byte blockAddr {4};
    byte trailerBlock {7};
    uint32_t cookie {0};
    uint8_t version {0};
    Folder folder; //Uninitialized!
};

#endif