#include "NfcTag.h"

NfcTag::NfcTag() // TODO: THIS ONE NEEDS TO GET RANDOMSEED AND EEPROM DEPENDENCIES INJECTED!
{
    // Init NFC reader
    SPI.begin();          // Init SPI bus
    m_mfrc522.PCD_Init(); // Init MFRC522
#if DEBUGSERIAL
    m_mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader
#endif
    for (byte i = 0; i < 6; ++i) //Init with undefined
    {
        m_eKey.keyByte[i] = 0xFF;
    }
}

bool NfcTag::get_folder(Folder &targetFolder)
{
    if (read_card())
    {
        targetFolder = folder; // Copy member object to target folder
        return true;
    }
    return false; //unknown card
}

bool NfcTag::set_folder(Folder &sourceFolder)
{
    if (targetFolder.is_valid())
    {
        folder = sourceFolder; // Copy source folder to member object
        if (write_card())
        {
            return true;
        }
    }
    return false;
}
bool NfcTag::is_card_present()
{
    return m_mfrc522.PICC_ReadCardSerial();
}
bool NfcTag::is_new_card_present()
{
    return m_mfrc522.PICC_IsNewCardPresent();
}

bool NfcTag::write_card()
{
    if (!folder.is_valid())
    {
#if DEBUGSERIAL
        Serial.print(F("folder object is not correctly initialized and cannot be saved to card"));
#endif
        return false;
    }
    MFRC522::StatusCode status;
    MFRC522::PICC_Type mifareType;
    byte buffer[NFCTAG_MEMORY_TO_OCCUPY] =
        {
            (byte)(m_cui32CardCookie >> 24),
            (byte)((m_cui32CardCookie >> 16) & 0xFF),
            (byte)((m_cui32CardCookie >> 8) & 0xFF),
            (byte)(m_cui32CardCookie & 0xFF), // 0x1337b347 magic cookie to identify our nfc tags
            (byte)folder.get_folder_id(),     // folder picked by the user
            (byte)folder.get_play_mode(),     // playback mode picked by the user
            (byte)folder.get_track_count(),   // track count of that folder //TODO: NEW CONTENTS!
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // Get card online and authenticate
    if (!set_card_online(mifareType))
    {
        return false;
    }

// Write data to block
#if DEBUGSERIAL
    Serial.print(F("Writing data into block "));
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(buffer, NFCTAG_MEMORY_TO_OCCUPY);
#endif

    if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (mifareType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (mifareType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
        // write 16byte block
        status = (MFRC522::StatusCode)m_mfrc522.MIFARE_Write(blockAddr, buffer, NFCTAG_MEMORY_TO_OCCUPY);
    }
    else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        // write 4byte block x 4
        byte buffer2[NFCTAG_MEMORY_TO_OCCUPY];
        for (uint8_t i = 0; i < 4; ++i)
        {
            memset(buffer2, 0, NFCTAG_MEMORY_TO_OCCUPY);
            memcpy(buffer2, buffer + (i * 4), 4);
            status = (MFRC522::StatusCode)m_mfrc522.MIFARE_Write(i + 8, buffer2, 16);
        }
    }
    m_mfrc522.PICC_HaltA();
    m_mfrc522.PCD_StopCrypto1();
    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(m_mfrc522.GetStatusCodeName(status));
#endif
        return false;
    }
    return true;
}

bool NfcTag::read_card()
{
    MFRC522::StatusCode status;
    MFRC522::PICC_Type mifareType;
    byte buffer[NFCTAG_MEMORY_TO_OCCUPY];
    byte size = sizeof(buffer);

    // Get card online and authenticate
    if (!set_card_online(mifareType))
    {
        return false;
    }

    // GET DATA FROM CARD    -------------------------------------------------------
    // Read data from the block
    if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (mifareType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (mifareType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
#if DEBUGSERIAL
        Serial.print(F("Reading data from block "));
        Serial.print(blockAddr);
        Serial.println(F(" ..."));
#endif
        status = (MFRC522::StatusCode)m_mfrc522.MIFARE_Read(blockAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK)
        {
#if DEBUGSERIAL
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(m_mfrc522.GetStatusCodeName(status));
#endif
            m_mfrc522.PICC_HaltA();
            m_mfrc522.PCD_StopCrypto1();
            return false;
        }
    }
    else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        byte buffer2[NFCTAG_MEMORY_TO_OCCUPY];
        byte size2 = sizeof(buffer2);

        for (uint8_t i = 0; i < 4; ++i)
        {
            memcpy(buffer + (i * 4), buffer2, 4);
            status = (MFRC522::StatusCode)m_mfrc522.MIFARE_Read(i + 8, buffer2, &size2);
            if (status != MFRC522::STATUS_OK)
            {
#if DEBUGSERIAL
                Serial.print(F("MIFARE_Read_4ByteBlock failed: "));
                Serial.println(m_mfrc522.GetStatusCodeName(status));
#endif
                m_mfrc522.PICC_HaltA();
                m_mfrc522.PCD_StopCrypto1();
                return false;
            }
        }
    }
    m_mfrc522.PICC_HaltA();
    m_mfrc522.PCD_StopCrypto1();
#if DEBUGSERIAL
    Serial.print(F("Data on Card "));
    Serial.println(F(":"));
    dump_byte_array(buffer, NFCTAG_MEMORY_TO_OCCUPY);
    Serial.println();
    Serial.println();
#endif
    // END GET DATA FROM CARD    -------------------------------------------------------
    // Transfer buffer from card read to nfcTag's variables
    cookie = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) | ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];
    uint8_t folderId = (uint8_t)buffer[4];
    Folder::PlayMode playMode = (Folder::PlayMode)buffer[5];
    uint8_t trackCount = (uint8_t)buffer[6];
    folder = Folder(folderId, playMode, trackCount);
    // Check if card and folder could be correctly setup
    if (cookie != m_cui32CardCookie)
    {
        return false; // unknown card.
    }
    if (!folder.is_valid())
    {
        // Error: Folder not set up correctly
        // E.g. because user aborted the setup process
        return false;
    }
    return true;
}

bool NfcTag::authenticate_card(MFRC522::PICC_Type mifareType)
{
    MFRC522::StatusCode status;
    // Authenticate using key B
    // authentificate with the card and set card specific parameters
    if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
        (mifareType == MFRC522::PICC_TYPE_MIFARE_1K) ||
        (mifareType == MFRC522::PICC_TYPE_MIFARE_4K))
    {
#if DEBUGSERIAL
        Serial.println(F("Authenticating using key A..."));
#endif
        status = m_mfrc522.PCD_Authenticate(
            MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &m_eKey, &(m_mfrc522.uid));
    }
    else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag
// Authenticate using key A
#if DEBUGSERIAL
        Serial.println(F("Authenticating UL..."));
#endif
        status = m_mfrc522.PCD_NTAG216_AUTH(m_eKey.keyByte, pACK);
    }

    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(m_mfrc522.GetStatusCodeName(status));
#endif
        return false;
    }
    return true;
}

bool NfcTag::set_card_online(MFRC522::PICC_Type &piccType)
{
    if (!m_mfrc522.PICC_ReadCardSerial())
    {
#if DEBUGSERIAL
        Serial.println(F("Couldn't detect card. Error."));
#endif
        m_mfrc522.PICC_HaltA();
        m_mfrc522.PCD_StopCrypto1();
        return false;
    }

    piccType = m_mfrc522.PICC_GetType(m_mfrc522.uid.sak);
    if (!authenticate_card(piccType))
    {
        m_mfrc522.PICC_HaltA();
        m_mfrc522.PCD_StopCrypto1();
        return false;
    }

#if DEBUGSERIAL
    Serial.print(F("Card UID:"));
    dump_byte_array(m_mfrc522.uid.uidByte, m_mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    Serial.println(m_mfrc522.PICC_GetTypeName(piccType));
#endif
    return true;
}