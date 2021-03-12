#include "Tonuino_config.h"

#include "NfcTag_MifareMini1k4k.h"

bool NfcTag_MifareMini1k4k::readTag(byte blockAddress, byte *readResult)
{
    bool status{false};
    const uint8_t BYTES_PER_BLOCK{18}; // Account for checksum
    byte buffer[BYTES_PER_BLOCK] = {};

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_rMfrc522.tagLogin(m_ui8TrailerBlockMini1k4k))
    {
        return status;
    }

    // NFC read procedure for certain types of Tag/Cards: Block of 18 bytes incl. checksum
    status = m_rMfrc522.tagRead(blockAddress, buffer, BYTES_PER_BLOCK);
    if (status)
    {
        NfcTag_interface::copyArray(readResult, buffer, NFCTAG_BYTES_TO_WRITE); // only copy "interesting" bytes
    }
    m_rMfrc522.tagHalt();
    return status;
}

bool NfcTag_MifareMini1k4k::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{false};
    const uint8_t BYTES_PER_BLOCK{16};
    // Tag type always writes 16 byte block, even if less data provided
    byte blockOfBytes[BYTES_PER_BLOCK] = {};
    copyArray(blockOfBytes, dataToWrite, NFCTAG_BYTES_TO_WRITE);

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_rMfrc522.tagLogin(m_ui8TrailerBlockMini1k4k))
    {
        return status;
    }
    status = m_rMfrc522.tagWrite(blockAddress, blockOfBytes, BYTES_PER_BLOCK);
    m_rMfrc522.tagHalt();
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
    m_ui8TrailerBlockMini1k4k = blockAddress + 3;
}