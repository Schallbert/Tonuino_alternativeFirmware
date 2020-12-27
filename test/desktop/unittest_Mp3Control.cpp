#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Tonuino_config.h"
#include "Folder.h"

#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "../UserInput/UserInput/UserInput_interface.h"
#include "../Mp3/Mp3Control/Mp3Control_implementation.h"

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
        m_pMp3Control = new Mp3Control(m_dfMiniMock,
                                       m_mp3PlayMock,
                                       m_nfcControlMock,
                                       m_messageHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pMp3Control;
    }

protected:
    NiceMock<Mock_DfMiniMp3> m_dfMiniMock{};
    NiceMock<Mock_Mp3Play> m_mp3PlayMock{};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};

    Mp3Control *m_pMp3Control{nullptr};
};

TEST_F(Mp3ControlTest, ClassConstructorMethodsCalled)
{
    EXPECT_CALL(m_dfMiniMock, setVolume(VOLUME_INIT));
    Mp3Control myMp3(m_dfMiniMock, m_mp3PlayMock, m_nfcControlMock, m_messageHandlerMock);
}

TEST_F(Mp3ControlTest, loop_blocked_wontDoAnything)
{
    m_pMp3Control->setBlocked(true);

    EXPECT_CALL(m_mp3PlayMock, autoplay()).Times(0);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, loop_callsAutoplay)
{
    EXPECT_CALL(m_mp3PlayMock, autoplay());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, newRegisteredTag_readSuccessful_startsPlayback)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
    m_pMp3Control->setTagState(NfcControl_interface::NEW_REGISTERED_TAG);
    EXPECT_CALL(m_mp3PlayMock, playFolder(_));
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeUp_belowMax_volumeIsIncreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MAX - 1));

    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(1);
    m_pMp3Control->setUserInput(UserInput_interface::INC_VOLUME);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeUp_Max_volumeNotIncreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MAX));

    EXPECT_CALL(m_dfMiniMock, increaseVolume()).Times(0);
    m_pMp3Control->setUserInput(UserInput_interface::INC_VOLUME);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeDown_aboveMin_volumeIsDecreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MIN + 1));

    EXPECT_CALL(m_dfMiniMock, decreaseVolume());
    m_pMp3Control->setUserInput(UserInput_interface::DEC_VOLUME);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, volumeDown_Min_volumeNotDecreased)
{
    ON_CALL(m_dfMiniMock, getVolume()).WillByDefault(Return(VOLUME_MIN));

    EXPECT_CALL(m_dfMiniMock, decreaseVolume()).Times(0);
    m_pMp3Control->setUserInput(UserInput_interface::DEC_VOLUME);
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, prev_PrevTrack)
{
    m_pMp3Control->setUserInput(UserInput_interface::PREV_TRACK);

    EXPECT_CALL(m_mp3PlayMock, playPrev());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, next_NextTrack)
{
    m_pMp3Control->setUserInput(UserInput_interface::NEXT_TRACK);

    EXPECT_CALL(m_mp3PlayMock, playNext());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, help_promptsHelp)
{
    m_pMp3Control->setUserInput(UserInput_interface::PP_LONGPRESS);

    EXPECT_CALL(m_messageHandlerMock, promptMessage(_));
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, pause_toggle_Plays)
{
    m_pMp3Control->setUserInput(UserInput_interface::PLAY_PAUSE);
    ON_CALL(m_dfMiniMock, isPlaying()).WillByDefault(Return(false));

    EXPECT_CALL(m_dfMiniMock, start());
    m_pMp3Control->loop();
}

TEST_F(Mp3ControlTest, play_toggle_Pauses)
{
    m_pMp3Control->setUserInput(UserInput_interface::PLAY_PAUSE);
    ON_CALL(m_dfMiniMock, isPlaying()).WillByDefault(Return(true));

    EXPECT_CALL(m_dfMiniMock, pause());
    m_pMp3Control->loop();
}