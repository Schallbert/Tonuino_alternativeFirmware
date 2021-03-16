#include "LinkMenu/LinkMenu.h"
#include "DeleteMenu/DeleteMenu.h"

#include "Menu_factory.h"

Menu_factory::~Menu_factory()
{
    removeInstance();
}

Menu_interface *Menu_factory::getInstance(eMenuType menuType,
                                          NfcControl_interface &rNfcCtrl,
                                          Mp3Play_interface &rMp3Play,
                                          Mp3Prompt_interface &rMp3Prompt,
                                          MessageHander_interface &rMessageHandler,
                                          PowerManager_interface &rPowerMgr)
{
    removeInstance();
    switch (menuType)
    {
    case DELETE_MENU:
        m_instance = new DeleteMenu(rNfcCtrl, rMp3Play, rMp3Prompt, rMessageHandler, rPowerMgr);
        break;
    case LINK_MENU:
        m_instance = new LinkMenu(rNfcCtrl, rMp3Play, rMp3Prompt, rMessageHandler, rPowerMgr);
        break;
    default:
        removeInstance();
        break;
    }

    return m_instance;
}

void Menu_factory::removeInstance()
{
    delete m_instance;
    m_instance = nullptr;
}
