#include "Tonuino_config.h"

#include "NfcTag_MifareUltralight.h"

bool NfcTag_MifareUltralight::readTag(byte blockAddress, byte *readResult)
{
    bool status{true};
    byte buffer[determineBlockCount() * MIFARE_UL_BLOCK_SIZE] = {};

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_rMfrc522.tagLogin(ULTRALIGHTSTARTPAGE))
    {
        return false;
    }

    for (uint8_t i{0}; i < determineBlockCount(); ++i)
    {
        status &= m_rMfrc522.tagRead(blockAddress + i, buffer + (i * MIFARE_UL_BLOCK_SIZE), MIFARE_UL_BLOCK_SIZE);
        // copy 4byte blocks to buffer
    }

    if (status)
    {
        NfcTag_interface::copyArray(readResult, buffer, NFCTAG_MEMORY_TO_OCCUPY); // ignores checksum bytes
    }
    m_rMfrc522.tagHalt();
    return status;
}

bool NfcTag_MifareUltralight::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{true};

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_rMfrc522.tagLogin(ULTRALIGHTSTARTPAGE))
    {
        return false;
    }

    for (uint8_t i = 0; i < determineBlockCount(); ++i)
    {
        byte buffer[MIFARE_UL_BLOCK_SIZE] = {}; // initialize with 0s
        NfcTag_interface::copyArray(buffer, dataToWrite + (i * MIFARE_UL_BLOCK_SIZE), MIFARE_UL_BLOCK_SIZE); // write 4 bytes to buffer
        // only the first 4 bytes are actually written. Rest is kept 0.
        status &= m_rMfrc522.tagWrite(blockAddress + i, buffer, MIFARE_UL_BLOCK_SIZE);
    }
    m_rMfrc522.tagHalt();
    return status;
}

uint8_t NfcTag_MifareUltralight::determineBlockCount()
{
    uint8_t numberOfBlocks{NFCTAG_MEMORY_TO_OCCUPY / MIFARE_UL_BLOCK_SIZE};
    if(NFCTAG_MEMORY_TO_OCCUPY % MIFARE_UL_BLOCK_SIZE)
    {
        ++numberOfBlocks; // account for integer division
    }
    return numberOfBlocks;
}

void NfcTag_MifareUltralight::checkAndRectifyBlockAddress(byte &blockAddress)
{
    // Make sure that blockAddr is within allowed range for MIFARE ultralight
    if (blockAddress < ULTRALIGHTSTARTPAGE)
    {
        blockAddress = ULTRALIGHTSTARTPAGE;
    }
    else if (blockAddress > (ULTRALIGHTSTOPPAGE - determineBlockCount()))
    {
        blockAddress = ULTRALIGHTSTOPPAGE - determineBlockCount();
    }
}