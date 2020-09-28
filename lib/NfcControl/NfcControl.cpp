#include "NfcControl.h"

NfcControl::NfcControl(Nfc_interface *pMfrc522,
                             Arduino_interface_com *pUsb) : m_pMfrc522(pMfrc522),
                                                            m_pUsb(pUsb)
{
    m_pMfrc522->initNfc();
    m_pBuffer = new uint8_t[NfcTag_interface::NFCTAG_MEMORY_TO_OCCUPY]();
}

NfcControl::~NfcControl()
{
    delete[] m_pBuffer;
}

Nfc_interface::eTagState NfcControl::get_tag_presence()
{
    // Adds "known tag" information if a new tag has been placed.
    // Otherwise, just wrapper for layer down method.
    auto tagPresence = m_pMfrc522->getTagPresence();
    if (tagPresence == Nfc_interface::NEW_TAG)
    {
        if (is_known_card())
        {
            return Nfc_interface::NEW_KNOWN_TAG;
        }
        else
        {
            return Nfc_interface::NEW_UNKNOWN_TAG;
        }
    }
    return tagPresence;
}

bool NfcControl::write_folder_to_card(const Folder &sourceFolder)
{
    m_oFolder = sourceFolder; // Copy source folder to member object
    if (!m_oFolder.is_valid())
    {
        m_oFolder = Folder(); // re-init to empty object
        return false;
    }
    folder_to_buffer(); // Set buffer according to local folder data
    // Get card online and authenticate
    return m_pMfrc522->writeTag(blockAddressToReadWrite, m_pBuffer);
}

bool NfcControl::erase_card()
{
    for (int i = 0; i < Nfc_interface::NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
    {
        m_pBuffer[i] = 0x00;
    }
    return m_pMfrc522->writeTag(blockAddressToReadWrite, m_pBuffer);
}

bool NfcControl::read_folder_from_card(Folder &targetFolder)
{
    if (m_pMfrc522->readTag(blockAddressToReadWrite, m_pBuffer))
    {
        buffer_to_folder();
        if (m_oFolder.is_initiated())
        {
            targetFolder = m_oFolder; // Copy member object to target folder
            return true;
        }
    }
    return false; //unknown or corrupted card
}

void NfcControl::folder_to_buffer()
{
    m_pBuffer[0] = (byte)(cui32MagicCookie >> 24);                       // 0
    m_pBuffer[1] = (byte)((cui32MagicCookie >> 16) & 0xFF);              // 1
    m_pBuffer[2] = (byte)((cui32MagicCookie >> 8) & 0xFF);               // 2
    m_pBuffer[3] = (byte)(cui32MagicCookie & 0xFF);                      // 3: magic cookie to identify our nfc tags
    m_pBuffer[4] = (byte)m_oFolder.get_folder_id();                      // 4: folder picked by the user
    m_pBuffer[5] = (byte)m_oFolder.get_play_mode();                      // 5: playback mode picked by the user
    m_pBuffer[6] = (byte)m_oFolder.get_track_count();                    // 6: track count of that m_oFolder
    for (int i = 7; i < Nfc_interface::NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
    {
        m_pBuffer[i] = 0x00;
    }
}

void NfcControl::buffer_to_folder()
{
    // Transfer m_pBuffer from card read to nfcTag's variables
    m_ui32CardCookie = ((uint32_t)m_pBuffer[0] << 24) |
                       ((uint32_t)m_pBuffer[1] << 16) |
                       ((uint32_t)m_pBuffer[2] << 8) |
                       (uint32_t)m_pBuffer[3];
    uint8_t folderId = (uint8_t)m_pBuffer[4];
    Folder::ePlayMode playMode = (Folder::ePlayMode)m_pBuffer[5];
    uint8_t trackCount = (uint8_t)m_pBuffer[6];
    // Set m_oFolder object.
    m_oFolder = Folder(folderId, playMode, trackCount);
}

bool NfcControl::is_known_card()
{
    Folder dummy;
    read_folder_from_card(dummy); // gets magic cookie.
    if (m_ui32CardCookie != cui32MagicCookie)
    {
        // Card has never been written with Magic Cookie:
        // This card can be read but is unknown!
        return false;
    }
    return true;
}

const char *NfcControl::stringFromNfcTagNotify(Nfc_interface::eTagState value)
{
#if DEBUGSERIAL
    static const char *NOTIFY_STRING[] = {
        "no tag detected",
        "Tag: active, known",
        "",
        "Tag: new, known",
        "Tag: new, unknown",
        "",
        "Error: request out of Range"};

    return NOTIFY_STRING[value];
#endif
    return "";
}