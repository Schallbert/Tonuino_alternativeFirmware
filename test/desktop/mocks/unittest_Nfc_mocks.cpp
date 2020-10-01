#include "unittest_NfcTag_mocks.h"

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
const char *Fake_Nfc::getNfcNotification()
{
    return "no Message";
}