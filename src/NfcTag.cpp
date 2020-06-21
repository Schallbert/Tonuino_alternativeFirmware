#include "NfcTag.h"

NfcTag::NfcTag()
{
    // NFC Leser initialisieren
    //mfrc522 = MFRC522(SS_PIN, RST_PIN); // Call constructor
    SPI.begin();                        // Init SPI bus
    mfrc522.PCD_Init();                 // Init MFRC522
#if DEBUGSERIAL
    mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader
#endif
    for (byte i = 0; i < 6; i++) //Init with undefined
    {
        key.keyByte[i] = 0xFF;
    }
}

bool NfcTag::get_folder(Folder &targetFolder)
{
    if (read_card())
    {
        // copy folder to target folder (refactor: copy constructor...?)
        targetFolder = folder;
        return true;
    }
    return false; //unknown card
}

bool NfcTag::set_folder(Folder targetFolder)
{
    folder = targetFolder;
    if (write_card())
    {
        return true;
    }
    return false;
}
bool NfcTag::is_card_present()
{
    return mfrc522.PICC_ReadCardSerial();
}
bool NfcTag::is_new_card_present()
{
    return mfrc522.PICC_IsNewCardPresent();
}

bool NfcTag::write_card()
{
    MFRC522::StatusCode status;
    MFRC522::PICC_Type mifareType;
    byte buffer[NFCTAG_MEMORY_TO_OCCUPY] =
        {
            (byte)(cardCookie >> 24),
            (byte)((cardCookie >> 16) & 0xFF),
            (byte)((cardCookie >> 8) & 0xFF),
            (byte)(cardCookie & 0xFF),    // 0x1337b347 magic cookie to identify our nfc tags
            0x02,                         // version 1
            (byte)folder.get_folder_id(), // the folder picked by the user
            (byte)folder.get_play_mode(), // the playback mode picked by the user
            (byte)folder.get_track_count(),
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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
        status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, buffer, NFCTAG_MEMORY_TO_OCCUPY);
    }
    else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        // write 4byte block x 4
        byte buffer2[NFCTAG_MEMORY_TO_OCCUPY];
        for (uint8_t i = 0; i < 4; ++i)
        {
            memset(buffer2, 0, NFCTAG_MEMORY_TO_OCCUPY);
            memcpy(buffer2, buffer + (i * 4), 4);
            status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(i + 8, buffer2, 16);
        }
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
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
        status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
        if (status != MFRC522::STATUS_OK)
        {
#if DEBUGSERIAL
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
#endif
            mfrc522.PICC_HaltA();
            mfrc522.PCD_StopCrypto1();
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
            status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(i + 8, buffer2, &size2);
            if (status != MFRC522::STATUS_OK)
            {
#if DEBUGSERIAL
                Serial.print(F("MIFARE_Read_4ByteBlock failed: "));
                Serial.println(mfrc522.GetStatusCodeName(status));
#endif
                mfrc522.PICC_HaltA();
                mfrc522.PCD_StopCrypto1();
                return false;
            }
        }
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
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
    uint8_t folderId = (uint8_t)buffer[5];
    Folder::PlayMode playMode = (Folder::PlayMode)buffer[6];
    uint8_t trackCount = (uint8_t)buffer[7];
    folder = Folder(folderId, playMode, trackCount);
    // Check if card and folder could be correctly setup
    if (cookie != cardCookie)
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
        status = mfrc522.PCD_Authenticate(
            MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    }
    else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL)
    {
        byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag
// Authenticate using key A
#if DEBUGSERIAL
        Serial.println(F("Authenticating UL..."));
#endif
        status = mfrc522.PCD_NTAG216_AUTH(key.keyByte, pACK);
    }

    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
#endif
        return false;
    }
    return true;
}

bool NfcTag::set_card_online(MFRC522::PICC_Type &piccType)
{
    if (!mfrc522.PICC_ReadCardSerial())
    {
#if DEBUGSERIAL
        Serial.println(F("Couldn't detect card. Error."));
#endif
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }

    piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (!authenticate_card(piccType))
    {
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        return false;
    }

#if DEBUGSERIAL
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
#endif
    return true;
}