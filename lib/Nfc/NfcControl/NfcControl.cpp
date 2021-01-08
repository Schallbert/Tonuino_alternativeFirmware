#include "NfcControl.h"
#include "Messages_interface.h"


void NfcControl::init()
{
    m_rNfc.initNfc();
}

NfcControl_interface::eTagState NfcControl::getTagPresence()
{
    // Adds "known tag" information if a new tag has been placed.
    // Otherwise, just wrapper for layer down method.
    NfcControl_interface::eTagState tagPresence = m_rNfc.getTagPresence();
    if (tagPresence == NfcControl_interface::NEW_UNKNOWN_TAG)
    {
        if (is_known_card())
        {
            tagPresence = NfcControl_interface::NEW_REGISTERED_TAG;
        }
    }
    Message ctrlStatus{Message(Messages_interface::NFCCONTROL, static_cast<uint8_t>(tagPresence))};
    m_rMessageHandler.printMessage(ctrlStatus);
    return tagPresence;
}

bool NfcControl::writeFolderToTag(const Folder &sourceFolder)
{
    m_oFolder = sourceFolder; // Copy source folder to member object
    if (!m_oFolder.isValid())
    {
        m_oFolder = Folder(); // re-init to empty object
        return false;
    }
    folder_to_buffer(); // Set buffer according to local folder data
    // Get card online and authenticate
    return m_rNfc.writeTag(blockAddressToReadWrite, m_pBuffer);
}

bool NfcControl::eraseTag()
{
    for (int i = 0; i < NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
    {
        m_pBuffer[i] = 0x00;
    }
    return m_rNfc.writeTag(blockAddressToReadWrite, m_pBuffer);
}

bool NfcControl::readFolderFromTag(Folder &targetFolder)
{
    bool status{false}; //unknown or corrupted card
    if (m_rNfc.readTag(blockAddressToReadWrite, m_pBuffer))
    {
        buffer_to_folder();
        if (m_oFolder.isInitiated())
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
    m_pBuffer[4] = (byte)m_oFolder.getFolderId();         // 4: folder picked by the user
    m_pBuffer[5] = (byte)m_oFolder.getPlayMode();         // 5: playback mode picked by the user
    m_pBuffer[6] = (byte)m_oFolder.getTrackCount();       // 6: track count of that m_oFolder
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
    m_oFolder = Folder(folderId, playMode);
    m_oFolder.setTrackCount(trackCount);
}

bool NfcControl::is_known_card()
{
    Folder dummy;
    if (readFolderFromTag(dummy)) // gets magic cookie.
    {
        return (m_ui32CardCookie == cui32MagicCookie);
    }
    else
    {
        return false;
    }
    // if false Card has never been written with Magic Cookie, thus is unknown to the system
}