#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Mp3/Mp3Control/Mp3Control_implementation.h"
#include "../Utilities/SimpleTimer.h"

#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ErrorHandler_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class Mp3ControlTest : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pMp3Control = new Mp3Control(&m_dfMiniMock,
                                       &m_mp3PlayMock,
                                       &m_errorHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pMp3Control;
    }

protected:
    NiceMock<Mock_DfMiniMp3> m_dfMiniMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_ErrorHandler> m_errorHandlerMock{};

    Mp3Control *m_pMp3Control{nullptr};
};

TEST_F(Mp3ControlTest, ClassConstructorMethodsCalled)
{
    EXPECT_CALL(m_dfMiniMock, setVolume(VOLUME_INIT));
    Mp3Control myMp3(&m_dfMiniMock, &m_mp3PlayMock, &m_errorHandlerMock);
}

TEST_F(Mp3ControlTest, loop_callesAutoplay)
{
    EXPECT_CALL(m_mp3PlayMock, autoplay());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeUp_belowMax_volumeIsIncreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MAX - 1));
    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(1);
    m_pMp3Control->volumeUp();
}

TEST_F(Mp3ControlTest, volumeUp_Max_volumeNotIncreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MAX));
    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3Control->volumeUp();
}

TEST_F(Mp3ControlTest, volumeDown_aboveMin_volumeIsDecreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MIN + 1));
    EXPECT_CALL(m_dfMiniMock, decreaseVolume()).Times(1);
    m_pMp3Control->volumeDown();
}

TEST_F(Mp3ControlTest, volumeDown_Min_volumeNotDecreased)
{
    EXPECT_CALL(m_dfMiniMock, getVolume()).WillRepeatedly(Return(VOLUME_MIN));
    EXPECT_CALL(m_dfMiniMock, decreaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3Control->volumeDown();
}

TEST_F(Mp3ControlTest, play_Start)
{
    EXPECT_CALL(m_dfMiniMock, start());
    m_pMp3Control->play();
}

TEST_F(Mp3ControlTest, pause_Pauses)
{
    EXPECT_CALL(m_dfMiniMock, pause());
    m_pMp3Control->pause();
}

TEST_F(Mp3ControlTest, pause_toggle_Plays)
{
    ON_CALL(m_mp3PlayMock, isPlaying()).WillByDefault(Return(false));

    EXPECT_CALL(m_dfMiniMock, start());
    m_pMp3Control->togglePlayPause();
}

TEST_F(Mp3ControlTest, play_toggle_Pauses)
{
    ON_CALL(m_mp3PlayMock, isPlaying()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, pause());
    m_pMp3Control->togglePlayPause();
}