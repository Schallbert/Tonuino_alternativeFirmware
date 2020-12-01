#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_Mp3Play_mocks.h"
#include "mocks/unittest_NfcControl_mocks.h"
#include "mocks/unittest_PowerManager_Mocks.h"

#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer.h"

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
        m_pMenuTimer = new SimpleTimer{};

        m_pVoiceMenu = new VoiceMenu(&m_Mp3PlayMock,
                                     &m_nfcControlMock,
                                     &m_powerManagerMock,
                                     m_pMenuTimer);
    }

    virtual void TearDown()
    {
        delete m_pVoiceMenu;
        delete m_pMenuTimer;
    }

protected:
    Menu_interface *m_deleteMenu{nullptr};

protected:
    NiceMock<Mock_Mp3Play> m_Mp3PlayMock{};
    NiceMock<Mock_NfcControl> m_nfcControlMock{};
    NiceMock<Mock_PowerManager> m_powerManagerMock{};
    SimpleTimer *m_pMenuTimer{nullptr};

    VoiceMenu *m_pVoiceMenu{nullptr};
};

TEST_F(VoiceMenuTest, noInit_isActive_returnsFalse)
{
    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, noInit_isTimerRunning_returnsFalse)
{
    ASSERT_FALSE(m_pMenuTimer->isRunning());
}

TEST_F(VoiceMenuTest, init_isTimerRunning_returnsTrue)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));
    m_pVoiceMenu->loop(); // entry conditions for Link menu met

    ASSERT_TRUE(m_pMenuTimer->isRunning());
}

TEST_F(VoiceMenuTest, timerElapes_isActive_returnFalse)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));
    m_pVoiceMenu->loop(); // entry conditions for Link menu met

    for (uint16_t i = 0; i <= MENU_TIMEOUT_SECS; ++i)
    {
        m_pMenuTimer->timerTick();
    }
    m_pVoiceMenu->loop();

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

// LinkMenu Specifics ---------------------------
TEST_F(VoiceMenuTest, initLinkMenu_isActive_returnsTrue)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));

    m_pVoiceMenu->loop(); // entry conditions for Link menu met

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuRunning_isActive_returnsTrue)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));
    m_pVoiceMenu->loop();                              // enters Menu: select folderId
    m_pVoiceMenu->setUserInput(UserInput::PLAY_PAUSE); // enters (invalid) folderId
    m_pVoiceMenu->loop();

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuComplete_isActive_returnsFalse)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));
    m_pVoiceMenu->loop();                              // enters Menu: select folderId
    m_pVoiceMenu->setUserInput(UserInput::PLAY_PAUSE); // provides (invalid) confirmation
    m_pVoiceMenu->loop();                              // selects folder Id
    m_pVoiceMenu->loop();                              // selects playmode and completes Menu

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, linkMenuCompleteAndCalledAgain_isActive_returnsTrue)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));
    m_pVoiceMenu->loop();                              // enters Menu: select folderId
    m_pVoiceMenu->setUserInput(UserInput::PLAY_PAUSE); // provides (invalid) confirmation
    m_pVoiceMenu->loop();                              // selects folder Id
    m_pVoiceMenu->loop();                              // selects playmode and completes Menu

    m_pVoiceMenu->loop(); // call again

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, initLinkMenu_loop_invokesPlayback)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));
    m_pVoiceMenu->setUserInput(UserInput::PLAY_PAUSE); // provides (invalid) confirmation
    m_pVoiceMenu->loop();

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(_));
    m_pVoiceMenu->loop();
}

TEST_F(VoiceMenuTest, linkMenu_linkPreview_isInvoked)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::NEW_UNKNOWN_TAG));

    m_pVoiceMenu->loop();                              // enter
    m_pVoiceMenu->setUserInput(UserInput::NEXT_TRACK); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_Mp3PlayMock, playFolder(_));
    m_pVoiceMenu->loop(); // should play preview for folder deletion
}

// Delete Menu specifics --------------------------
TEST_F(VoiceMenuTest, initDeleteMenu_isActive_returnsTrue)
{
    EXPECT_CALL(m_nfcControlMock, getTagPresence()).WillRepeatedly(Return(Nfc_interface::ACTIVE_KNOWN_TAG));
    m_pVoiceMenu->setUserInput(UserInput::PP_LONGPRESS);

    m_pVoiceMenu->loop(); // entry conditions for Delete menu met

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, deleteMenuRunning_isActive_returnsTrue)
{
    EXPECT_CALL(m_nfcControlMock, getTagPresence()).WillRepeatedly(Return(Nfc_interface::NEW_REGISTERED_TAG));
    EXPECT_CALL(m_nfcControlMock, getTagPresence()).WillOnce(Return(Nfc_interface::ACTIVE_KNOWN_TAG)).RetiresOnSaturation();

    m_pVoiceMenu->setUserInput(UserInput::PP_LONGPRESS);
    m_pVoiceMenu->loop(); // enter
    m_pVoiceMenu->setUserInput(UserInput::NO_ACTION);
    m_pVoiceMenu->loop(); // state: please confirm deletion

    ASSERT_TRUE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, deleteMenuComplete_isActive_returnsFalse)
{
    EXPECT_CALL(m_nfcControlMock, getTagPresence()).WillRepeatedly(Return(Nfc_interface::NEW_REGISTERED_TAG));
    EXPECT_CALL(m_nfcControlMock, getTagPresence()).WillOnce(Return(Nfc_interface::ACTIVE_KNOWN_TAG)).RetiresOnSaturation();

    m_pVoiceMenu->setUserInput(UserInput::PP_LONGPRESS);
    m_pVoiceMenu->loop();                              // enter
    m_pVoiceMenu->loop();                              // register known tag
    m_pVoiceMenu->setUserInput(UserInput::PLAY_PAUSE); // provides (invalid) confirmation
    m_pVoiceMenu->loop();                              // completes menu

    ASSERT_FALSE(m_pVoiceMenu->isActive());
}

TEST_F(VoiceMenuTest, initdeleteMenu_loop_invokesPlayback)
{
    ON_CALL(m_nfcControlMock, getTagPresence()).WillByDefault(Return(Nfc_interface::ACTIVE_KNOWN_TAG));
    m_pVoiceMenu->setUserInput(UserInput::PP_LONGPRESS);
    m_pVoiceMenu->loop();                             // enter
    m_pVoiceMenu->setUserInput(UserInput::NO_ACTION); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_Mp3PlayMock, playPrompt(_));
    m_pVoiceMenu->loop();
}

TEST_F(VoiceMenuTest, deleteMenu_deletePreview_isInvoked)
{
    ON_CALL(m_nfcControlMock, readFolderFromTag(_)).WillByDefault(Return(true));
    EXPECT_CALL(m_nfcControlMock, getTagPresence()).WillRepeatedly(Return(Nfc_interface::NEW_REGISTERED_TAG));
    EXPECT_CALL(m_nfcControlMock, getTagPresence()).WillOnce(Return(Nfc_interface::ACTIVE_KNOWN_TAG)).RetiresOnSaturation();

    m_pVoiceMenu->setUserInput(UserInput::PP_LONGPRESS);
    m_pVoiceMenu->loop();                             // enter
    m_pVoiceMenu->setUserInput(UserInput::NO_ACTION); // if it stays PP_LONGPRESS that will abort the menu

    EXPECT_CALL(m_Mp3PlayMock, playFolder(_));
    m_pVoiceMenu->loop(); // should play preview for folder deletion
}