#include "LinkMenu/LinkMenu.h"
#include "DeleteMenu/DeleteMenu.h"

#include "Menu_factory.h"

Menu_interface *Menu_factory::getInstance(eMenuType menuType,
                                          NfcControl_interface &rNfcCtrl,
                                          Mp3Play_interface &rMp3Play,
                                          MessageHander_interface &rMessageHandler,
                                          PowerManager_interface &rPowerMgr)
{
    static Menu_interface *instance;
    switch (menuType)
    {
    case DELETE_MENU:
        instance = new DeleteMenu(rNfcCtrl, rMp3Play, rMessageHandler, rPowerMgr); // TODO
        break;
    case LINK_MENU:
        instance = new LinkMenu(rNfcCtrl, rMp3Play, rMessageHandler, rPowerMgr);
        break;
    }
    return instance;
}
