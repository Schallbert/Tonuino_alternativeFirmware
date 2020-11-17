#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../OutputManager/PlaybackControl.h"

#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"
#include "mocks/unittest_Mp3PlayerControl_mocks.h"
#include "mocks/unittest_ErrorHandler_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Sequence;

// TEST FIXTURE
class PlaybackControlTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_pPlaybackControl = new PlaybackControl(&m_arduHalMock,
                                                 &m_powerManagerMock,
                                                 &m_nfcControlMock,
                                                 &m_mp3PlayerControlMock,
                                                 &m_errorHandlerMock);
    }

    virtual void TearDown()
    {
        delete m_pPlaybackControl;
    }

protected:
    NiceMock<Mock_ArduinoDIcontainer> m_arduHalMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_Mp3PlayerControl> m_mp3PlayerControlMock{};
    NiceMock<Mock_ErrorHandler> m_errorHandlerMock{};

    PlaybackControl *m_pPlaybackControl{nullptr};
};

TEST_F(PlaybackControlTest, newRegisteredTag_readSuccessful_requestsPlayFolder)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    ON_CALL(m_nfcControlMock, read_folder_from_card(_)).WillByDefault(Return(true));

    EXPECT_CALL(m_mp3PlayerControlMock, play_folder(_));
    m_pPlaybackControl->loop();
}

TEST_F(PlaybackControlTest, newRegisteredTag_readFails_setsTagReadError)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    ON_CALL(m_nfcControlMock, read_folder_from_card(_)).WillByDefault(Return(false));

    EXPECT_CALL(m_errorHandlerMock, setTagReadError());
    m_pPlaybackControl->loop();
}

TEST_F(PlaybackControlTest, newRegisteredTag_mp3TrackcountIs0_setsFolderError)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NEW_REGISTERED_TAG));
    ON_CALL(m_nfcControlMock, read_folder_from_card(_)).WillByDefault(Return(true));

    EXPECT_CALL(m_errorHandlerMock, setFolderError());
    m_pPlaybackControl->loop();
}

// Spot check testing NO_TAG case only (as dispatcher tested on sample basis only)
TEST_F(PlaybackControlTest, noTag_playPause_callsPlayPause)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NO_TAG));
    m_pPlaybackControl->setUserInput(UserInput::PLAY_PAUSE);

    EXPECT_CALL(m_mp3PlayerControlMock, play_pause());
    m_pPlaybackControl->loop();
}

TEST_F(PlaybackControlTest, noTag_help_callsHelp)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NO_TAG));
    m_pPlaybackControl->setUserInput(UserInput::PP_LONGPRESS);

    EXPECT_CALL(m_errorHandlerMock, setHelpRequested());
    m_pPlaybackControl->loop();
}

TEST_F(PlaybackControlTest, noTag_next_callsNextTrack)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NO_TAG));
    m_pPlaybackControl->setUserInput(UserInput::NEXT_TRACK);

    EXPECT_CALL(m_mp3PlayerControlMock, next_track());
    m_pPlaybackControl->loop();
}

TEST_F(PlaybackControlTest, noTag_prev_callsPrevTrack)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NO_TAG));
    m_pPlaybackControl->setUserInput(UserInput::PREV_TRACK);

    EXPECT_CALL(m_mp3PlayerControlMock, prev_track());
    m_pPlaybackControl->loop();
}

TEST_F(PlaybackControlTest, noTag_incVol_callsVolumeUp)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NO_TAG));
    m_pPlaybackControl->setUserInput(UserInput::INC_VOLUME);

    EXPECT_CALL(m_mp3PlayerControlMock, volume_up());
    m_pPlaybackControl->loop();
}

TEST_F(PlaybackControlTest, noTag_decVol_callsNextTrack)
{
    ON_CALL(m_nfcControlMock, get_tag_presence()).WillByDefault(Return(Nfc_interface::NO_TAG));
    m_pPlaybackControl->setUserInput(UserInput::DEC_VOLUME);

    EXPECT_CALL(m_mp3PlayerControlMock, volume_down());
    m_pPlaybackControl->loop();
}