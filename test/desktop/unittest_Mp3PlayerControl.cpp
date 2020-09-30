#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Mp3PlayerControl.h>
#include <SimpleTimer.h>
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"

using ::testing::_;
using ::testing::InvokeWithoutArgs;
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
        m_pArduinoHal = new NiceMock<Mock_ArduinoDIcontainer>;
        m_pArduinoSerial = new NiceMock<Mock_com>;
        m_pLullabyeTimer = new SimpleTimer{};
        m_pDfMiniMsgTimeout = new SimpleTimer{};
        m_pMp3PlrCtrl = new Mp3PlayerControl(m_pDfMini, m_pPinCtrl, m_pArduinoSerial, m_pLullabyeTimer, m_pDfMiniMsgTimeout);
    }

    virtual void TearDown()
    {
        delete m_pMp3PlrCtrl;
        delete m_pArduinoSerial;
        delete m_pDfMini;
        delete m_pPinCtrl;
        delete m_pArduinoSerial;
        delete m_pLullabyeTimer;
        delete m_pDfMiniMsgTimeout;
    }

protected:
    NiceMock<Mock_DfMiniMp3> *m_pDfMini;
    NiceMock<Mock_pinCtrl> *m_pPinCtrl;
    NiceMock<Mock_ArduinoDIcontainer> *m_pArduinoHal;
    NiceMock<Mock_com> *m_pArduinoSerial;
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pDfMiniMsgTimeout{nullptr};

    Mp3PlayerControl *m_pMp3PlrCtrl;
};

class DebugOutput : public PlayerCtrl{};

#if DEBUGSERIAL
TEST_F(DebugOutput, messageHeadersCorrect)
{
    ON_CALL(*m_pArduinoHal, getSerial()).WillByDefault(Return(m_pArduinoSerial));
    EXPECT_CALL(*m_pArduinoSerial, com_println(_)).Times(2); // message content
    EXPECT_CALL(*m_pArduinoSerial, com_println("PLAYER CONTROL DEBUG:"));
    EXPECT_CALL(*m_pArduinoSerial, com_println("MP3 DEBUG: DfMiniMp3"));

    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(DebugOutput, noAction_printsNoMessage)
{
    EXPECT_CALL(*m_pArduinoSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pArduinoSerial, com_println("No Message"));

    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(DebugOutput, volumeUp_printsVolUp)
{
    EXPECT_CALL(*m_pArduinoSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pArduinoSerial, com_println("volume up"));

    m_pMp3PlrCtrl->volume_up();
    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(DebugOutput, volumeDown_printsVolDown)
{
    EXPECT_CALL(*m_pArduinoSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pArduinoSerial, com_println("volume down"));

    m_pMp3PlrCtrl->volume_down();
    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(DebugOutput, play_printsPlay)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    EXPECT_CALL(*m_pArduinoSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pArduinoSerial, com_println("play folder"));

    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->print_debug_message();
}
#endif

TEST_F(PlayerCtrl, ClassConstructorMethodsCalled)
{
    EXPECT_CALL(*m_pDfMini, begin());
    EXPECT_CALL(*m_pDfMini, setEq(DFMINI_EQ_SETTING));
    EXPECT_CALL(*m_pDfMini, setVolume(VOLUME_INIT));
    Mp3PlayerControl myMp3(m_pDfMini, m_pPinCtrl, m_pArduinoSerial, m_pLullabyeTimer, m_pDfMiniMsgTimeout);
}

TEST_F(PlayerCtrl, AutoPlayCalledOnLoop)
{
    EXPECT_CALL(*m_pDfMini, loop());
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(false));
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, volumeUp_belowMax_volumeIsIncreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MAX - 1));
    EXPECT_CALL(*m_pDfMini, increaseVolume()).Times(1);
    m_pMp3PlrCtrl->volume_up();
}

TEST_F(PlayerCtrl, volumeUp_Max_volumeNotIncreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MAX));
    EXPECT_CALL(*m_pDfMini, increaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3PlrCtrl->volume_up();
}

TEST_F(PlayerCtrl, volumeDown_aboveMin_volumeIsDecreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MIN + 1));
    EXPECT_CALL(*m_pDfMini, decreaseVolume()).Times(1);
    m_pMp3PlrCtrl->volume_down();
}

TEST_F(PlayerCtrl, volumeDown_Min_volumeNotDecreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MIN));
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
    testFolder.setup_dependencies(m_pArduinoHal);
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
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(1);                // autoplay calls stop
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, autoplay_LULLABYE_trackFinished_next)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::LULLABYE, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 2)).Times(1); // autoplay calls next_track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, autoplay_LULLABYE_trackFinished_borderline_next)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::LULLABYE, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    m_pLullabyeTimer->start(LULLABYE_TIMEOUT_SECS);
    // make timeout expire
    for (long i = 0; i < (LULLABYE_TIMEOUT_SECS - 1); ++i)
    {
        m_pLullabyeTimer->timer_tick();
    }
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 2)).Times(1); // autoplay calls next_track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, autoplay_LULLABYE_trackFinished_timeout_stop)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ONELARGETRACK, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    // make timeout expire
    m_pLullabyeTimer->start(LULLABYE_TIMEOUT_SECS);
    for (int i = 0; i < (LULLABYE_TIMEOUT_SECS); ++i)
    {
        m_pLullabyeTimer->timer_tick();
    }
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(1);                // autoplay calls stop
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(PlayerCtrl, dontSkip_notPlaying_Timeout)
{
    ON_CALL(*m_pPinCtrl, digital_read(_)).WillByDefault(Return(true));                                                                   // not playing
    EXPECT_CALL(*m_pDfMini, loop()).Times(WAIT_DFMINI_READY).WillOnce(InvokeWithoutArgs(m_pDfMiniMsgTimeout, &SimpleTimer::timer_tick)); //called twice before timeout
    m_pMp3PlrCtrl->dont_skip_current_track();
}

TEST_F(PlayerCtrl, dontSkip_notFinishing_Timeout)
{
    ON_CALL(*m_pPinCtrl, digital_read(_)).WillByDefault(Return(false));                                                                            // not playing
    EXPECT_CALL(*m_pDfMini, loop()).Times(TIMEOUT_PROMPT_PLAYED).WillRepeatedly(InvokeWithoutArgs(m_pDfMiniMsgTimeout, &SimpleTimer::timer_tick)); //called twice before timeout
    m_pMp3PlrCtrl->dont_skip_current_track();
}

TEST_F(PlayerCtrl, dontSkip_playing_noTimeout)
{
    // timeout not elapsing
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

TEST_F(PlayerCtrl, nextTrack_FolderSAVEPROGRESS_dependencyNotSet_noop)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::SAVEPROGRESS, 8);
    // dependencies not set
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(PlayerCtrl, nextTrack_FolderRANDOM_dependencyNotSet_noop)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::RANDOM, 8);
    // dependencies not set
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(PlayerCtrl, nextTrack_FolderALBUM_dependencyNotSet_playsNext)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::SAVEPROGRESS, 8);
    // dependencies not set
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(PlayerCtrl, nextTrack_FolderALBUM_dependencySet_playsNext)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(PlayerCtrl, nextTrack_FolderSAVEPROGRESS_dependencySet_playsNext)
{
    NiceMock<Mock_Eeprom> mockEeprom; // we don't test EEPROM here
    Folder testFolder(1, Folder::SAVEPROGRESS, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
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

TEST_F(PlayerCtrl, prevTrack_FolderALBUM_dependenciesNotSet_playsPrev)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    // dependencies not set!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->prev_track();
}

TEST_F(PlayerCtrl, prevTrack_FolderOk_playsPrev)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->prev_track();
}

TEST_F(PlayerCtrl, get_trackCount_noFolder_Returns0)
{
    EXPECT_CALL(*m_pDfMini, getFolderTrackCount(1)).WillRepeatedly(Return(0));
    EXPECT_EQ(0, m_pMp3PlrCtrl->get_trackCount_of_folder(1));
}

TEST_F(PlayerCtrl, get_trackCount_Folder_ReturnsNumber)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, getFolderTrackCount(1)).WillRepeatedly(Return(8));
    EXPECT_EQ(8, m_pMp3PlrCtrl->get_trackCount_of_folder(1));
}

TEST_F(PlayerCtrl, get_trackCount_TrackNumberTooHigh_Returns0)
{
    Mock_Eeprom mockEeprom;
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, getFolderTrackCount(1)).WillRepeatedly(Return(256));
    EXPECT_EQ(0, m_pMp3PlrCtrl->get_trackCount_of_folder(1));
}