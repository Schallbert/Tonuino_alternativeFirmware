#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Mp3PlayerControl.h>
#include "mocks/unittest_Folder_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
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
    NiceMock<Mock_DfMiniMp3> *m_pDfMini;
    NiceMock<Mock_pinCtrl> *m_pPinCtrl;
    NiceMock<Mock_com> *m_pUsb;
    NiceMock<Mock_delay> *m_pDelayCtrl;
    Mp3PlayerControl *m_pMp3PlrCtrl;
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
    EXPECT_CALL(*m_pDfMini, getVolume()).WillOnce(Return(VOLUME_MAX - 1));
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
    EXPECT_CALL(*m_pDfMini, getVolume()).WillOnce(Return(VOLUME_MIN + 1));
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

TEST_F(PlayerCtrl, playPause_paused_Plays)
{
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).WillOnce(Return(true));
    //EXPECT_CALL(*(m_pMp3PlrCtrl::m_currentFolder), is_valid()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, start()).Times(1);
    m_pMp3PlrCtrl->play_pause();
}

TEST_F(PlayerCtrl, autoplay_trackPlaying_nop)
{
    EXPECT_CALL(*m_pDfMini, loop()).Times(1);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(false));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    EXPECT_CALL(*m_pDfMini, stop()).Times(0);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, autoplay_ALBUM_trackFinished_next)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(&mockEeprom, 0);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 2)).Times(1); // autoplay calls next_track.
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, autoplay_ONELARGETRACK_trackFinished_stop)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ONELARGETRACK, 8);
    testFolder.setup_dependencies(&mockEeprom, 0);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(1); // autoplay calls stop
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, autoplay_LULLABYE_trackFinished_next)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::LULLABYE, 8);
    testFolder.setup_dependencies(&mockEeprom, 0);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 2)).Times(1); // autoplay calls next_track.
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, autoplay_LULLABYE_trackFinished_timeout_stop)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ONELARGETRACK, 8);
    testFolder.setup_dependencies(&mockEeprom, 0);
    // make timeout expire
    for (int i = 0; i<(LULLABYE_TIMER_SECS*1000); ++i)
    {
        m_pMp3PlrCtrl->lullabye_timeout_tick1ms();
    }
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(1); // autoplay calls stop
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, dontSkip_notPlaying_Timeout)
{
    ON_CALL(*m_pPinCtrl, digital_read(_)).WillByDefault(Return(true));       // not playing
    EXPECT_CALL(*m_pDelayCtrl, milli_seconds()).Times(3).WillOnce(Return(0)) // Timer init
        .WillOnce(Return(WAIT_DFMINI_READY - 1))                             // loop 1
        .WillRepeatedly(Return(WAIT_DFMINI_READY));                          // loop 2
    EXPECT_CALL(*m_pDfMini, loop()).Times(1);                                //called twice before timeout
    m_pMp3PlrCtrl->dont_skip_current_track();
}

TEST_F(PlayerCtrl, dontSkip_playing_noTimeout)
{
    ON_CALL(*m_pDelayCtrl, milli_seconds()).WillByDefault(Return(0));                                                                // no time elapsing
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).Times(3).WillOnce(Return(false)).WillOnce(Return(false)).WillRepeatedly(Return(true)); // not playing
    EXPECT_CALL(*m_pDfMini, loop()).Times(1);                                                                                        //called once before isplaying returns true
    m_pMp3PlrCtrl->dont_skip_current_track();
}

TEST_F(PlayerCtrl, nextTrack_noFolder_noop)
{
    // No folder defined!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(PlayerCtrl, nextTrack_FolderNok_noop)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    // dependencies not set!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(PlayerCtrl, nextTrack_FolderOk_playsNext)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(&mockEeprom, 0);
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(PlayerCtrl, prevTrack_noFolder_noop)
{
    // No folder defined!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->prev_track();
}

TEST_F(PlayerCtrl, prevTrack_FolderNok_noop)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    // dependencies not set!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->prev_track();
}

TEST_F(PlayerCtrl, prevTrack_FolderOk_playsNext)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(&mockEeprom, 0);
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->prev_track();
}

// continue writing tests here