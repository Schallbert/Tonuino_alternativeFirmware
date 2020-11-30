#include "unittest_Nfc_mocks.h"

void Fake_Nfc::initNfc()
{
    return;
}
Nfc_interface::eTagState Fake_Nfc::getTagPresence()
{
    return Nfc_interface::ACTIVE_KNOWN_TAG;
}
bool Fake_Nfc::writeTag(byte blockAddress, byte *dataToWrite)
{
    return true;
}
bool Fake_Nfc::readTag(byte blockAddress, byte *readResult)
{
    for (int i = 0; i < NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
    {
        *(readResult + i) = fakeBufferData[i];
    }
    return true;
}

bool Fake_MFRC522_MifareMini1k4k::tagRead(byte blockAddress, byte *buffer, byte bufferSize) 
{
    for (int i = 0; i < bufferSize; ++i)
    {
        *(buffer + i) = fakeBufferData[i];
    }
    return true;
}

bool Fake_MFRC522_MifareUltralight::tagRead(byte blockAddress, byte *buffer, byte bufferSize) 
{
    static uint8_t lastBlockNr{0};
    const uint8_t BLOCK_SIZE{4}; // Mifare UL's data block size
    const uint8_t NROFITERATIONS{NFCTAG_MEMORY_TO_OCCUPY/BLOCK_SIZE};
    for (int i = 0; i < BLOCK_SIZE; ++i)
    {
        *(buffer + i) = fakeBufferData[(lastBlockNr * BLOCK_SIZE) + i];
    }
    lastBlockNr++;
    if (lastBlockNr == NROFITERATIONS)
    {
        lastBlockNr = 0; // buffer read complete.
    }
    return true;
}



// RESULT CHECKER
bool resultArrayByteCompare(const byte* compareSrc, byte* compareTgt, uint8_t size)
{
    bool status{true};
    for (int i = 0; i < size; ++i)
    {
        if (*(compareTgt + i) != *(compareSrc + i))
        {
            EXPECT_EQ("", "Error: " + std::to_string(i) + ". element not matching:");
            EXPECT_EQ(*(compareTgt + i), *(compareSrc + i));
            status = false;
        }
    }
    return status;
}