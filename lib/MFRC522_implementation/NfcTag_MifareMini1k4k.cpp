#include "NfcTag_MifareMini1k4k.h"

bool NfcTag_MifareMini1k4k::readTag(byte blockAddress, byte *readResult)
{
    checkAndRectifyBlockAddress(blockAddress);
    if (!authenticateTag())
    {
        return false;
    }
    MFRC522::StatusCode status = MFRC522::STATUS_ERROR;
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY + 2; // Account for checksum
    byte buffer[ui8_bufSize] = {};
    // NFC read procedure for certain types of Tag/Cards: Block of 18 bytes incl. checksum
    status = m_pMfrc522->MIFARE_Read(blockAddress, buffer, &ui8_bufSize);
    memcpy(readResult, buffer+2, NFCTAG_MEMORY_TO_OCCUPY); // ignores checksum bytes [0,1] // TODO: VERIFY!!!
    return (status == MFRC522::STATUS_OK);
}

bool NfcTag_MifareMini1k4k::writeTag(byte blockAddress, byte *dataToWrite)
{
    checkAndRectifyBlockAddress(blockAddress);
    if (!authenticateTag())
    {
        return false;
    }
    MFRC522::StatusCode status = m_pMfrc522->MIFARE_Write(blockAddress, dataToWrite, NFCTAG_MEMORY_TO_OCCUPY);
    return (status == MFRC522::STATUS_OK);
}

bool NfcTag_MifareMini1k4k::authenticateTag()
{
    MFRC522::StatusCode status;
    status = m_pMfrc522->PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A,
        m_ui8TrailerBlockMini1k4k,
        &m_eKey,
        &(m_pMfrc522->uid));
    if (status != MFRC522::STATUS_OK)
    {
        m_eNotification = errorTagAuthenticateFailed;
        return false;
    }
    return true;
}

void NfcTag_MifareMini1k4k::checkAndRectifyBlockAddress(byte &blockAddress)
{
    if (blockAddress == 0)
    {
        blockAddress = 1; // 0 is reserved!
        m_eNotification = errorTagRequestOutOfRange;
    }
    if ((blockAddress % 4) == SECTORSTRAILERBLOCKMINI1K4K)
    {
        ++blockAddress;
        m_eNotification = errorTagRequestOutOfRange;
    }
    // 4Blocks per sector
    m_ui8SectorMini1k4k = blockAddress / 4;
    // Block 3 of each sector is the trailer block containing keys
    // (init 0xFFFFFFFF), we won't touch those!
    m_ui8TrailerBlockMini1k4k = m_ui8SectorMini1k4k * 4 + 3;
}