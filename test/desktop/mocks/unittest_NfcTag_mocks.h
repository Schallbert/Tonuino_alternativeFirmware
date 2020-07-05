#include <Arduino.h>
#include <MFRC522_interface.h>
#include <NfcTag.h>

class Mock_MFRC522 : public MFRC522_interface
{
public:
    // Method:  output name   input   overrides functionality of interface
    // IF THE METHOD IS INPUTS VOID; JUST WRITE () NEVER WRITE (void) !!!
    MOCK_METHOD(void, initReader, (), (override));
    MOCK_METHOD(bool, isCardPresent, (), (override));
    MOCK_METHOD(bool, isNewCardPresent, (), (override));
    MOCK_METHOD(bool, writeCard, (byte blockAddr, byte *dataToWrite), (override));
    MOCK_METHOD(bool, readCard, (byte blockAddr, byte *readResult), (override));
};