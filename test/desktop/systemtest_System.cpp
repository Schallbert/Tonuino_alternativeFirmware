/*
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "SimpleTimer.h"
#include "../Nfc/NfcControl/NfcControl.h"
#include "../Nfc/Nfc/Nfc_interface.h"

#include "mocks/unittest_Nfc_mocks.h"
//#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_PowerManager_mocks.h"
#include "mocks/unittest_Mp3Control_mocks.h"

#include <PlaybackControl.h>

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
        m_pArduinoHal = new NiceMock<Mock_ArduinoDIcontainer>;
        m_pSerial = new NiceMock<Mock_serial>;
        m_pPins = new NiceMock<Mock_pinCtrl>;
        m_pEeprom = new NiceMock<Mock_eeprom>;
        m_pRandom = new NiceMock<Mock_random>;
        m_pArduinoHal->DelegateToMockSerial(m_pSerial);
        m_pArduinoHal->DelegateToMockPins(m_pPins);
        m_pArduinoHal->DelegateToMockEeprom(m_pEeprom);
        m_pArduinoHal->DelegateToMockRandom(m_pRandom);

        m_pMp3Control = new NiceMock<Mock_Mp3Control>;
        m_pSystemPower = new NiceMock<Mock_PowerManager>;
        m_pNfc = new NiceMock<Mock_Nfc>;
        m_pNfcControl = new NfcControl(m_pNfc, m_pArduinoHal->getSerial());

        m_pMenuTimer = new SimpleTimer{};

        m_pOutputManager = new PlaybackControl(m_pArduinoHal,
                                             m_pSystemPower,
                                             m_pNfcControl,
                                             m_pMp3Control,
                                             m_pMenuTimer);
    }

    virtual void TearDown()
    {
        delete m_pOutputManager;
        delete m_pMp3Control;
        delete m_pNfc;

        delete m_pSystemPower;
        delete m_pNfcControl;
        delete m_pArduinoHal;
        delete m_pSerial;
        delete m_pPins;
        delete m_pEeprom;
        delete m_pRandom;
        delete m_pMenuTimer;
    }

protected:
    NiceMock<Mock_ArduinoDIcontainer> *m_pArduinoHal;
    NiceMock<Mock_serial> *m_pSerial;
    NiceMock<Mock_pinCtrl> *m_pPins;
    NiceMock<Mock_eeprom> *m_pEeprom;
    NiceMock<Mock_random> *m_pRandom;
    NiceMock<Mock_Mp3Control> *m_pMp3Control;

    NiceMock<Mock_PowerManager> *m_pSystemPower{nullptr};
    NiceMock<Mock_Nfc> *m_pNfc{nullptr};
    NfcControl *m_pNfcControl{nullptr};

    SimpleTimer *m_pMenuTimer{nullptr};

    PlaybackControl *m_pOutputManager{nullptr};
};

// TESTS
TEST_F(OutputManagerTest, setTagState_noCardNoAction_deleteMenuNotCalled)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG);;
    EXPECT_CALL(*m_pSystemPower, setDeleteMenu()).Times(0);
}

TEST_F(OutputManagerTest, setTagState_noCardNoAction_linkMenuNotCalled)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pSystemPower, setLinkMenu()).Times(0);
}

TEST_F(OutputManagerTest, dispatcher_noCardNoAction_errorHandler_noError)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(_)).Times(0); // No Error Message sent
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_NoAction_errorHandler_cardInputOutOfRange)
{
    m_pOutputManager->setTagState(static_cast<NfcControl_interface::eTagState>(6)setUserINput(UserInput_interface::NO_ACTION);;
    // Assert
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCard_errorHandler_userActionOutOfRange)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG, UserInput_interface::NUMBER_OF_REQUESTS);
    // Assert
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCardPlayPause_playCalled)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_pause());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardPlayPause_playCalled)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_pause());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCard_readCalled)
{
    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pNfc, readTag(_, _));
    m_pOutputManager->dispatchInputs();
}

// LINK MENU SPECIFIC TESTS ---------------------------------------

TEST_F(OutputManagerTest, linkMenu_linkMenuEnterNotRepeated)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_SELECT_FOLDERID)).Times(0); // linkMenu init not called multiple times
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::NEXT_TRACK);
}

TEST_F(OutputManagerTest, linkMenu_entry)
{
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_SELECT_FOLDERID));
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
}

TEST_F(OutputManagerTest, linkMenu_lockInLinkMenu)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(1));                                              // Check if we are successfully locked in unknown card menu
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::NEXT_TRACK); // although card state changes
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_nextSelected_nextOptionSelected)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(1)).Times(1);
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_folderSelectionWorking)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MAXFOLDERCOUNT));
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PREV_TRACK);
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuFolderSelect_folderInvalidSelection0)
{
    EXPECT_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillOnce(Return(0));
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(_));
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->dispatchInputs();
}

MATCHER_P(folderIdIs, value, "") { return (arg->getFolderId() == value); }

TEST_F(OutputManagerTest, linkMenu_linkMenuFolderSelect_folderPreviewPlayed)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    EXPECT_CALL(*m_pMp3Control, play_folder(folderIdIs(1)));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuPlayModeSelect_playModeSelectionWorking)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PREV_TRACK);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_SELECT_PLAYMODE + static_cast<uint8_t>(Folder::ONELARGETRACK)));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuPlayModeSelect_playModeInvalidSelection0)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(_));
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_configureSuccessful)
{
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pNfc, writeTag(_, _)).WillByDefault(Return(true));
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(true));
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_TAGCONFSUCCESS));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_writesInfoToCard)
{
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(true));

    // Simulate navigation through linkMenu with folder1, playMode Album, 8 tracks
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to folder 1
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs(); // should log folder 1
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to playmode ALBUM
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pNfc, writeTag(_, _)); // Make sure card info is written
    m_pOutputManager->dispatchInputs();    // should log playmode and complete linkMenu
}

MATCHER_P3(folderOk, expFolderId, expPlayMode, expTrackCnt, "")
{
    return ((arg->getFolderId() == expFolderId) &&
            (arg->getPlayMode() == expPlayMode) &&
            (arg->getTrackCount() == expTrackCnt));
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_startsPlaybackWithCorrectSettings)
{
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pNfc, writeTag(_, _)).WillByDefault(Return(true));
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(true));

    // Simulate navigation through linkMenu with folder1, playMode Album, 8 tracks
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to folder 1
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs(); // should log folder 1
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to playmode ALBUM
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_folder(folderOk(1, Folder::ALBUM, 8)));
    m_pOutputManager->dispatchInputs(); // should log playmode and complete linkMenu
}

TEST_F(OutputManagerTest, linkMenu_menuTimeout_resetsMenu)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);; // enter linkMenu
    for (uint16_t i = 0; i <= MENU_TIMEOUT_SECS; ++i)
    {
        m_pMenuTimer->timerTick(); // make menuTimer elapse
    }
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);; // enter linkMenu
}

TEST_F(OutputManagerTest, linkMenu_userAbort_resetsMenu)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);; // enter linkMenu
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAG, UserInput_interface::PP_LONGPRESS); // abort linkMenu
    m_pOutputManager->dispatchInputs();
}

// END OF LINK MENU SPECIFIC TESTS ---------------------------------------

// DELETE MENU SPECIFIC TESTS --------------------------------------------
TEST_F(OutputManagerTest, deleteCardMenu_entry_playsDeletePrompt)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PP_LONGPRESS);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_DELETETAG));
    m_pOutputManager->dispatchInputs(); // enter delete menu
}

TEST_F(OutputManagerTest, deleteCardMenu_deleteNotReady_confirmDeletion_replaysDeletePrompt)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs(); // enter delete menu
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_DELETETAG));
    m_pOutputManager->dispatchInputs(); // delete menu cannot complete but stays active
}

TEST_F(OutputManagerTest, deleteCardMenu_deletionReady_confirmDeletion_playsDeleteConfirmPrompt)
{
    ON_CALL(*m_pNfc, writeTag(_, _)).WillByDefault(Return(true));
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs();                                                       // enter delete menu
    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;    // get delete menu ready
    m_pOutputManager->setTagState(Nfc_interface::DELETE_TAG_MENU, UserInput_interface::PLAY_PAUSE); // confirm deletion
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_CONFIRMED));
    m_pOutputManager->dispatchInputs(); // complete delete menu
}

TEST_F(OutputManagerTest, deleteCardMenu_menuTimeout_resetsMenu)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs(); // enter delete menu
    for (uint16_t i = 0; i <= MENU_TIMEOUT_SECS; ++i)
    {
        m_pMenuTimer->timerTick(); // make menuTimer elapse
    }
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
}

TEST_F(OutputManagerTest, deleteMenu_userAbort_resetsMenu)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs();
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs();
}

// END OF DELETE MENU SPECIFIC TESTS --------------------------------------------

TEST_F(OutputManagerTest, dispatcher_noCardPPlongPress_help_playsPrompt)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG, UserInput_interface::PP_LONGPRESS);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_HELP));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCardNext_playsNextTrack)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG, UserInput_interface::NEXT_TRACK);
    EXPECT_CALL(*m_pMp3Control, nextTrack());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardNext_playsNextTrack)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::NEXT_TRACK);
    EXPECT_CALL(*m_pMp3Control, nextTrack());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardNext_readCalled)
{
    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAG, UserInput_interface::NEXT_TRACK);
    EXPECT_CALL(*m_pNfc, readTag(_, _));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCardPrev_playsPrevTrack)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG, UserInput_interface::PREV_TRACK);
    EXPECT_CALL(*m_pMp3Control, prevTrack());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardPrev_playsPrevTrack)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::PREV_TRACK);
    EXPECT_CALL(*m_pMp3Control, prevTrack());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardPrev_readCalled)
{
    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAG, UserInput_interface::PREV_TRACK);
    EXPECT_CALL(*m_pNfc, readTag(_, _));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCardIncVol_increasesVolume)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG, UserInput_interface::INC_VOLUME);
    EXPECT_CALL(*m_pMp3Control, volumeUp());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardIncVol_increasesVolume)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::INC_VOLUME);
    EXPECT_CALL(*m_pMp3Control, volumeUp());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardIncVol_readCalled)
{
    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAG, UserInput_interface::INC_VOLUME);
    EXPECT_CALL(*m_pNfc, readTag(_, _));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCardDecVol_decreasesVolume)
{
    m_pOutputManager->setTagState(NfcControl_interface::NO_TAG, UserInput_interface::DEC_VOLUME);
    EXPECT_CALL(*m_pMp3Control, volumeDown());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardVol_decreasesVolume)
{
    m_pOutputManager->setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG, UserInput_interface::DEC_VOLUME);
    EXPECT_CALL(*m_pMp3Control, volumeDown());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCardVol_readCalled)
{
    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAG, UserInput_interface::DEC_VOLUME);
    EXPECT_CALL(*m_pNfc, readTag(_, _));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_readSuccesful_noErrorGenerated)
{
    m_pNfc->DelegateToFake(); // Delegates readTag() call to fake object
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(fakeBufferData[6]));

    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR_CARDREAD)).Times(0);
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_read_noUpdateOfFolderInfoNecessary_cardNotUpdated)
{
    m_pNfc->DelegateToFake(); // Delegates readTag() call to fake object
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(fakeBufferData[6]));

    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pNfc, writeTag(_, _)).Times(0); // no need to update card
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR_CARDREAD)).Times(0);
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_read_updateOfFolderInfoNecessary_cardUpdated)
{
    byte updatedExpectedBuffer[16] = {};
    memcpy(updatedExpectedBuffer, fakeBufferData, sizeof(byte) * 16);
    updatedExpectedBuffer[6] = 9;
    m_pNfc->DelegateToFake(); // Delegates readTag() call to fake object
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(9));

    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pNfc, writeTag(_, arrayByteCompare(
                                         updatedExpectedBuffer,
                                         NFCTAG_MEMORY_TO_OCCUPY)))
        .Times(1);
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_read_folderDeletedOnCard_playsErrorPrompt)
{
    m_pNfc->DelegateToFake(); // Delegates readTag() call to fake object
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(0));

    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->dispatchInputs();
}

*/