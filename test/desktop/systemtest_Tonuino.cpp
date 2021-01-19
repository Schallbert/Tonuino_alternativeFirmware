
// NFC
#include "../Nfc/Nfc/Nfc_implementation.h"
#include "../Nfc/NfcControl/NfcControl.h"
// MP3
#include "../Mp3/Mp3Play/Mp3Play_implementation.h"
#include "../Mp3/Mp3Control/Mp3Control_implementation.h"
#include "../Mp3/Mp3Prompt/Mp3Prompt_implementation.h"
// MISC
#include "../PowerManager/PowerManager_implementation.h"
#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

#include "../Tonuino/Tonuino.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_UserInput_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_Mp3Prompt_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_Nfc_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class SystemTestPlayback : public ::testing::Test
{
protected:
    // Mock all dependencies to Hardware ONLY. Apart from that,
    // This is a "copy" the Loader module.
    virtual void SetUp()
    {
        // relay Arduino Hal Calls to respective specialized mocks
        m_ArduinoHalMock.DelegateToMockSerial(&m_SerialMock);
        m_ArduinoHalMock.DelegateToMockPins(&m_PinsMock);
        m_ArduinoHalMock.DelegateToMockEeprom(&m_EepromMock);
        m_ArduinoHalMock.DelegateToMockRandom(&m_RandomMock);

        // Use production code implementation for business logic system tests
        m_pPwrCtrl = new PowerManager(m_ArduinoHalMock.getPins(), m_IdleTimer);

        m_pMp3Play = new Mp3Play_implementation(m_ArduinoHalMock, m_DfMiniMp3Mock, m_Mp3PromptMock, m_LullabyeTimer, m_MessageHandlerMock);
        m_pMp3Control = new Mp3Control(m_DfMiniMp3Mock, *m_pMp3Play, m_Mp3PromptMock, *m_pPwrCtrl, m_MessageHandlerMock);

        m_pNfc = new Nfc_implementation(m_Mfrc522Mock, m_MessageHandlerMock);
        m_pNfcControl = new NfcControl(*m_pNfc, m_MessageHandlerMock);

        m_pVoiceMenu = new VoiceMenu(*m_pMp3Play, m_Mp3PromptMock, *m_pNfcControl, m_MessageHandlerMock, *m_pPwrCtrl, m_MenuTimer);
        m_pTonuino = new Tonuino(&m_UserInputMock, *m_pNfcControl, *m_pMp3Control, *m_pVoiceMenu);
    }

    virtual void TearDown()
    {
        delete m_pPwrCtrl;
        delete m_pMp3Play;
        delete m_pMp3Control;
        delete m_pNfc;
        delete m_pNfcControl;
        delete m_pVoiceMenu;
        delete m_pTonuino;

        m_pPwrCtrl = nullptr;
        m_pMp3Play = nullptr;
        m_pMp3Control = nullptr;
        m_pNfc = nullptr;
        m_pNfcControl = nullptr;
        m_pVoiceMenu = nullptr;
        m_pTonuino = nullptr;
    }

protected:
    // Mocks used
    NiceMock<Mock_ArduinoDIcontainer> m_ArduinoHalMock{};
    NiceMock<Mock_serial> m_SerialMock{};
    NiceMock<Mock_pinCtrl> m_PinsMock{};
    NiceMock<Mock_eeprom> m_EepromMock{};
    NiceMock<Mock_random> m_RandomMock{};

    NiceMock<Mock_UserInput> m_UserInputMock{};
    NiceMock<Mock_DfMiniMp3> m_DfMiniMp3Mock{};
    NiceMock<Mock_MessageHandler> m_MessageHandlerMock{};
    NiceMock<Mock_Mp3Prompt> m_Mp3PromptMock{};
    NiceMock<Mock_MFRC522> m_Mfrc522Mock{};

    // Real implementations without dependencies used
    SimpleTimer m_MenuTimer{SimpleTimer()};
    SimpleTimer m_LullabyeTimer{SimpleTimer()};
    SimpleTimer m_IdleTimer{SimpleTimer()};
    SimpleTimer m_DfMiniPromptTimer{SimpleTimer()};

    // Real implementations that depend on initialized objects (realized as pointers)
    PowerManager *m_pPwrCtrl{nullptr};

    Mp3Play_implementation *m_pMp3Play{nullptr};
    Mp3Control *m_pMp3Control{nullptr};

    Nfc_implementation *m_pNfc{nullptr};
    NfcControl *m_pNfcControl{nullptr};

    VoiceMenu *m_pVoiceMenu{nullptr};
    Tonuino *m_pTonuino{nullptr};
};

/*class SystemTestVoiceMenu : public SystemTestPlayback
{
};*/

// TESTS

TEST_F(SystemTestPlayback, initWorksFine)
{
    EXPECT_CALL(m_UserInputMock, getUserRequest()).WillOnce(Return(UserInput_interface::NO_ACTION));
    m_pTonuino->loop(); 
}

/*
TEST_F(SystemTestPlayback, setTagState_noCardNoAction_deleteMenuNotCalled)
{
    m_pOutputManager->setTagState(Message::NOTAG);
    ;
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
    m_pOutputManager->setTagState(static_cast<Message::eMessageContent>(6)setUserINput(UserInput_interface::NO_ACTION);;
    // Assert
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCard_errorHandler_userActionOutOfRange)
{
    m_pOutputManager->setTagState(Message::NOTAG, UserInput_interface::NUMBER_OF_REQUESTS);
    // Assert
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCardPlayPause_playCalled)
{
    m_pOutputManager->setTagState(Message::NOTAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_pause());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardPlayPause_playCalled)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PLAY_PAUSE);
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
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::NEXT_TRACK);
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
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::NEXT_TRACK); // although card state changes
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_nextSelected_nextOptionSelected)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(1)).Times(1);
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_folderSelectionWorking)
{
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MAXFOLDERCOUNT));
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PREV_TRACK);
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuFolderSelect_folderInvalidSelection0)
{
    EXPECT_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillOnce(Return(0));
    m_pOutputManager->setTagState(NfcControl_interface::NEW_UNKNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(_));
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->dispatchInputs();
}

MATCHER_P(folderIdIs, value, "") { return (arg->getFolderId() == value); }

TEST_F(OutputManagerTest, linkMenu_linkMenuFolderSelect_folderPreviewPlayed)
{
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    EXPECT_CALL(*m_pMp3Control, play_folder(folderIdIs(1)));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuPlayModeSelect_playModeSelectionWorking)
{
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PREV_TRACK);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_SELECT_PLAYMODE + static_cast<uint8_t>(Folder::ONELARGETRACK)));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuPlayModeSelect_playModeInvalidSelection0)
{
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(_));
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ERROR_FOLDER));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_configureSuccessful)
{
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pNfc, writeTag(_, _)).WillByDefault(Return(true));
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(true));
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs();
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_TAGCONFSUCCESS));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, linkMenu_linkMenuComplete_writesInfoToCard)
{
    ON_CALL(*m_pMp3Control, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pNfc, readTag(_, _)).WillByDefault(Return(true));

    // Simulate navigation through linkMenu with folder1, playMode Album, 8 tracks
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to folder 1
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs(); // should log folder 1
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to playmode ALBUM
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pNfc, writeTag(_, _)); // Make sure card info is written
    m_pOutputManager->dispatchInputs();    // should log playmode and complete linkMenu
}

MATCHER_P3(FOLDEROK, expFolderId, expPlayMode, expTrackCnt, "")
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
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to folder 1
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    m_pOutputManager->dispatchInputs(); // should log folder 1
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::NEXT_TRACK);
    m_pOutputManager->dispatchInputs(); // should set link menu to playmode ALBUM
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_folder(FOLDEROK(1, Folder::ALBUM, 8)));
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
    m_pOutputManager->setTagState(Message::UNKNOWNTAG, UserInput_interface::PP_LONGPRESS); // abort linkMenu
    m_pOutputManager->dispatchInputs();
}

// END OF LINK MENU SPECIFIC TESTS ---------------------------------------

// DELETE MENU SPECIFIC TESTS --------------------------------------------
TEST_F(OutputManagerTest, deleteCardMenu_entry_playsDeletePrompt)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PP_LONGPRESS);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_DELETETAG));
    m_pOutputManager->dispatchInputs(); // enter delete menu
}

TEST_F(OutputManagerTest, deleteCardMenu_deleteNotReady_confirmDeletion_replaysDeletePrompt)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs(); // enter delete menu
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_DELETETAG));
    m_pOutputManager->dispatchInputs(); // delete menu cannot complete but stays active
}

TEST_F(OutputManagerTest, deleteCardMenu_deletionReady_confirmDeletion_playsDeleteConfirmPrompt)
{
    ON_CALL(*m_pNfc, writeTag(_, _)).WillByDefault(Return(true));
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs();                                                       // enter delete menu
    m_pOutputManager->setTagState(Nfc_interface::NEW_KNOWN_TAGsetUserINput(UserInput_interface::NO_ACTION);;    // get delete menu ready
    m_pOutputManager->setTagState(Nfc_interface::DELETE_TAG_MENU, UserInput_interface::PLAY_PAUSE); // confirm deletion
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_CONFIRMED));
    m_pOutputManager->dispatchInputs(); // complete delete menu
}

TEST_F(OutputManagerTest, deleteCardMenu_menuTimeout_resetsMenu)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PP_LONGPRESS);
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
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs();
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_ABORTED));
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PP_LONGPRESS);
    m_pOutputManager->dispatchInputs();
}

// END OF DELETE MENU SPECIFIC TESTS --------------------------------------------

TEST_F(OutputManagerTest, dispatcher_noCardPPlongPress_help_playsPrompt)
{
    m_pOutputManager->setTagState(Message::NOTAG, UserInput_interface::PP_LONGPRESS);
    EXPECT_CALL(*m_pMp3Control, play_specific_file(MSG_HELP));
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_noCardNext_playsNextTrack)
{
    m_pOutputManager->setTagState(Message::NOTAG, UserInput_interface::NEXT_TRACK);
    EXPECT_CALL(*m_pMp3Control, nextTrack());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardNext_playsNextTrack)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::NEXT_TRACK);
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
    m_pOutputManager->setTagState(Message::NOTAG, UserInput_interface::PREV_TRACK);
    EXPECT_CALL(*m_pMp3Control, prevTrack());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardPrev_playsPrevTrack)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::PREV_TRACK);
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
    m_pOutputManager->setTagState(Message::NOTAG, UserInput_interface::INC_VOLUME);
    EXPECT_CALL(*m_pMp3Control, volumeUp());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardIncVol_increasesVolume)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::INC_VOLUME);
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
    m_pOutputManager->setTagState(Message::NOTAG, UserInput_interface::DEC_VOLUME);
    EXPECT_CALL(*m_pMp3Control, volumeDown());
    m_pOutputManager->dispatchInputs();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardVol_decreasesVolume)
{
    m_pOutputManager->setTagState(Message::ACTIVETAG, UserInput_interface::DEC_VOLUME);
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