#include "Menu_factory.h"

Menu_interface *Menu_factory::getInstance(eMenuType menuType,
                                          NfcControl_interface *pNfcCtrl,
                                          PromptPlayer_interface *pPromptPlayer,
                                          PowerManager_interface *pPowerMgr)
{
    Menu_interface *instance;
    switch (menuType)
    {
    case DELETE_MENU:
        instance = new DeleteMenu(pNfcCtrl, pPromptPlayer, pPowerMgr);
        break;
    case LINK_MENU:
        instance = new LinkMenu(pNfcCtrl, pPromptPlayer, pPowerMgr);
        break;
    }
    return instance;
}
