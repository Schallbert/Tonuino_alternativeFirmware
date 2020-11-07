#include "Menu_implementation.h"

Menu_interface *Menu_factory::getInstance(eMenuType menuType,
                                          NfcControl_interface *pNfcCtrl,
                                          PromptPlayer_interface *pPromptPlayer)
{
    Menu_interface *instance;
    switch (menuType)
    {
    case DELETE_MENU:
        instance = new DeleteMenu(pNfcCtrl, pPromptPlayer);
        break;
    case LINK_MENU:
        instance = new LinkMenu(pNfcCtrl, pPromptPlayer);
        break;
    }
    return instance;
}
