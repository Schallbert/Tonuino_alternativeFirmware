#include <Arduino.h>
#include <MFC522_interface.h>

class Mock_MFRC522 : public MFRC522_interface
{
    public:
    // Method:  output name  input   overrides functionality of interface
    MOCK_METHOD(void, init, (void), (override));
    MOCK_METHOD(void, isCardPresent, (void), (override));
    MOCK_METHOD(bool, isNewCardPresent, (void), (override));
    MOCK_METHOD(bool, write, (byte blockAddr, byte *dataToWrite), (override));
    MOCK_METHOD(bool, read, (byte blockAddr, byte *readResult), (override));
};