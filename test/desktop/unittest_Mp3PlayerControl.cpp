#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Mp3PlayerControl.h>
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;

// Fixture
class PlayerCtrl : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pDfMini = new NiceMock<Mock_DfMiniMp3>;
        m_pPinCtrl = new NiceMock<Mock_pinCtrl>;
        m_pUsb = new NiceMock<Mock_com>;
        m_pDelayCtrl = new NiceMock<Mock_delay>;
        m_pMp3PlrCtrl = new Mp3PlayerControl(m_pDfMini, m_pPinCtrl, m_pUsb, m_pDelayCtrl);
    }

    virtual void TearDown()
    {
        delete m_pDfMini;
        delete m_pDelayCtrl;
        delete m_pPinCtrl;
        delete m_pUsb;
    }

protected:
    NiceMock<Mock_DfMiniMp3>* m_pDfMini;
    Mp3PlayerControl* m_pMp3PlrCtrl;
    Arduino_interface_pins* m_pPinCtrl;
    Arduino_interface_com* m_pUsb;
    Arduino_interface_delay* m_pDelayCtrl;
};

using ::testing::Return;

TEST_F(PlayerCtrl, ClassConstructorMethodsCalled)
{
    EXPECT_CALL(*m_pDfMini, begin());
    EXPECT_CALL(*m_pDfMini, setEq(DFMINI_EQ_SETTING));
    EXPECT_CALL(*m_pDfMini, setVolume(VOLUME_INIT));
    Mp3PlayerControl myMp3(m_pDfMini, m_pPinCtrl, m_pUsb, m_pDelayCtrl);
}

TEST_F(PlayerCtrl, AutoPlayCalledOnLoop)
{
    EXPECT_CALL(*m_pDfMini, loop());
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(false));
    m_pMp3PlrCtrl->loop();
}

// continue writing tests here