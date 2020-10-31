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
        deleteMenu = Menu_factory::getInstance(Menu_factory::DELETE_MENU);
    }

    virtual void TearDown()
    {
        delete deleteMenu;
    }

protected:
    Menu_interface *deleteMenu{nullptr};
};

// INIT() ------------------------------------------------------

TEST_F(DeleteMenuTest, noInit_noPromptSet)
{
    ASSERT_EQ((deleteMenu->getPrompt()).promptId, 0);
}

TEST_F(DeleteMenuTest, init_getPrompt_promptsDeleteTag)
{
    deleteMenu->confirm();

    Menu_interface::VoicePrompt result = deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_DELETETAG);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, init2x_getPrompt_promptsDeleteTag)
{
    deleteMenu->confirm();
    deleteMenu->confirm();

    Menu_interface::VoicePrompt result = deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_DELETETAG);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, placedTagToDelete_getPrompt_promptsWaitForConfirm)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);

    Menu_interface::VoicePrompt result = deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_CONFIRM_DELETION);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, placedTagToDelete_confirmDeletion_promptsTagConfigurationComplete)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);
    deleteMenu->confirm();

    Menu_interface::VoicePrompt result = deleteMenu->getPrompt();

    ASSERT_EQ(result.promptId, MSG_TAGCONFSUCCESS);
    ASSERT_EQ(result.allowSkip, true);
}

TEST_F(DeleteMenuTest, placedTagToDelete_confirmDeletion_menuComplete)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);
    deleteMenu->confirm();

    ASSERT_TRUE(deleteMenu->isComplete());
}

TEST_F(DeleteMenuTest, noInit_NotlockedInDeleteMenu)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NO_TAG;

    deleteMenu->setTagState(tagState);
    tagState = deleteMenu->getLockState();

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

TEST_F(DeleteMenuTest, entered_lockInDeleteMenu_locked)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);
    tagState = deleteMenu->getLockState();

    ASSERT_EQ(tagState, Nfc_interface::DELETE_TAG_MENU);
}

TEST_F(DeleteMenuTest, menuComplete_notLockedInDeleteMenu)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState); //requests lock state, should lock
    deleteMenu->confirm(); // completes Menu

    tagState = Nfc_interface::NO_TAG;
    deleteMenu->setTagState(tagState); //re-requests lock state, should not lock
    tagState = deleteMenu->getLockState();

    ASSERT_EQ(tagState, Nfc_interface::NO_TAG);
}

TEST_F(DeleteMenuTest, noInit_abort_noPromptSet)
{
    deleteMenu->abort();

    ASSERT_EQ((deleteMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(DeleteMenuTest, entered_abort_noPromptSet)
{
    deleteMenu->confirm();
    deleteMenu->abort();

    ASSERT_EQ((deleteMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(DeleteMenuTest, tagToDeleteDetected_abort_noPromptSet)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);
    deleteMenu->abort();

    ASSERT_EQ((deleteMenu->getPrompt()).promptId, MSG_ABORTED);
}

TEST_F(DeleteMenuTest, tagToDeleteDetected_abort_lockDisengaged)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);
    deleteMenu->abort();
    tagState = deleteMenu->getLockState();

    ASSERT_EQ(tagState, Nfc_interface::NEW_REGISTERED_TAG);
}

TEST_F(DeleteMenuTest, menuComplete_abort_reentry_lockEngaged)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);
    deleteMenu->confirm();
    deleteMenu->abort();

    deleteMenu->confirm();

    tagState = deleteMenu->getLockState();

    ASSERT_EQ(tagState, Nfc_interface::DELETE_TAG_MENU);
}

TEST_F(DeleteMenuTest, menuComplete_abort_reentry_promptsDeleteTag)
{
    Nfc_interface::eTagState tagState = Nfc_interface::NEW_REGISTERED_TAG;

    deleteMenu->confirm();
    deleteMenu->setTagState(tagState);
    deleteMenu->confirm();
    deleteMenu->abort();

    deleteMenu->confirm();

    ASSERT_EQ((deleteMenu->getPrompt()).promptId, MSG_DELETETAG);
}