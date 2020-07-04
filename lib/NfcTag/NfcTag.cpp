#include "NfcTag.h"

NfcTag::NfcTag(MFRC522_interface *pMfrc522)
{
    m_pMfrc522 = pMfrc522; // make internal variable point to external object
    m_pMfrc522->initReader();
} 

bool NfcTag::is_card_present()
{
    return m_pMfrc522->isCardPresent();
}
bool NfcTag::is_new_card_present()
{
    return m_pMfrc522->isNewCardPresent();
}

bool NfcTag::write_folder_to_card(const Folder &sourceFolder)
{
    m_oFolder = sourceFolder;                  // Copy source folder to member object
    byte buffer[MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY] = {};
    if (!m_oFolder.is_valid())
    {
#if DEBUGSERIAL
        Serial.print(F("write_card: ERROR: folder object is not correctly initialized"));
#endif
        return false;
    }
    folder_to_buffer(buffer); // Set buffer according to local folder data
    // Get card online and authenticate
    return m_pMfrc522->writeCard(blockAddressToReadWrite, buffer);
}

bool NfcTag::read_folder_from_card(Folder &targetFolder)
{
    byte buffer[MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY] = {};
    if (m_pMfrc522->readCard(blockAddressToReadWrite, buffer))
    {
        buffer_to_folder(buffer);
        targetFolder = m_oFolder; // Copy member object to target folder

        return true;
    }
    return false; //unknown card
}

void NfcTag::folder_to_buffer(byte *buffer)
{
    buffer[0] = (byte)(cui32MagicCookie >> 24);          // 0
    buffer[1] = (byte)((cui32MagicCookie >> 16) & 0xFF);  // 1
    buffer[2] = (byte)((cui32MagicCookie >> 8) & 0xFF);   // 2
    buffer[3] = (byte)(cui32MagicCookie & 0xFF);          // 3: magic cookie to identify our nfc tags
    buffer[4] = (byte)m_oFolder.get_folder_id();            // 4: folder picked by the user
    buffer[5] = (byte)m_oFolder.get_play_mode();            // 5: playback mode picked by the user
    buffer[6] = (byte)m_oFolder.get_track_count();          // 6: track count of that m_oFolder
    for (int i = 7; i< 16; ++i)                             // 7-15: Empty
    {
        buffer[i] = 0x00;
    }
}

void NfcTag::buffer_to_folder(byte *buffer)
{
    // Transfer buffer from card read to nfcTag's variables
    m_ui32CardCookie = ((uint32_t)buffer[0] << 24) |
                       ((uint32_t)buffer[1] << 16) |
                       ((uint32_t)buffer[2] << 8) |
                       (uint32_t)buffer[3];
    uint8_t folderId = (uint8_t)buffer[4];
    Folder::PlayMode playMode = (Folder::PlayMode)buffer[5];
    uint8_t trackCount = (uint8_t)buffer[6];
    // Set m_oFolder object.
    m_oFolder = Folder(folderId, playMode, trackCount);
}

bool NfcTag::checkKnownCard()
{
    if (m_ui32CardCookie != cui32MagicCookie)
    {
        // Card has never been written with Magic Cookie:
        // This card can be read but is unknown!
        //Callback???!!!
        return false;
    }
    return true;
}