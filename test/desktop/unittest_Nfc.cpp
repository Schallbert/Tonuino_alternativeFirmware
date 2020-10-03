#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Nfc_mocks.h"
#include "../Nfc/Nfc_implementation/Nfc_implementation.h"

// This file tests the Nfc implementation
using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// TEST FIXTURE
class Nfc_getTagPresence : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pMfrc = new NiceMock<Mock_MFRC522>;
        m_pNfc = new Nfc_implementation(m_pMfrc);
    }

    virtual void TearDown()
    {
        delete m_pNfc;
        delete m_pMfrc;
    }

protected:
    NiceMock<Mock_MFRC522> *m_pMfrc;
    Nfc_implementation *m_pNfc{nullptr};
};

// TESTS
TEST_F(Nfc_getTagPresence, noTag_returnsNO_TAG)
{
    ON_CALL(*m_pMfrc, isCardPresent()).WillByDefault(Return(Nfc_interface::NO_TAG));
    EXPECT_EQ(Nfc_interface::NO_TAG, m_pNfc->getTagPresence());
}