#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <SimpleTimer.h>
#include <NfcControl.h>
#include <MFRC522_interface.h>

#include "mocks/unittest_NfcTag_mocks.h"
#include "mocks/unittest_Folder_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"
#include "mocks/unittest_Mp3PlayerControl_mocks.h"

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
        m_pPinCtrl = new NiceMock<Mock_pinCtrl>;
        m_pUsb = new NiceMock<Mock_com>;
        m_pMp3 = new NiceMock<Mock_Mp3PlayerControl>;

        m_pSysPwr = new NiceMock<Mock_PowerManager>;
        m_pMfrc = new NiceMock<Mock_MFRC522>;
        m_pNfcTag = new NfcControl(m_pMfrc);
        m_pMenuTimer = new SimpleTimer{};
        m_pEeprom = new NiceMock<Mock_Eeprom>;

        m_pOutputManager = new OutputManager(m_pUsb,
                                             m_pSysPwr,
                                             m_pNfcTag,
                                             m_pMp3,
                                             m_pMenuTimer,
                                             m_pEeprom,
                                             5);

        // set mp3 status to "not playing"
        ON_CALL(*m_pPinCtrl, digital_read(DFMINI_PIN_ISIDLE)).WillByDefault(Return(true));
    }

    virtual void TearDown()
    {
        delete m_pOutputManager;
        delete m_pMp3;
        delete m_pMfrc;

        delete m_pUsb;
        delete m_pSysPwr;
        delete m_pNfcTag;
        delete m_pPinCtrl;
        delete m_pMenuTimer;
        delete m_pEeprom;
    }

protected:
    NiceMock<Mock_pinCtrl> *m_pPinCtrl;
    NiceMock<Mock_com> *m_pUsb;

    NiceMock<Mock_Mp3PlayerControl> *m_pMp3;

    NiceMock<Mock_PowerManager> *m_pSysPwr{nullptr};
    NiceMock<Mock_MFRC522> *m_pMfrc{nullptr};
    NfcControl *m_pNfcTag{nullptr};

    SimpleTimer *m_pMenuTimer{nullptr};
    NiceMock<Mock_Eeprom> *m_pEeprom{nullptr};

    OutputManager *m_pOutputManager{nullptr};
};

// TESTS

TEST_F(OutputManagerTest, setInputStates_dependenciesCalled)
{
    // Only makes sure that isPlaying() is checked within setInputStates
    EXPECT_CALL(*m_pMp3, is_playing()).Times(1);
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::NO_ACTION);
}

TEST_F(OutputManagerTest, setInputStates_noCardNoAction_deleteMenuNotCalled)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pSysPwr, set_delMenu()).Times(0);
}

TEST_F(OutputManagerTest, setInputStates_noCardNoAction_linkMenuNotCalled)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pSysPwr, set_linkMenu()).Times(0);
}

TEST_F(OutputManagerTest, dispatcher_noCardNoAction_errorHandler_noError)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::NO_ACTION);
    m_pOutputManager->runDispatcher();
    EXPECT_CALL(*m_pMp3, play_specific_file(_)).Times(0); // No Error Message sent
}

TEST_F(OutputManagerTest, dispatcher_NoAction_errorHandler_cardInputOutOfRange)
{
    m_pOutputManager->setInputStates(InputManager::NUMBER_OF_TAG_STATES, UserInput::NO_ACTION);
    // Assert
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCard_errorHandler_userActionOutOfRange)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::NUMBER_OF_REQUESTS);
    // Assert
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCard_errorHandler_userActionError)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::ERROR);
    // Assert
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCardPlayPause_playCalled)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_pause());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardPlayPause_playCalled)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_pause());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCard_readCalled)
{
    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMfrc, readCard(_, _));
    m_pOutputManager->runDispatcher();
}

// LINK MENU SPECIFIC TESTS ---------------------------------------

TEST_F(OutputManagerTest, linkMenu_linkMenuEnterNotRepeated)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_SELECT_FOLDERID)).Times(0); // linkMenu init not called multiple times
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::NEXT_TRACK);
}

TEST_F(OutputManagerTest, linkMenu_entry)
{
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_SELECT_FOLDERID));
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION);
}

TEST_F(OutputManagerTest, linkMenu_lockInLinkMenu)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pMp3, play_specific_file(1)); // Check if we are successfully locked in unknown card menu
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::NEXT_TRACK); // although card state changes
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, linkMenu_nextSelected_nextOptionSelected)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pMp3, play_specific_file(1)).Times(1);
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, linkMenu_folderSelectionWorking)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pMp3, play_specific_file(MAXFOLDERCOUNT)); 
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PREV_TRACK);
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuFolderSelect_folderInvalidSelection0)
{
    EXPECT_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillOnce(Return(0));
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION);
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_specific_file(_));
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->runDispatcher();
    
}

MATCHER_P(folderIdIs, value, "") { return (arg->get_folder_id() == value); }

TEST_F(OutputManagerTest, linkMenu_linkMenuFolderSelect_folderPreviewPlayed)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    EXPECT_CALL(*m_pMp3, play_folder(folderIdIs(1)));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuPlayModeSelect_playModeSelectionWorking)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PREV_TRACK);
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_SELECT_PLAYMODE+static_cast<uint8_t>(Folder::ONELARGETRACK)));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuPlayModeSelect_playModeInvalidSelection0)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_specific_file(_));
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_configureSuccessful)
{
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pMfrc, writeTag(_, _)).WillByDefault(Return(true));
    ON_CALL(*m_pMfrc, readCard(_, _)).WillByDefault(Return(true));
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher();
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_TAGCONFSUCCESS)); 
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_writesInfoToCard)
{
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pMfrc, readCard(_, _)).WillByDefault(Return(true));

    // Simulate navigation through linkMenu with folder1, playMode Album, 8 tracks
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher(); // should set link menu to folder 1
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher(); // should log folder 1
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher(); // should set link menu to playmode ALBUM
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMfrc, writeTag(_, _));                        // Make sure card info is written
    m_pOutputManager->runDispatcher(); // should log playmode and complete linkMenu
}

MATCHER_P3(folderOk, expFolderId, expPlayMode, expTrackCnt, ""){
    return ((arg->get_folder_id() == expFolderId) &&
    (arg->get_play_mode() == expPlayMode) &&
    (arg->get_track_count() == expTrackCnt));
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_startsPlaybackWithCorrectSettings)
{
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pMfrc, writeTag(_, _)).WillByDefault(Return(true)); 
    ON_CALL(*m_pMfrc, readCard(_, _)).WillByDefault(Return(true));

    // Simulate navigation through linkMenu with folder1, playMode Album, 8 tracks
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher(); // should set link menu to folder 1
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher(); // should log folder 1
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher(); // should set link menu to playmode LULLABYE
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_folder(folderOk(1, Folder::LULLABYE, 8)));
    m_pOutputManager->runDispatcher(); // should log playmode and complete linkMenu
}

TEST_F(OutputManagerTest, linkMenu_menuTimeout_resetsMenu)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION); // enter linkMenu
    for (uint16_t i = 0; i <= MENU_TIMEOUT_SECS; ++i)
    {
        m_pMenuTimer->timer_tick(); // make menuTimer elapse
    }
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION); // enter linkMenu
}

TEST_F(OutputManagerTest, linkMenu_userAbort_resetsMenu)
{
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::NO_ACTION); // enter linkMenu
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setInputStates(InputManager::NEW_UNKNOWN_TAG, UserInput::PP_LONGPRESS); // abort linkMenu
    m_pOutputManager->runDispatcher();
}

// END OF LINK MENU SPECIFIC TESTS ---------------------------------------

// DELETE MENU SPECIFIC TESTS --------------------------------------------
TEST_F(OutputManagerTest, deleteCardMenu_entry_playsDeletePrompt)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PP_LONGPRESS); 
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_DELETETAG));
    m_pOutputManager->runDispatcher(); // enter delete menu
}

TEST_F(OutputManagerTest, deleteCardMenu_deleteNotReady_confirmDeletion_replaysDeletePrompt)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PP_LONGPRESS); 
    m_pOutputManager->runDispatcher(); // enter delete menu
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_DELETETAG));
    m_pOutputManager->runDispatcher(); // delete menu cannot complete but stays active
}

TEST_F(OutputManagerTest, deleteCardMenu_deletionReady_confirmDeletion_playsDeleteConfirmPrompt)
{
    ON_CALL(*m_pMfrc, writeTag(_, _)).WillByDefault(Return(true));
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PP_LONGPRESS); 
    m_pOutputManager->runDispatcher(); // enter delete menu
    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::NO_ACTION); // get delete menu ready
    m_pOutputManager->setInputStates(InputManager::DELETE_TAG_MENU, UserInput::PLAY_PAUSE); // confirm deletion
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_CONFIRMED));
    m_pOutputManager->runDispatcher(); // complete delete menu
}

TEST_F(OutputManagerTest, deleteCardMenu_menuTimeout_resetsMenu)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PP_LONGPRESS); 
    m_pOutputManager->runDispatcher(); // enter delete menu
    for (uint16_t i = 0; i <= MENU_TIMEOUT_SECS; ++i)
    {
        m_pMenuTimer->timer_tick(); // make menuTimer elapse
    }
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::NO_ACTION);
}

TEST_F(OutputManagerTest, deleteMenu_userAbort_resetsMenu)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PP_LONGPRESS);
    m_pOutputManager->runDispatcher();
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PP_LONGPRESS);
    m_pOutputManager->runDispatcher();
}

// END OF DELETE MENU SPECIFIC TESTS --------------------------------------------

TEST_F(OutputManagerTest, dispatcher_noCardPPlongPress_help_playsPrompt)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::PP_LONGPRESS); 
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_HELP));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCardNext_playsNextTrack)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::NEXT_TRACK); 
    EXPECT_CALL(*m_pMp3, next_track());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardNext_playsNextTrack)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::NEXT_TRACK); 
    EXPECT_CALL(*m_pMp3, next_track());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardNext_readCalled)
{
    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::NEXT_TRACK); 
    EXPECT_CALL(*m_pMfrc, readCard(_, _));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCardPrev_playsPrevTrack)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::PREV_TRACK); 
    EXPECT_CALL(*m_pMp3, prev_track());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardPrev_playsPrevTrack)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::PREV_TRACK); 
    EXPECT_CALL(*m_pMp3, prev_track());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardPrev_readCalled)
{
    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::PREV_TRACK); 
    EXPECT_CALL(*m_pMfrc, readCard(_, _));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCardIncVol_increasesVolume)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::INC_VOLUME); 
    EXPECT_CALL(*m_pMp3, volume_up());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardIncVol_increasesVolume)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::INC_VOLUME); 
    EXPECT_CALL(*m_pMp3, volume_up());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardIncVol_readCalled)
{
    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::INC_VOLUME); 
    EXPECT_CALL(*m_pMfrc, readCard(_, _));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCardDecVol_decreasesVolume)
{
    m_pOutputManager->setInputStates(InputManager::NO_TAG, UserInput::DEC_VOLUME); 
    EXPECT_CALL(*m_pMp3, volume_down());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardVol_decreasesVolume)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_TAG, UserInput::DEC_VOLUME); 
    EXPECT_CALL(*m_pMp3, volume_down());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardVol_readCalled)
{
    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::DEC_VOLUME); 
    EXPECT_CALL(*m_pMfrc, readCard(_, _));
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_readSuccesful_noErrorGenerated)
{
    m_pMfrc->DelegateToFake(); // Delegates readCard() call to fake object
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(fakeBufferData[6]));

    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::NO_ACTION); 
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR_CARDREAD)).Times(0);
    m_pOutputManager->runDispatcher(); 
}

TEST_F(OutputManagerTest, dispatcher_read_noUpdateOfFolderInfoNecessary_cardNotUpdated)
{
    m_pMfrc->DelegateToFake(); // Delegates readCard() call to fake object
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(fakeBufferData[6]));

    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::NO_ACTION); 
    EXPECT_CALL(*m_pMfrc, writeTag(_, _)).Times(0); // no need to update card
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR_CARDREAD)).Times(0);
    m_pOutputManager->runDispatcher(); 
}

TEST_F(OutputManagerTest, dispatcher_read_updateOfFolderInfoNecessary_cardUpdated)
{
    byte updatedExpectedBuffer[16] = {};
    memcpy(updatedExpectedBuffer, fakeBufferData, sizeof(byte) * 16);
    updatedExpectedBuffer[6] = 9;
    m_pMfrc->DelegateToFake(); // Delegates readCard() call to fake object
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(9));

    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::NO_ACTION); 
    EXPECT_CALL(*m_pMfrc, writeTag(_, arrayByteCompare(
                                updatedExpectedBuffer,
                                MFRC522_interface::NFCTAG_MEMORY_TO_OCCUPY
                                ))).Times(1);
    m_pOutputManager->runDispatcher(); 
}

TEST_F(OutputManagerTest, dispatcher_read_folderDeletedOnCard_playsErrorPrompt)
{
    m_pMfrc->DelegateToFake(); // Delegates readCard() call to fake object
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(0));

    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_TAG, UserInput::NO_ACTION); 
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->runDispatcher(); 
}