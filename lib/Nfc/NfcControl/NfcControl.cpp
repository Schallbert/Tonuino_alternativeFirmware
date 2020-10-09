#include "NfcControl.h"

NfcControl::NfcControl(Nfc_interface *pNfc,
                       Arduino_interface_com *pUsb) : m_pNfc(pNfc),
                                                      m_pSerial(pUsb)
{
    m_pNfc->initNfc();
    m_pBuffer = new uint8_t[NFCTAG_MEMORY_TO_OCCUPY]();
}

NfcControl::~NfcControl()
{
    delete[] m_pBuffer;
}

#if DEBUGSERIAL
void NfcControl::print_debug_message()
{
    m_pSerial->com_println("NFC CONTROL DEBUG:");
    m_pSerial->com_println(stringFromNfcTagNotify(get_tag_presence()));
    m_pSerial->com_println("NFC DEBUG: MFRC522");
    m_pSerial->com_println(m_pNfc->getNfcNotification());
};
#endif

Nfc_interface::eTagState NfcControl::get_tag_presence()
{
    // Adds "known tag" information if a new tag has been placed.
    // Otherwise, just wrapper for layer down method.
    auto tagPresence = m_pNfc->getTagPresence();
    if (tagPresence == Nfc_interface::NEW_UNKNOWN_TAG)
    {
        if (is_known_card())
        {
            tagPresence = Nfc_interface::NEW_KNOWN_TAG;
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
    return m_pNfc->writeTag(blockAddressToReadWrite, m_pBuffer);
}

bool NfcControl::erase_card()
{
    for (int i = 0; i < NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
    {
        m_pBuffer[i] = 0x00;
    }
    return m_pNfc->writeTag(blockAddressToReadWrite, m_pBuffer);
}

bool NfcControl::read_folder_from_card(Folder &targetFolder)
{
    bool status{false}; //unknown or corrupted card
    if (m_pNfc->readTag(blockAddressToReadWrite, m_pBuffer))
    {
        buffer_to_folder();
        if (m_oFolder.is_initiated())
        {
            targetFolder = m_oFolder; // Copy member object to target folder
            status = true;
        }
    }
    return status; 
}

void NfcControl::folder_to_buffer()
{
    m_pBuffer[0] = (byte)(cui32MagicCookie >> 24);          // 0
    m_pBuffer[1] = (byte)((cui32MagicCookie >> 16) & 0xFF); // 1
    m_pBuffer[2] = (byte)((cui32MagicCookie >> 8) & 0xFF);  // 2
    m_pBuffer[3] = (byte)(cui32MagicCookie & 0xFF);         // 3: magic cookie to identify our nfc tags
    m_pBuffer[4] = (byte)m_oFolder.get_folder_id();         // 4: folder picked by the user
    m_pBuffer[5] = (byte)m_oFolder.get_play_mode();         // 5: playback mode picked by the user
    m_pBuffer[6] = (byte)m_oFolder.get_track_count();       // 6: track count of that m_oFolder
    for (int i = 7; i < NFCTAG_MEMORY_TO_OCCUPY; ++i)       // 7-15: Empty
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
    if (read_folder_from_card(dummy)) // gets magic cookie.
    {
        return (m_ui32CardCookie == cui32MagicCookie);
    }
    else
    {
       return false;
    }
    // if false Card has never been written with Magic Cookie, thus is unknown to the system
}

const char *NfcControl::stringFromNfcTagNotify(Nfc_interface::eTagState value)
{
#if DEBUGSERIAL
    static const char *NOTIFY_STRING[Nfc_interface::NUMBER_OF_TAG_STATES] = {
        "no Tag",
        "Tag: active, known",
        "Tag: new, known",
        "Tag: new, unknown",
        "", // unused: reserved for DELETE_TAG
        "Error"};

    return NOTIFY_STRING[value];
#endif
    return "";
}