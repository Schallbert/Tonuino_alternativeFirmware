#include "DeleteMenu_StateManager.h"

void DeleteMenu_StateManager::confirm()
{
    if (menuState == CONFIRM_DELETE)
    {
        handleDeletionConfirmed();
    }
    else
    {
        initMenu();
    }
}

void DeleteMenu_StateManager::abort()
{
    tagToDeleteDetected = false;
    menuState = ABORTED;
}

void DeleteMenu_StateManager::setTagToDeleteDetected()
{
    if (menuState == PLACE_TAG_TO_DELETE)
    {
        menuState = CONFIRM_DELETE;
    }
}

uint16_t DeleteMenu_StateManager::getMenuStateMessage()
{
    return static_cast<uint16_t>(menuState);
}


void DeleteMenu_StateManager::initMenu()
{
    menuState = PLACE_TAG_TO_DELETE;
}

void DeleteMenu_StateManager::handleDeletionConfirmed()
{
    menuState = COMPLETE;
}

bool DeleteMenu_StateManager::isActive()
{
    return ((menuState != NO_MENU &&
             menuState != ABORTED));
}

bool DeleteMenu_StateManager::isComplete()
{
    return (menuState == COMPLETE);
}

bool DeleteMenu_StateManager::isDeletionConfirmed()
{
    return (menuState == CONFIRM_DELETE);
}