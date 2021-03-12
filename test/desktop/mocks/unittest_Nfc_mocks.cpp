#include "Tonuino_config.h"
#include "NfcTag/NfcTag_MifareUltralight.h"
#include "Folder.h"

#include "unittest_Nfc_mocks.h"

void Fake_Nfc::initNfc()
{
    return;
}
Message::eMessageContent Fake_Nfc::getTagPresence()
{
    return Message::ACTIVETAG;
}
bool Fake_Nfc::writeTag(byte blockAddress, byte *dataToWrite)
{
    return true;
}
bool Fake_Nfc::readTag(byte blockAddress, byte *readResult)
{
    for (int i = 0; i < NFCTAG_BYTES_TO_WRITE; ++i)
    {
        *(readResult + i) = fakeBufferData[i];
    }
    return true;
}

bool Fake_MFRC522_MifareMini1k4k::tagRead(byte blockAddress, byte *buffer, byte bufferSize)
{
    for (int i = 0; i < NFCTAG_BYTES_TO_WRITE; ++i)
    {
        *(buffer + i) = fakeBufferData[i];
    }
    return true;
}

bool Fake_MFRC522_MifareUltralight::tagRead(byte blockAddress, byte *buffer, byte bufferSize)
{
     for (int i = 0; i < NFCTAG_BYTES_TO_WRITE; ++i)
    {
        *(buffer + i) = fakeBufferData[i];
    }
    return true;
}

// RESULT CHECKER
bool resultArrayByteCompare(const byte *compareSrc, byte *compareTgt, uint8_t size)
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