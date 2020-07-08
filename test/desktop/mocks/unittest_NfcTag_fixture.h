#ifndef UNITTEST_NFCTAG_FIXTURE_H
#define UNITTEST_NFCTAG_FIXTURE_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Arduino.h>
#include <MFRC522_interface.h>
#include <Folder.h>
#include <NfcTag.h>
#include "unittest_NfcTag_fakes.h"
#include "unittest_Folder_mocks.h"


using ::testing::NiceMock;

// TEST FIXTURE
class NfcTagReadWrite : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pMfrc = new NiceMock<Mock_MFRC522>;
        m_pEeprom = new Mock_Eeprom;
        m_pNfc = new NfcTag(m_pMfrc);
        m_pTestFolder = new Folder(fakeBufferData[4],
                                   (Folder::ePlayMode)fakeBufferData[5],
                                   fakeBufferData[6]);
        m_pTestFolder->setup_dependencies(m_pEeprom, 0);
    }

    virtual void TearDown()
    {
        delete m_pMfrc;
        delete m_pNfc;
        delete m_pEeprom;
        delete m_pTestFolder;
    }

protected:
    NiceMock<Mock_MFRC522>* m_pMfrc;
    NfcTag* m_pNfc;
    Mock_Eeprom* m_pEeprom;
    Folder* m_pTestFolder;
};
#endif // UNITTEST_NFCTAG_FIXTURE_H