#include "DeleteMenu_StateManager.h"

void DeleteMenu_StateManager::confirm()
{
    switch (menuState)
    {
    case NO_MENU:
        initMenu();
        break;
    case CONFIRM_DELETE:
        handleDeletionConfirmed();
        break;
    default:
        break;
    }
}

void DeleteMenu_StateManager::abort()
{
    //  tagToDeleteDetected = false;
    // menuState = NO_MENU;
}

void DeleteMenu_StateManager::setTagToDeleteDetected()
{
    menuState = CONFIRM_DELETE;
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