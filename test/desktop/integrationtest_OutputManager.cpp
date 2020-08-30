#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Mp3PlayerControl.h>
#include <SimpleTimer.h>
#include <NfcTag.h>
#include <MFRC522_interface.h>

#include "mocks/unittest_NfcTag_mocks.h"
#include "mocks/unittest_Folder_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"

#include <OutputManager.h>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class OutputManagerTest : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pDfMini = new NiceMock<Mock_DfMiniMp3>;
        m_pPinCtrl = new NiceMock<Mock_pinCtrl>;
        m_pUsb = new NiceMock<Mock_com>;
        m_pDelayCtrl = new NiceMock<Mock_delay>;
        m_pLullabyeTimer = new SimpleTimer{};
        m_pMp3 = new Mp3PlayerControl(m_pDfMini, m_pPinCtrl, m_pUsb, m_pDelayCtrl, m_pLullabyeTimer);

        m_pSysPwr = new NiceMock<Mock_PowerManager>;
        m_pMfrc = new NiceMock<Mock_MFRC522>;
        m_pNfcTag = new NfcTag(m_pMfrc);
        m_pMenuTimer = new SimpleTimer{};
        m_pEeprom = new NiceMock<Mock_Eeprom>;

        m_pOutputManager = new OutputManager(m_pUsb,
                                             m_pSysPwr,
                                             m_pNfcTag,
                                             m_pMp3,
                                             m_pMenuTimer,
                                             m_pEeprom,
                                             5);
    }

    virtual void TearDown()
    {
        delete m_pOutputManager;

        delete m_pMp3;
        delete m_pMfrc;

        delete m_pUsb;
        delete m_pSysPwr;
        delete m_pNfcTag;
        delete m_pDfMini;
        delete m_pPinCtrl;
        delete m_pDelayCtrl;
        delete m_pLullabyeTimer;
        delete m_pMenuTimer;
        delete m_pEeprom;
    }

protected:
    NiceMock<Mock_DfMiniMp3> *m_pDfMini;
    NiceMock<Mock_pinCtrl> *m_pPinCtrl;
    NiceMock<Mock_com> *m_pUsb;
    NiceMock<Mock_delay> *m_pDelayCtrl;
    SimpleTimer *m_pLullabyeTimer{nullptr};

    Mp3PlayerControl *m_pMp3;

    NiceMock<Mock_PowerManager> *m_pSysPwr{nullptr};
    MFRC522_interface *m_pMfrc{nullptr};
    NfcTag* m_pNfcTag{nullptr};

    SimpleTimer *m_pMenuTimer{nullptr};
    NiceMock<Mock_Eeprom> *m_pEeprom{nullptr};

    OutputManager* m_pOutputManager{nullptr};
};

using ::testing::Return;

TEST_F(OutputManagerTest, setInputStates_dependenciesCalled)
{
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).Times(1);

    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::NO_ACTION);
}
