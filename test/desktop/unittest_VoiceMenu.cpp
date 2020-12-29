#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"

#include "Folder.h"
#include "Tonuino_config.h"
#include "VoiceMenu.h"
#include "SimpleTimer/SimpleTimer.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Sequence;

// TEST FIXTURE
class VoiceMenuTest : public ::testing::Test
{
protected:
    NiceMock<Mock_Mp3Play> m_Mp3PlayMock{};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_MessageHandler> m_messageHandlerMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
    SimpleTimer m_MenuTimer;
    VoiceMenu m_VoiceMenu{VoiceMenu(m_Mp3PlayMock,
                                     m_nfcControlMock,
                                     m_messageHandlerMock,
                                     m_powerManagerMock,
                                     m_MenuTimer)};
};

TEST_F(VoiceMenuTest, noInit_isActive_returnsFalse)
{
    ASSERT_FALSE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, noInit_isTimerRunning_returnsFalse)
{
    ASSERT_FALSE(m_MenuTimer.isRunning());
}

TEST_F(VoiceMenuTest, init_isTimerRunning_returnsTrue)
{
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);
    m_VoiceMenu.loop(); // entry conditions for Link menu met

    ASSERT_TRUE(m_MenuTimer.isRunning());
}

TEST_F(VoiceMenuTest, timerElapes_isActive_returnFalse)
{
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);
    m_VoiceMenu.loop(); // entry conditions for Link menu met

    for (uint16_t i = 0; i <= MENU_TIMEOUT_SECS; ++i)
    {
        m_MenuTimer.timerTick();
    }
    m_VoiceMenu.loop();

    ASSERT_FALSE(m_VoiceMenu.isActive());
}

// LinkMenu Specifics ---------------------------
TEST_F(VoiceMenuTest, initLinkMenu_isActive_returnsTrue)
{
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);

    m_VoiceMenu.loop(); // entry conditions for Link menu met

    ASSERT_TRUE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, linkMenuRunning_isActive_returnsTrue)
{
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);
    m_VoiceMenu.loop();                              // enters Menu: select folderId
    m_VoiceMenu.setUserInput(UserInput_interface::PLAY_PAUSE); // enters (invalid) folderId
    m_VoiceMenu.loop();

    ASSERT_TRUE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, linkMenuComplete_isActive_returnsFalse)
{
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);
    m_VoiceMenu.loop();                              // enters Menu: select folderId
    m_VoiceMenu.setUserInput(UserInput_interface::PLAY_PAUSE); // provides (invalid) confirmation
    m_VoiceMenu.loop();                              // selects folder Id
    m_VoiceMenu.loop();                              // selects playmode and completes Menu

    ASSERT_FALSE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, linkMenuCompleteAndCalledAgain_isActive_returnsTrue)
{
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);
    m_VoiceMenu.loop();                              // enters Menu: select folderId
    m_VoiceMenu.setUserInput(UserInput_interface::PLAY_PAUSE); // provides (invalid) confirmation
    m_VoiceMenu.loop();                              // selects folder Id
    m_VoiceMenu.loop();                              // selects playmode and completes Menu

    m_VoiceMenu.loop(); // call again

    ASSERT_TRUE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, initLinkMenu_loop_invokesPrompt)
{
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::PLAY_PAUSE); // provides (invalid) confirmation
    m_VoiceMenu.loop();

    EXPECT_CALL(m_messageHandlerMock, promptMessage(_));
    m_VoiceMenu.loop();
}

TEST_F(VoiceMenuTest, linkMenu_linkPreview_isInvoked)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_UNKNOWN_TAG);

    m_VoiceMenu.loop();                              // enter
    m_VoiceMenu.setUserInput(UserInput_interface::NEXT_TRACK); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_Mp3PlayMock, playFolder(_));
    m_VoiceMenu.loop(); // should play preview for folder deletion
}

// Delete Menu specifics --------------------------
TEST_F(VoiceMenuTest, initDeleteMenu_isActive_returnsTrue)
{
    m_VoiceMenu.setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::PP_LONGPRESS);

    m_VoiceMenu.loop(); // entry conditions for Delete menu met

    ASSERT_TRUE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, deleteMenuRunning_isActive_returnsTrue)
{
    m_VoiceMenu.setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::PP_LONGPRESS);
    m_VoiceMenu.loop(); // enter
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_REGISTERED_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::NO_ACTION);
    m_VoiceMenu.loop(); // state: please confirm deletion

    ASSERT_TRUE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, deleteMenuComplete_isActive_returnsFalse)
{
    m_VoiceMenu.setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::PP_LONGPRESS);
    m_VoiceMenu.loop();                              // enter
    m_VoiceMenu.setUserInput(UserInput_interface::NO_ACTION);
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_REGISTERED_TAG);
    m_VoiceMenu.loop();                              // register known tag
    m_VoiceMenu.setUserInput(UserInput_interface::PLAY_PAUSE); // provides (invalid) confirmation
    m_VoiceMenu.loop();                              // completes menu

    ASSERT_FALSE(m_VoiceMenu.isActive());
}

TEST_F(VoiceMenuTest, initdeleteMenu_loop_invokesPrompt)
{
    m_VoiceMenu.setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::PP_LONGPRESS);
    m_VoiceMenu.loop();                             // enter
    m_VoiceMenu.setUserInput(UserInput_interface::NO_ACTION); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_messageHandlerMock, promptMessage(_));
    m_VoiceMenu.loop();
}

TEST_F(VoiceMenuTest, deleteMenu_deletePreview_isInvoked)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
   
    m_VoiceMenu.setTagState(NfcControl_interface::ACTIVE_KNOWN_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::PP_LONGPRESS);
    m_VoiceMenu.loop();                             // enter
    m_VoiceMenu.setTagState(NfcControl_interface::NEW_REGISTERED_TAG);
    m_VoiceMenu.setUserInput(UserInput_interface::NO_ACTION); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_Mp3PlayMock, playFolder(_));
    m_VoiceMenu.loop(); // should play preview for folder deletion
}