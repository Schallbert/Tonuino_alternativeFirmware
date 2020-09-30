#include "../Nfc/NfcTag_implementation/NfcTag_MifareUltralight.h"

bool NfcTag_MifareUltralight::readTag(byte blockAddress, byte *readResult)
{
    bool status{true};
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY + 2; // Account for checksum
    byte buffer[ui8_bufSize];

    checkAndRectifyBlockAddress(blockAddress);
    // For MIFARE Ultralight, Authentication is not a must
    // to keep it lightweight, we will not introduce it.
    for (uint8_t i = 0; i < MIFARE_UL_BLOCK_SIZE; ++i)
    {
        status &= m_pMfrc522->tagRead(blockAddress + 1, buffer, &ui8_bufSize);
        // copy 4byte block from buffer2 to buffer
        memcpy(readResult + (i * MIFARE_UL_BLOCK_SIZE), buffer, MIFARE_UL_BLOCK_SIZE);
    }
    m_pMfrc522->tagHalt();
    return status;
}

bool NfcTag_MifareUltralight::writeTag(byte blockAddress, byte *dataToWrite)
{
    bool status{true};
    byte buffer[NFCTAG_MEMORY_TO_OCCUPY] = {};
    byte ui8_bufSize = NFCTAG_MEMORY_TO_OCCUPY;
    
    checkAndRectifyBlockAddress(blockAddress);
    // For MIFARE Ultralight, Authentication is not a must 
    // to keep it lightweight, we will not introduce it.

    for (uint8_t i = 0; i < MIFARE_UL_BLOCK_SIZE; ++i)
    {
        memset(buffer, 0, NFCTAG_MEMORY_TO_OCCUPY);
        // copy 4byte block from data to buffer
        memcpy(buffer, dataToWrite + (i * MIFARE_UL_BLOCK_SIZE), MIFARE_UL_BLOCK_SIZE);
        status &= (MFRC522::StatusCode)m_pMfrc522->tagWrite(blockAddress + i, buffer, ui8_bufSize);
        m_pMfrc522->tagHalt();
    }
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