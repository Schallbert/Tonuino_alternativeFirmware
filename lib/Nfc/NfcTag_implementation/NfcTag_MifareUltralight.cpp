#include "../Nfc/NfcTag_implementation/NfcTag_MifareUltralight.h"

bool NfcTag_MifareUltralight::readTag(byte blockAddress, byte *readResult)
{
    bool status{true};
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY + 2; // Account for checksum
    byte buffer[ui8_bufSize] = {};

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_pMfrc522->tagLogin(ULTRALIGHTSTARTPAGE))
    {
        return false;
    }
    for (uint8_t i = 0; i < (NFCTAG_MEMORY_TO_OCCUPY / MIFARE_UL_BLOCK_SIZE); ++i)
    {
        status &= m_pMfrc522->tagRead(blockAddress + i, buffer + (i * MIFARE_UL_BLOCK_SIZE), ui8_bufSize);
        // copy 4byte block from buffer2 to buffer
    }

    if (status)
    {
        NfcTag_interface::copyArray(readResult, buffer, NFCTAG_MEMORY_TO_OCCUPY); // ignores checksum bytes
    }
    m_pMfrc522->tagHalt();
    return status;
}

bool NfcTag_MifareUltralight::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{true};

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_pMfrc522->tagLogin(ULTRALIGHTSTARTPAGE))
    {
        return false;
    }

    for (uint8_t i = 0; i < (NFCTAG_MEMORY_TO_OCCUPY / MIFARE_UL_BLOCK_SIZE); ++i)
    {
        byte buffer[NFCTAG_MEMORY_TO_OCCUPY] = {}; // initialize with 0s
        NfcTag_interface::copyArray(buffer, dataToWrite + (i * MIFARE_UL_BLOCK_SIZE), MIFARE_UL_BLOCK_SIZE); // write 4 bytes to buffer
        // only the first 4 bytes are actually written. Rest is kept 0.
        status &= m_pMfrc522->tagWrite(blockAddress + i , buffer, NFCTAG_MEMORY_TO_OCCUPY);
    }
    m_pMfrc522->tagHalt();
    return status;
}

void NfcTag_MifareUltralight::checkAndRectifyBlockAddress(byte &blockAddress)
{
    // Make sure that blockAddr is within allowed range for MIFARE ultralight
    if (blockAddress < ULTRALIGHTSTARTPAGE)
    {
        blockAddress = ULTRALIGHTSTARTPAGE;
    }
    else if (blockAddress > ULTRALIGHTSTOPPAGE)
    {
        blockAddress = ULTRALIGHTSTOPPAGE;
    }
}