#ifndef UNITTEST_NFCTAG_MOCKS_H
#define UNITTEST_NFCTAG_MOCKS_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Arduino.h>
#include <MFRC522_interface.h>
#include "unittest_NfcTag_fakes.h"

// MOCKS
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

    void DelegateToFake()
    {
        ON_CALL(*this, readCard).WillByDefault([this](byte blockAddr, byte *readResult) 
        {
            return m_FakeRead.readCard(blockAddr, readResult);
        });
    }

private:
    Fake_MFRC522 m_FakeRead {};
};
#endif // UNITTEST_NFCTAG_MOCKS_H