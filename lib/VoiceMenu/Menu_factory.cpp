#include "LinkMenu/LinkMenu.h"
#include "DeleteMenu/DeleteMenu.h"

#include "Menu_factory.h"

Menu_interface *Menu_factory::getInstance(eMenuType menuType,
                                          NfcControl_interface *pNfcCtrl,
                                          Mp3Play_interface *pMp3Play,
                                          MessageHander_interface *pMessageHandler,
                                          PowerManager_interface *pPowerMgr)
{
    static Menu_interface *instance;
    switch (menuType)
    {
    case DELETE_MENU:
        instance = new DeleteMenu(pNfcCtrl, pMp3Play, pMessageHandler, pPowerMgr);
        break;
    case LINK_MENU:
        instance = new LinkMenu(pNfcCtrl, pMp3Play, pMessageHandler, pPowerMgr);
        break;
    }
    return instance;
}
