#ifndef PLAYERROR_H
#define PLAYERROR_H

#include "Tonuino_config.h"
#include "Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"

class PlayError
{
public:
    PlayError(Mp3PlayerControl_interface *pMp3Ctrl) : m_pMp3Control(pMp3Ctrl){};

    void promptHelpRequested();
    void promptTagReadError();
    void promptFolderError();

private:
    Mp3PlayerControl_interface *m_pMp3Control{nullptr};
};

#endif // PLAYERROR_H