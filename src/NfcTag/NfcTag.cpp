#include "NfcTag.h"

#define MIFARE_UL_PAGE_OFFSET 8
#define MIFARE_UL_BLOCK_SIZE 4

NfcTag::NfcTag(MFRC522_interface* mfrc522)
{
    m_mfrc522 = mfrc522; // make internal variable point to external object
    // Init NFC reader
    SPI.begin();          // Init SPI bus
    m_mfrc522->PCD_Init(); // Init MFRC522
#if DEBUGSERIAL
    m_mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader
#endif
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; ++i) //Init with undefined
    {
        m_eKey.keyByte[i] = 0xFF;
    }
}

bool NfcTag::is_card_present()
{
    return m_mfrc522->PICC_ReadCardSerial();
}
bool NfcTag::is_new_card_present()
{
    return m_mfrc522->PICC_IsNewCardPresent();
}


// WRITE METHODS ----------------------------------------------
bool NfcTag::write_folder_to_card(const Folder &sourceFolder)
{
    m_oFolder = sourceFolder; // Copy source folder to member object
    return write_card();
}

bool NfcTag::write_card()
{
    if (!m_oFolder.is_valid())
    {
#if DEBUGSERIAL
        Serial.print(F("write_card: ERROR: folder object is not correctly initialized"));
#endif
        return false;
    }
    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    MFRC522::PICC_Type piccType = MFRC522::PICC_TYPE_UNKNOWN;
    byte buffer[NFCTAG_MEMORY_TO_OCCUPY] = {};
    // Set buffer according to local folder data
    folder_to_buffer(buffer);
    // Get card online and authenticate
    if (!set_card_online(piccType))
    {
        return false;
    }

#if DEBUGSERIAL
    Serial.print(F("write_card: Writing data to card"));
#endif
    if (!write_buffer_to_card(buffer, status, piccType))
    {
#if DEBUGSERIAL
        Serial.print(F("write_card: ERROR: MIFARE_Write() failed: "));
        Serial.println(to_string((uint8_t)(status)));
#endif
        return false;
    }
    return true;
}

bool NfcTag::write_buffer_to_card(byte *buffer, MFRC522::StatusCode &status, MFRC522::PICC_Type piccType)
{
    if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
        // write 16byte block
        status = (MFRC522::StatusCode)m_mfrc522->MIFARE_Write(m_cui8BlockAddress, buffer, NFCTAG_MEMORY_TO_OCCUPY);
    }
    else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        // Block length limited to 4 bytes!
        byte buffer2[NFCTAG_MEMORY_TO_OCCUPY] = {};
        // write to 4byte-page according to this PICC type's specification
        for (uint8_t i = 0; i < (NFCTAG_MEMORY_TO_OCCUPY/MIFARE_UL_BLOCK_SIZE); ++i)
        {
            memcpy(buffer2, buffer + (i * MIFARE_UL_BLOCK_SIZE), MIFARE_UL_BLOCK_SIZE); // copies 4byte block withing buffer to buffer2
            status = (MFRC522::StatusCode)m_mfrc522->MIFARE_Write(i + MIFARE_UL_PAGE_OFFSET, buffer2, NFCTAG_MEMORY_TO_OCCUPY);
        }
    }
    set_card_offline();
    return (status == MFRC522::STATUS_OK);
}

void NfcTag::folder_to_buffer(byte *buffer)
{
    byte contentsToCopy[NFCTAG_MEMORY_TO_OCCUPY] =
    {
        (byte)(m_cui32MagicCookie >> 24),
        (byte)((m_cui32MagicCookie >> 16) & 0xFF),
        (byte)((m_cui32MagicCookie >> 8) & 0xFF),
        (byte)(m_cui32MagicCookie & 0xFF), // magic cookie to identify our nfc tags
        (byte)m_oFolder.get_folder_id(),   // folder picked by the user
        (byte)m_oFolder.get_play_mode(),   // playback mode picked by the user
        (byte)m_oFolder.get_track_count(), // track count of that m_oFolder
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    memcpy(buffer, contentsToCopy, NFCTAG_MEMORY_TO_OCCUPY);
}

// READ METHODS ----------------------------------------------
bool NfcTag::read_folder_from_card(Folder &targetFolder)
{
    if (read_card())
    {
        targetFolder = m_oFolder; // Copy member object to target folder
        return true;
    }
    return false; //unknown card
}

bool NfcTag::read_card()
{
    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    MFRC522::PICC_Type piccType = MFRC522::PICC_TYPE_UNKNOWN;
    // read buffer MUST be bigger than write buffer due to checksum (that we ignore!)
    byte buffer[NFCTAG_MEMORY_TO_OCCUPY + 2] = {};

    // Get card online and authenticate
    if (!set_card_online(piccType))
    {
        return false;
    }

#if DEBUGSERIAL
    Serial.print(F("read_card: Reading data from card"));
#endif
    // Read actual data to buffer
    if (!read_card_to_buffer(buffer, status, piccType))
    {
#if DEBUGSERIAL
        Serial.print(F("read_card: MIFARE_Read() failed: "));
        Serial.println(to_string((uint8_t)(status)));
#endif
        return false;
    }
    // Convert buffer to folder data
    buffer_to_folder(buffer);
    if (m_cui32Cookie != m_cui32MagicCookie)
    {
        return false; // unknown card.
    }
    return true;
}

bool NfcTag::read_card_to_buffer(byte* buffer, MFRC522::StatusCode &status, MFRC522::PICC_Type piccType)
{
    if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
        status = (MFRC522::StatusCode)m_mfrc522->MIFARE_Read(m_cui8BlockAddress, buffer, NFCTAG_MEMORY_TO_OCCUPY + 2);
    }
    else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        // Block length limited to 4 bytes!
        byte buffer2[NFCTAG_MEMORY_TO_OCCUPY + 2];
        for (uint8_t i = 0; i < (NFCTAG_MEMORY_TO_OCCUPY/MIFARE_UL_BLOCK_SIZE); ++i)
        {
            status = (MFRC522::StatusCode)m_mfrc522->MIFARE_Read(i + MIFARE_UL_PAGE_OFFSET, buffer2, NFCTAG_MEMORY_TO_OCCUPY + 2);
            memcpy(buffer + (i * MIFARE_UL_BLOCK_SIZE), buffer2, MIFARE_UL_BLOCK_SIZE); // copy 4byte block from buffer2 to buffer1
        }
    }
    set_card_offline();
    return (status == MFRC522::STATUS_OK);
}

void NfcTag::buffer_to_folder(byte *buffer)
{
    // Transfer buffer from card read to nfcTag's variables
    m_cui32Cookie = ((uint32_t)buffer[0] << 24) |
                    ((uint32_t)buffer[1] << 16) |
                    ((uint32_t)buffer[2] << 8) |
                    (uint32_t)buffer[3];
    uint8_t folderId = (uint8_t)buffer[4];
    Folder::PlayMode playMode = (Folder::PlayMode)buffer[5];
    uint8_t trackCount = (uint8_t)buffer[6];
    // Set m_oFolder object.
    m_oFolder = Folder(folderId, playMode, trackCount);
}

// ADMIN METHODS ---------------------------------------------------------
bool NfcTag::authenticate_card(MFRC522::PICC_Type piccType)
{
    MFRC522::StatusCode status;
    // Authenticate using key B
    // authentificate with the card and set card specific parameters
    if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (piccType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
#if DEBUGSERIAL
        Serial.println(F("authenticate_card: Authenticating using key A..."));
#endif
        status = m_mfrc522->PCD_Authenticate(
            MFRC522::PICC_CMD_MF_AUTH_KEY_A, m_cui8TrailerBlock, &m_eKey, &(m_mfrc522->get_uid()));
    }
    else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag
// Authenticate using key A
#if DEBUGSERIAL
        Serial.println(F("authenticate_card: Authenticating UL..."));
#endif
        status = m_mfrc522->PCD_NTAG216_AUTH(m_eKey.keyByte, pACK);
    }

    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
        Serial.print(F("authenticate_card: ERROR: PCD_Authenticate() failed: "));
        Serial.println(to_string((uint8_t)(status)));
#endif
        return false;
    }
    return true;
}

bool NfcTag::set_card_online(MFRC522::PICC_Type &piccType)
{
    if (m_mfrc522->PICC_ReadCardSerial() != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
        Serial.println(F("set_card_online: ERROR: Couldn't detect card."));
#endif
        set_card_offline();
        return false;
    }

    piccType = m_mfrc522->PICC_GetType((m_mfrc522->get_uid()).sak);
    if (!authenticate_card(piccType))
    {
        set_card_offline();
        return false;
    }

#if DEBUGSERIAL
    Serial.print(F("set_card_online: Card UID:"));
    Serial.println();
    Serial.print(F("PICC type: "));
    Serial.println(to_string((uint8_t)(piccType)));
#endif
    return true;
}

void NfcTag::set_card_offline()
{
    m_mfrc522->PICC_HaltA();
    m_mfrc522->PCD_StopCrypto1();
}