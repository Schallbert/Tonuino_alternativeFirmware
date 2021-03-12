#include "Tonuino_config.h"

#include "NfcTag_MifareUltralight.h"

bool NfcTag_MifareUltralight::readTag(byte blockAddress, byte *readResult)
{
    bool status{false};
    const uint8_t BYTES_TO_TRANSFER{18}; // Account for checksum MFRC522.cpp
    byte buffer[BYTES_TO_TRANSFER] = {};

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_rMfrc522.tagLogin(ULTRALIGHTSTARTPAGE))
    {
        return status;
    }

    status = m_rMfrc522.tagRead(blockAddress, buffer, BYTES_TO_TRANSFER); // reads consecutive blocks

    if (status)
    {
        NfcTag_interface::copyArray(readResult, buffer, NFCTAG_BYTES_TO_WRITE); // ignores checksum bytes
    }
    m_rMfrc522.tagHalt();
    return status;
}

bool NfcTag_MifareUltralight::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{true};
    const uint8_t BYTES_TO_TRANSFER{16}; // See MFRC522.cpp documentation

    checkAndRectifyBlockAddress(blockAddress);
    if (!m_rMfrc522.tagLogin(ULTRALIGHTSTARTPAGE))
    {
        return false;
    }

    for (uint8_t i = 0; i < determineBlockCount(); ++i)
    {
        byte buffer[BYTES_TO_TRANSFER] = {};
        // write 4 bytes to buffer's least significant bits
        NfcTag_interface::copyArray(buffer, dataToWrite + (i * MIFARE_UL_BLOCK_SIZE), MIFARE_UL_BLOCK_SIZE); 
        // only the first 4 bytes are actually written. Rest is kept 0.
        status &= m_rMfrc522.tagWrite(blockAddress + i, buffer, BYTES_TO_TRANSFER);
    }
    m_rMfrc522.tagHalt();
    return status;
}

uint8_t NfcTag_MifareUltralight::determineBlockCount()
{
    uint8_t numberOfBlocks{NFCTAG_BYTES_TO_WRITE / MIFARE_UL_BLOCK_SIZE};
    if(NFCTAG_BYTES_TO_WRITE % MIFARE_UL_BLOCK_SIZE)
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