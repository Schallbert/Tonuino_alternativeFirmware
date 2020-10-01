#include "unittest_NfcTag_mocks.h"

void Fake_Nfc::initReader() { return; }
bool Fake_Nfc::isTagPresent() { return true; }
bool Fake_Nfc::isNewTagPresent() { return true; }
bool Fake_Nfc::writeTag(byte blockAddr, byte *dataToWrite) { return true; }
const char *Fake_Nfc::getMFRC522Notification() { return "no Message"; }

bool Fake_Nfc::readTag(byte blockAddr, byte *readResult)
{
    // copies fakeBufferData to "readResult", simulating read from NFC tag
    for (int i = 0; i < NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
    {
        *(readResult + i) = fakeBufferData[i];
    }
    return true;
}