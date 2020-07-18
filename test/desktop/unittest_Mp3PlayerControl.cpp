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
        delete m_pMp3PlrCtrl;
    }

protected:
    NiceMock<Mock_DfMiniMp3>* m_pDfMini;
    NiceMock<Mock_pinCtrl>* m_pPinCtrl;
    NiceMock<Mock_com>* m_pUsb;
    NiceMock<Mock_delay>* m_pDelayCtrl;
    Mp3PlayerControl* m_pMp3PlrCtrl;
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

TEST_F(PlayerCtrl, volumeUp_belowMax_volumeIsIncreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillOnce(Return(VOLUME_MAX-1));
    EXPECT_CALL(*m_pDfMini, increaseVolume()).Times(1);
    m_pMp3PlrCtrl->volume_up();
}

TEST_F(PlayerCtrl, volumeUp_Max_volumeNotIncreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillOnce(Return(VOLUME_MAX));
    EXPECT_CALL(*m_pDfMini, increaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3PlrCtrl->volume_up();
}

TEST_F(PlayerCtrl, volumeDown_aboveMin_volumeIsDecreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillOnce(Return(VOLUME_MIN+1));
    EXPECT_CALL(*m_pDfMini, decreaseVolume()).Times(1);
    m_pMp3PlrCtrl->volume_down();
}

TEST_F(PlayerCtrl, volumeDown_Min_volumeNotDecreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillOnce(Return(VOLUME_MIN));
    EXPECT_CALL(*m_pDfMini, decreaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3PlrCtrl->volume_down();
}

TEST_F(PlayerCtrl, playPause_playing_Pauses)
{
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).WillOnce(Return(false));
    EXPECT_CALL(*m_pDfMini, pause()).Times(1);
    m_pMp3PlrCtrl->play_pause();
}

TEST_F(PlayerCtrl, playPause_paused_correctFolder_Plays)
{
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).WillOnce(Return(true));
    //EXPECT_CALL(*(m_pMp3PlrCtrl::m_currentFolder), is_valid()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, start()).Times(1);
    m_pMp3PlrCtrl->play_pause();
}

// continue writing tests here