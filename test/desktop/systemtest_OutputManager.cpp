#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <SimpleTimer.h>
#include <NfcTag.h>
#include <MFRC522_interface.h>

#include "mocks/unittest_NfcTag_mocks.h"
#include "mocks/unittest_Folder_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"
#include "mocks/unittest_Mp3PlayerControl_mocks.h"

#include <OutputManager.h>

using ::testing::_;
using ::testing::Assign;
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
        m_pNfcTag = new NfcTag(m_pMfrc);
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
        ON_CALL(*m_pPinCtrl, digital_read(DFMINI_STATUS_ISIDLE)).WillByDefault(Return(true));
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
    NfcTag *m_pNfcTag{nullptr};

    SimpleTimer *m_pMenuTimer{nullptr};
    NiceMock<Mock_Eeprom> *m_pEeprom{nullptr};

    OutputManager *m_pOutputManager{nullptr};
};

using ::testing::Return;

TEST_F(OutputManagerTest, setInputStates_dependenciesCalled)
{
    // Only makes sure that isPlaying() is checked within setInputStates
    EXPECT_CALL(*m_pMp3, is_playing()).Times(1);
    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::NO_ACTION);
}

TEST_F(OutputManagerTest, setInputStates_noCardNoAction_delMenuSetNotCalled)
{
    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pSysPwr, set_delMenu()).Times(0);
}

TEST_F(OutputManagerTest, setInputStates_noCardNoAction_linkMenuSetNotCalled)
{
    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pSysPwr, set_linkMenu()).Times(0);
}

TEST_F(OutputManagerTest, setInpuStates_linkCardMenu_linkMenuEntered)
{
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_UNKNOWNTAG));
    m_pOutputManager->setInputStates(InputManager::UNKNOWN_CARD_MENU, UserInput::NO_ACTION);
}

TEST_F(OutputManagerTest, setInputStates_linkCardMenu_lockInLinkMenu)
{
    m_pOutputManager->setInputStates(InputManager::UNKNOWN_CARD_MENU, UserInput::NO_ACTION);
    EXPECT_CALL(*m_pMp3, play_specific_file(1)); // Check if we are successfully locked in unknown card menu
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_CARD, UserInput::NEXT_TRACK); // although card state changes
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCardNoAction_errorHandler_noError)
{
    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::NO_ACTION);
    m_pOutputManager->runDispatcher();
    EXPECT_CALL(*m_pMp3, play_specific_file(_)).Times(0); // No Error Message sent
}

TEST_F(OutputManagerTest, dispatcher_NoAction_errorHandler_cardInputOutOfRange)
{
    m_pOutputManager->setInputStates(InputManager::NUMBER_OF_CARD_STATES, UserInput::NO_ACTION);
    // Assert
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCard_errorHandler_userActionOutOfRange)
{
    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::NUMBER_OF_REQUESTS);
    // Assert
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCard_errorHandler_userActionError)
{
    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::ERROR);
    // Assert
    EXPECT_CALL(*m_pMp3, play_specific_file(MSG_ERROR)).Times(1); // Error Message sent
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_noCardPlayPause_playCalled)
{
    m_pOutputManager->setInputStates(InputManager::NO_CARD, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_pause());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_activeKnownCardPlayPause_playCalled)
{
    m_pOutputManager->setInputStates(InputManager::ACTIVE_KNOWN_CARD, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_pause());
    m_pOutputManager->runDispatcher();
}

TEST_F(OutputManagerTest, dispatcher_newKnownCard_playCalled)
{
    m_pOutputManager->setInputStates(InputManager::NEW_KNOWN_CARD, UserInput::PLAY_PAUSE);
    EXPECT_CALL(*m_pMp3, play_pause());
    m_pOutputManager->runDispatcher();
}

MATCHER(validFolder, "") { return ((arg.folderId == 1) &&
                                   (arg.playMode == Folder::ALBUM) &&
                                   (arg.trackCount == 8)); }

TEST_F(OutputManagerTest, dispatcher_unknownCard_linkMenuComplete_writesInfoToCard)
{
    ON_CALL(*m_pMp3, get_trackCount_of_folder(_)).WillByDefault(Return(8));
    ON_CALL(*m_pMfrc, readCard(_, _)).WillByDefault(Return(true)); // Card Read method tested separately.
    EXPECT_CALL(*m_pMfrc, writeCard(_, _));                        // Make sure card info is written

    // Simulate navigation through linkMenu with folder1, playMode Album, 8 tracks
    m_pOutputManager->setInputStates(InputManager::UNKNOWN_CARD_MENU, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher(); // should set link menu to folder 1
    m_pOutputManager->setInputStates(InputManager::UNKNOWN_CARD_MENU, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher(); // should log folder 1
    m_pOutputManager->setInputStates(InputManager::UNKNOWN_CARD_MENU, UserInput::NEXT_TRACK);
    m_pOutputManager->runDispatcher(); // should set link menu to playmode ALBUM
    m_pOutputManager->setInputStates(InputManager::UNKNOWN_CARD_MENU, UserInput::PLAY_PAUSE);
    m_pOutputManager->runDispatcher(); // should log playmode and complete linkMenu
}

// EXPECT_CALL(*m_pDfMini, playFolderTrack(1, 1); // Make sure card read is performed.

