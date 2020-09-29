#include "../NfcTag/NfcTag_implementation/NfcTag_MifareMini1k4k.h"

bool NfcTag_MifareMini1k4k::readTag(byte blockAddress, byte *readResult)
{
    bool status{false};
    checkAndRectifyBlockAddress(blockAddress);
    if (!m_pMfrc522->tagLogin(m_ui8TrailerBlockMini1k4k))
    {
        return status;
    }
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY + 2; // Account for checksum
    byte buffer[ui8_bufSize] = {};
    // NFC read procedure for certain types of Tag/Cards: Block of 18 bytes incl. checksum
    status = m_pMfrc522->tagRead(blockAddress, buffer, &ui8_bufSize);
    memcpy(readResult, buffer, NFCTAG_MEMORY_TO_OCCUPY); // ignores checksum bytes
    m_pMfrc522->tagHalt();
    return (status);
}

bool NfcTag_MifareMini1k4k::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{false};
    checkAndRectifyBlockAddress(blockAddress);
    if (!m_pMfrc522->tagLogin(m_ui8TrailerBlockMini1k4k))
    {
        return status;
    }
    status = m_pMfrc522->tagWrite(blockAddress, dataToWrite, NFCTAG_MEMORY_TO_OCCUPY);
    m_pMfrc522->tagHalt();
    return status;
}

void NfcTag_MifareMini1k4k::checkAndRectifyBlockAddress(byte &blockAddress)
{
    if (blockAddress == 0)
    {
        blockAddress = 1; // 0 is reserved!
    }
    if ((blockAddress % 4) == SECTORSTRAILERBLOCKMINI1K4K)
    {
        ++blockAddress;
    }
    // 4Blocks per sector
    m_ui8SectorMini1k4k = blockAddress / 4;
    // Block 3 of each sector is the trailer block containing keys
    // (init 0xFFFFFFFF), we won't touch those!
    m_ui8TrailerBlockMini1k4k = m_ui8SectorMini1k4k * 4 + 3;
}