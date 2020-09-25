#include "unittest_NfcTag_mocks.h"

void Fake_MFRC522::initReader() { return; }
bool Fake_MFRC522::isCardPresent() { return true; }
bool Fake_MFRC522::isNewCardPresent() { return true; }
bool Fake_MFRC522::writeCard(byte blockAddr, byte *dataToWrite) { return true; }
const char *Fake_MFRC522::checkMFRC522Notification() { return "no Message"; }

bool Fake_MFRC522::readCard(byte blockAddr, byte *readResult)
{
    // copies fakeBufferData to "readResult", simulating read from NFC tag
    for (int i = 0; i < MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY; ++i) // 7-15: Empty
    {
        *(readResult + i) = fakeBufferData[i];
    }
    return true;
}