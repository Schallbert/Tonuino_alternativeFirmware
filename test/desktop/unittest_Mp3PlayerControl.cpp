#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../Mp3/Mp3PlayerControl_implementation/Mp3PlayerControl_implementation.h"
#include "../Utilities/SimpleTimer.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"

using ::testing::_;
using ::testing::InvokeWithoutArgs;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class Mp3Ctrl : public ::testing::Test
{
protected:
    // Arrange
    virtual void SetUp()
    {
        m_pArduinoHal = new NiceMock<Mock_ArduinoDIcontainer>;
        m_pPinCtrl = new NiceMock<Mock_pinCtrl>;
        m_pSerial = new NiceMock<Mock_serial>;
        m_pEeprom = new NiceMock<Mock_eeprom>;
        // Delegation MUST have taken place before instantiation of using object (Mp3PlrCtrl)
        m_pArduinoHal->DelegateToMockPins(m_pPinCtrl);
        m_pArduinoHal->DelegateToMockSerial(m_pSerial);
        m_pArduinoHal->DelegateToMockEeprom(m_pEeprom);

        m_pDfMini = new NiceMock<Mock_DfMiniMp3>;
        m_pLullabyeTimer = new SimpleTimer{};
        m_pDfMiniMsgTimeout = new SimpleTimer{};

        m_pMp3PlrCtrl = new Mp3PlayerControl(m_pArduinoHal, m_pDfMini, m_pLullabyeTimer, m_pDfMiniMsgTimeout);
    }

    virtual void TearDown()
    {
        delete m_pMp3PlrCtrl;

        delete m_pArduinoHal;
        delete m_pPinCtrl;
        delete m_pSerial;
        delete m_pEeprom;
        

        delete m_pDfMini;
        delete m_pLullabyeTimer;
        delete m_pDfMiniMsgTimeout;
    }

protected:
    NiceMock<Mock_ArduinoDIcontainer> *m_pArduinoHal;
    NiceMock<Mock_pinCtrl> *m_pPinCtrl;
    NiceMock<Mock_serial> *m_pSerial;
    NiceMock<Mock_eeprom> *m_pEeprom;
    NiceMock<Mock_DfMiniMp3> *m_pDfMini;
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pDfMiniMsgTimeout{nullptr};

    Mp3PlayerControl *m_pMp3PlrCtrl;
};

class Mp3CtrlDebugOutput : public Mp3Ctrl
{
};

#if DEBUGSERIAL
TEST_F(Mp3CtrlDebugOutput, messageHeadersCorrect)
{
    EXPECT_CALL(*m_pSerial, com_println(_)).Times(2); // message content
    EXPECT_CALL(*m_pSerial, com_println("PLAYER CONTROL DEBUG:"));
    EXPECT_CALL(*m_pSerial, com_println("MP3 DEBUG: DfMiniMp3"));

    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(Mp3CtrlDebugOutput, noAction_printsNoMessage)
{
    EXPECT_CALL(*m_pSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pSerial, com_println("No Message"));

    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(Mp3CtrlDebugOutput, volumeUp_printsVolUp)
{
    EXPECT_CALL(*m_pSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pSerial, com_println("volume up"));

    m_pMp3PlrCtrl->volume_up();
    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(Mp3CtrlDebugOutput, volumeDown_printsVolDown)
{
    EXPECT_CALL(*m_pSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pSerial, com_println("volume down"));

    m_pMp3PlrCtrl->volume_down();
    m_pMp3PlrCtrl->print_debug_message();
}

TEST_F(Mp3CtrlDebugOutput, play_printsPlay)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    EXPECT_CALL(*m_pSerial, com_println(_)).Times(3); // message content
    EXPECT_CALL(*m_pSerial, com_println("play folder"));

    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->print_debug_message();
}
#endif

TEST_F(Mp3Ctrl, ClassConstructorMethodsCalled)
{
    EXPECT_CALL(*m_pDfMini, begin());
    EXPECT_CALL(*m_pDfMini, setEq(DFMINI_EQ_SETTING));
    EXPECT_CALL(*m_pDfMini, setVolume(VOLUME_INIT));
    Mp3PlayerControl myMp3(m_pArduinoHal, m_pDfMini, m_pLullabyeTimer, m_pDfMiniMsgTimeout);
}

TEST_F(Mp3Ctrl, AutoPlayCalledOnLoop)
{
    EXPECT_CALL(*m_pDfMini, loop());
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(false));
    m_pMp3PlrCtrl->loop();
}

TEST_F(Mp3Ctrl, volumeUp_belowMax_volumeIsIncreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MAX - 1));
    EXPECT_CALL(*m_pDfMini, increaseVolume()).Times(1);
    m_pMp3PlrCtrl->volume_up();
}

TEST_F(Mp3Ctrl, volumeUp_Max_volumeNotIncreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MAX));
    EXPECT_CALL(*m_pDfMini, increaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3PlrCtrl->volume_up();
}

TEST_F(Mp3Ctrl, volumeDown_aboveMin_volumeIsDecreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MIN + 1));
    EXPECT_CALL(*m_pDfMini, decreaseVolume()).Times(1);
    m_pMp3PlrCtrl->volume_down();
}

TEST_F(Mp3Ctrl, volumeDown_Min_volumeNotDecreased)
{
    EXPECT_CALL(*m_pDfMini, getVolume()).WillRepeatedly(Return(VOLUME_MIN));
    EXPECT_CALL(*m_pDfMini, decreaseVolume()).Times(0); // not allowed to increase volume here
    m_pMp3PlrCtrl->volume_down();
}

TEST_F(Mp3Ctrl, playPause_playing_Pauses)
{
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).WillOnce(Return(false));
    EXPECT_CALL(*m_pDfMini, pause()).Times(1);
    m_pMp3PlrCtrl->play_pause();
}

TEST_F(Mp3Ctrl, playPause_paused_Plays)
{
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).WillOnce(Return(true));
    //EXPECT_CALL(*(m_pMp3PlrCtrl::m_currentFolder), is_valid()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, start()).Times(1);
    m_pMp3PlrCtrl->play_pause();
}

TEST_F(Mp3Ctrl, autoplay_trackPlaying_nop)
{
    EXPECT_CALL(*m_pDfMini, loop()).Times(1);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(false));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    EXPECT_CALL(*m_pDfMini, stop()).Times(0);
    m_pMp3PlrCtrl->loop();
}

TEST_F(Mp3Ctrl, autoplay_ALBUM_trackFinished_next)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 2)).Times(1); // autoplay calls next_track.
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(Mp3Ctrl, autoplay_ONELARGETRACK_trackFinished_stop)
{
    Folder testFolder(1, Folder::ONELARGETRACK, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(1);                // autoplay calls stop
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(Mp3Ctrl, autoplay_LULLABYE_trackFinished_next)
{
    Folder testFolder(1, Folder::LULLABYE, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 2)).Times(1); // autoplay calls next_track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(Mp3Ctrl, autoplay_LULLABYE_trackFinished_borderline_next)
{
    Folder testFolder(1, Folder::LULLABYE, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    m_pLullabyeTimer->start(LULLABYE_TIMEOUT_SECS);
    // make timeout expire
    for (long i = 0; i < (LULLABYE_TIMEOUT_SECS - 1); ++i)
    {
        m_pLullabyeTimer->timerTick();
    }
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 2)).Times(1); // autoplay calls next_track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(Mp3Ctrl, autoplay_LULLABYE_trackFinished_timeout_stop)
{

    Folder testFolder(1, Folder::ONELARGETRACK, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    // make timeout expire
    m_pLullabyeTimer->start(LULLABYE_TIMEOUT_SECS);
    for (int i = 0; i < (LULLABYE_TIMEOUT_SECS); ++i)
    {
        m_pLullabyeTimer->timerTick();
    }
    EXPECT_CALL(*m_pDfMini, checkTrackFinished()).WillOnce(Return(true));
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, 1)).Times(1); // play_folder calls first track.
    EXPECT_CALL(*m_pDfMini, stop()).Times(1);                // autoplay calls stop
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->loop();
}

TEST_F(Mp3Ctrl, playPrompt_noSkip_notPlaying_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;

    ON_CALL(*m_pPinCtrl, digital_read(_)).WillByDefault(Return(true)); // not playing                                                               // not playing
    ON_CALL(*m_pDfMini, loop()).WillByDefault(InvokeWithoutArgs(m_pDfMiniMsgTimeout, &SimpleTimer::timerTick)); //called twice before timeout
    
    m_pMp3PlrCtrl->play_prompt(prompt);
}

TEST_F(Mp3Ctrl, playPrompt_noSkip_notFinishing_Timeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;

    ON_CALL(*m_pPinCtrl, digital_read(_)).WillByDefault(Return(false)); // playing                                                                           // not playing
    ON_CALL(*m_pDfMini, loop()).WillByDefault(InvokeWithoutArgs(m_pDfMiniMsgTimeout, &SimpleTimer::timerTick)); //called twice before timeout
    
    m_pMp3PlrCtrl->play_prompt(prompt);
}

TEST_F(Mp3Ctrl, playPrompt_noSkip_playing_noTimeout)
{
    VoicePrompt prompt;
    prompt.allowSkip = false;
    prompt.promptId = MSG_CONFIRMED;
    // timeout not elapsing
    EXPECT_CALL(*m_pPinCtrl, digital_read(_)).Times(3).WillOnce(Return(false)).WillOnce(Return(false)).WillRepeatedly(Return(true)); // not playing
    EXPECT_CALL(*m_pDfMini, loop()).Times(1);                                                                                        //called once before isplaying returns true
    
    m_pMp3PlrCtrl->play_prompt(prompt);
}

TEST_F(Mp3Ctrl, nextTrack_noFolder_noop)
{
    // No folder defined!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(Mp3Ctrl, nextTrack_FolderSAVEPROGRESS_dependencyNotSet_noop)
{

    Folder testFolder(1, Folder::SAVEPROGRESS, 8);
    // dependencies not set
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(Mp3Ctrl, nextTrack_FolderRANDOM_dependencyNotSet_noop)
{
    Folder testFolder(1, Folder::RANDOM, 8);
    // dependencies not set
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(Mp3Ctrl, nextTrack_FolderALBUM_dependencyNotSet_playsNext)
{
    Folder testFolder(1, Folder::SAVEPROGRESS, 8);
    // dependencies not set
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(Mp3Ctrl, nextTrack_FolderALBUM_dependencySet_playsNext)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(Mp3Ctrl, nextTrack_FolderSAVEPROGRESS_dependencySet_playsNext)
{
    Folder testFolder(1, Folder::SAVEPROGRESS, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);

    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->next_track();
}

TEST_F(Mp3Ctrl, prevTrack_noFolder_noop)
{
    // No folder defined!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(0);
    m_pMp3PlrCtrl->prev_track();
}

TEST_F(Mp3Ctrl, prevTrack_FolderALBUM_dependenciesNotSet_playsPrev)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    // dependencies not set!
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->prev_track();
}

TEST_F(Mp3Ctrl, prevTrack_FolderOk_playsPrev)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, playFolderTrack(_, _)).Times(2);
    m_pMp3PlrCtrl->play_folder(&testFolder);
    m_pMp3PlrCtrl->prev_track();
}

TEST_F(Mp3Ctrl, get_trackCount_noFolder_Returns0)
{
    EXPECT_CALL(*m_pDfMini, getFolderTrackCount(1)).WillRepeatedly(Return(0));
    EXPECT_EQ(0, m_pMp3PlrCtrl->get_trackCount_of_folder(1));
}

TEST_F(Mp3Ctrl, get_trackCount_Folder_ReturnsNumber)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, getFolderTrackCount(1)).WillRepeatedly(Return(8));
    EXPECT_EQ(8, m_pMp3PlrCtrl->get_trackCount_of_folder(1));
}

TEST_F(Mp3Ctrl, get_trackCount_TrackNumberTooHigh_Returns0)
{
    Folder testFolder(1, Folder::ALBUM, 8);
    testFolder.setup_dependencies(m_pArduinoHal);
    EXPECT_CALL(*m_pDfMini, getFolderTrackCount(1)).WillRepeatedly(Return(256));
    EXPECT_EQ(0, m_pMp3PlrCtrl->get_trackCount_of_folder(1));
}