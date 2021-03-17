#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_Mp3Prompt_mocks.h"
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
    virtual void SetUp()
    {
        m_pVoiceMenu = new VoiceMenu(m_Mp3PlayMock,
                                     m_Mp3PromptMock,
                                     m_NfcControlMock,
                                     m_MessageHandlerMock,
                                     m_PowerManagerMock,
                                     m_MenuTimer);
    }

    virtual void TearDown()
    {
        delete m_pVoiceMenu;
        m_pVoiceMenu = nullptr;
    }

    NiceMock<Mock_Mp3Play> m_Mp3PlayMock{};
    NiceMock<Mock_Mp3Prompt> m_Mp3PromptMock{};
    NiceMock<Mock_NfcControl> m_NfcControlMock{};
    NiceMock<Mock_MessageHandler> m_MessageHandlerMock{};
    NiceMock<Mock_PowerManager> m_PowerManagerMock{};
    SimpleTimer m_MenuTimer;
    VoiceMenu *m_pVoiceMenu{nullptr};
};

TEST_F(VoiceMenuTest, noInit_isActive_returnsFalse)
{
    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, noInit_isTimerRunning_returnsFalse)
{
    ASSERT_FALSE(m_MenuTimer.isRunning());
}

TEST_F(VoiceMenuTest, noInit_loop_isActiveReturnsFalse)
{
    m_pVoiceMenu->loop();
    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, init_isTimerRunning_returnsTrue)
{
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);
    m_pVoiceMenu->loop(); // entry conditions for Link menu met

    ASSERT_TRUE(m_MenuTimer.isRunning());
}

TEST_F(VoiceMenuTest, timerElapes_isActive_returnFalse)
{
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);
    m_pVoiceMenu->loop(); // entry conditions for Link menu met

    for (uint16_t i = 0; i <= MENU_TIMEOUT_SECS; ++i)
    {
        m_MenuTimer.timerTick();
    }
    m_pVoiceMenu->loop();

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

// LinkMenu Specifics ---------------------------
TEST_F(VoiceMenuTest, initLinkMenu_isActive_returnsTrue)
{
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);
    m_pVoiceMenu->loop(); // entry conditions for Link menu met

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuRunning_isActive_returnsTrue)
{
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);
    m_pVoiceMenu->loop();                           // enters Menu: select folderId
    m_pVoiceMenu->setUserInput(Message::INPUTPLPS); // enters (invalid) folderId
    m_pVoiceMenu->loop();

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuComplete_isActive_returnsFalse)
{
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);
    m_pVoiceMenu->loop();                           // enters Menu: select folderId
    m_pVoiceMenu->setUserInput(Message::INPUTPLPS); // provides (invalid) confirmation
    m_pVoiceMenu->loop();                           // selects folder Id
    m_pVoiceMenu->loop();                           // selects playmode and completes Menu

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuCompleteAndCalledAgain_isActive_returnsTrue)
{
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);
    m_pVoiceMenu->loop();                           // enters Menu: select folderId
    m_pVoiceMenu->setUserInput(Message::INPUTPLPS); // provides (invalid) confirmation
    m_pVoiceMenu->loop();                           // selects folder Id
    m_pVoiceMenu->loop();                           // selects playmode and completes Menu

    m_pVoiceMenu->loop(); // call again

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, initLinkMenu_loop_invokesPrompt)
{
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTPLPS); // provides (invalid) confirmation
    m_pVoiceMenu->loop();

    EXPECT_CALL(m_Mp3PromptMock, playPrompt(_));
    m_pVoiceMenu->loop();
}

TEST_F(VoiceMenuTest, linkMenu_linkPreview_isInvoked)
{
    ON_CALL(m_NfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
    m_pVoiceMenu->setTagState(Message::UNKNOWNTAG);

    m_pVoiceMenu->loop();                           // enter
    m_pVoiceMenu->setUserInput(Message::INPUTNEXT); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_Mp3PlayMock, playFolder(_));
    m_pVoiceMenu->loop(); // should play preview for folder deletion
}

// Delete Menu specifics --------------------------
TEST_F(VoiceMenuTest, initDeleteMenu_isActive_returnsTrue)
{
    m_pVoiceMenu->setTagState(Message::NOTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTPLPSDC);

    m_pVoiceMenu->loop(); // entry conditions for Delete menu met

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, deleteMenuRunning_isActive_returnsTrue)
{
    m_pVoiceMenu->setTagState(Message::NOTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTPLPSDC);
    m_pVoiceMenu->loop(); // enter
    m_pVoiceMenu->setTagState(Message::NEWKNOWNTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTNONE); // state: please confirm deletion

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, deleteMenuComplete_isActive_returnsFalse)
{
    m_pVoiceMenu->setTagState(Message::NOTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTPLPSDC);
    m_pVoiceMenu->loop(); // enter
    m_pVoiceMenu->setUserInput(Message::INPUTNONE);
    m_pVoiceMenu->setTagState(Message::NEWKNOWNTAG);
     m_pVoiceMenu->loop();
    m_pVoiceMenu->setUserInput(Message::INPUTPLPS); // provides (invalid) confirmation
    m_pVoiceMenu->loop();                           // completes menu

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, initdeleteMenu_loop_invokesPrompt)
{
    m_pVoiceMenu->setTagState(Message::NOTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTPLPSDC);

    EXPECT_CALL(m_Mp3PromptMock, playPrompt(_));
    m_pVoiceMenu->loop(); // enter
}

TEST_F(VoiceMenuTest, deleteMenu_deletePreview_isInvoked)
{
    ON_CALL(m_NfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));

    m_pVoiceMenu->setTagState(Message::NOTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTPLPSDC);
    m_pVoiceMenu->loop(); // enter
    m_pVoiceMenu->setTagState(Message::NEWKNOWNTAG);
    m_pVoiceMenu->setUserInput(Message::INPUTNONE); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_Mp3PlayMock, playFolder(_));
    m_pVoiceMenu->loop(); // should play preview for folder deletion
}
