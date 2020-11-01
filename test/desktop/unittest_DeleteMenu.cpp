#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Utilities/Menu_interface.h"
#include "../Utilities/DeleteMenu.h"
#include "../Utilities/Menu_implementation.h"

class DeleteMenuTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_deleteMenu = Menu_factory::getInstance(Menu_factory::DELETE_MENU);
    }

    virtual void TearDown()
    {
        delete m_deleteMenu;
    }

protected:
    Menu_interface *m_deleteMenu{nullptr};
};

// INIT() ------------------------------------------------------

TEST_F(DeleteMenuTest, noInit_noPromptSet)
{
    ASSERT_EQ((m_deleteMenu->getPrompt()).promptId, 0);
}

TEST_F(DeleteMenuTest, init_getPrompt_promptsDeleteTag)
{
    m_deleteMenu->confirm();

    Menu_interface::VoicePrompt result = m_deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_DELETETAG);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, init2x_getPrompt_promptsDeleteTag)
{
    m_deleteMenu->confirm();
    m_deleteMenu->confirm();

    Menu_interface::VoicePrompt result = m_deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_DELETETAG);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, placedTagToDelete_getPrompt_promptsWaitForConfirm)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);

    Menu_interface::VoicePrompt result = m_deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_CONFIRM_DELETION);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, placedTagToDelete_confirmDeletion_promptsTagConfigurationComplete)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);
    m_deleteMenu->confirm();

    Menu_interface::VoicePrompt result = m_deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_TAGCONFSUCCESS);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, noInit_NotlockedInDeleteMenu)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NO_TAG;

    m_deleteMenu->updateTagState(tagState);

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

TEST_F(DeleteMenuTest, entered_lockInDeleteMenu_locked)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);

    ASSERT_EQ(tagState, Nfc_interface::DELETE_TAG_MENU);
}

TEST_F(DeleteMenuTest, menuComplete_notLockedInDeleteMenu)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState); //requests lock state, should lock
    m_deleteMenu->confirm(); // completes Menu

    tagState = Nfc_interface::NO_TAG;
    m_deleteMenu->updateTagState(tagState); //re-requests lock state, should not lock

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

TEST_F(DeleteMenuTest, noInit_abort_noPromptSet)
{
    m_deleteMenu->abort();

    ASSERT_EQ((m_deleteMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(DeleteMenuTest, entered_abort_noPromptSet)
{
    m_deleteMenu->confirm();
    m_deleteMenu->abort();

    ASSERT_EQ((m_deleteMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(DeleteMenuTest, tagToDeleteDetected_abort_noPromptSet)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);
    m_deleteMenu->abort();

    ASSERT_EQ((m_deleteMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(DeleteMenuTest, lockedInMenu_abort_lockDisengaged)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    // Lock in Menu
    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);
    // This should disengage lock
    m_deleteMenu->abort();
    tagState = Nfc_interface::NO_TAG;
    m_deleteMenu->updateTagState(tagState);

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

TEST_F(DeleteMenuTest, menuComplete_abort_reentry_lockEngaged)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);
    m_deleteMenu->confirm();
    m_deleteMenu->abort();

    m_deleteMenu->confirm();

    ASSERT_EQ(tagState, Nfc_interface::DELETE_TAG_MENU);
}

TEST_F(DeleteMenuTest, menuComplete_abort_reentry_promptsDeleteTag)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);
    m_deleteMenu->confirm();
    m_deleteMenu->abort();

    m_deleteMenu->confirm();

    ASSERT_EQ((m_deleteMenu->getPrompt()).promptId, MSG_DELETETAG);
}

TEST_F(DeleteMenuTest, noInit_isComplete_returnsFalse)
{
    ASSERT_FALSE(m_deleteMenu->isComplete());
}

TEST_F(DeleteMenuTest, entered_isComplete_returnsFalse)
{
    m_deleteMenu->confirm();

    ASSERT_FALSE(m_deleteMenu->isComplete());
}

TEST_F(DeleteMenuTest, tagToDeleteDetected_isComplete_returnsFalse)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);

    ASSERT_FALSE(m_deleteMenu->isComplete());
}

TEST_F(DeleteMenuTest, placedTagToDelete_confirmDeletion_menuComplete)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    m_deleteMenu->confirm();
    m_deleteMenu->updateTagState(tagState);
    m_deleteMenu->confirm();

    ASSERT_TRUE(m_deleteMenu->isComplete());
}