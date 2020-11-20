#include "Menu_factory.h"

Menu_interface *Menu_factory::getInstance(eMenuType menuType,
                                          NfcControl_interface *pNfcCtrl,
                                          Mp3Play_interface *pMp3Play,
                                          PowerManager_interface *pPowerMgr)
{
    Menu_interface *instance;
    switch (menuType)
    {
    case DELETE_MENU:
        instance = new DeleteMenu(pNfcCtrl, pMp3Play, pPowerMgr);
        break;
    case LINK_MENU:
        instance = new LinkMenu(pNfcCtrl, pMp3Play, pPowerMgr);
        break;
    }
    return instance;
}
