#ifndef PLAYERROR_H
#define PLAYERROR_H

#include "Tonuino_config.h"
#include "Mp3Control/Mp3Control_interface.h"

class PlayError
{
public:
    PlayError(Mp3Control_interface *pMp3Ctrl) : m_pMp3Control(pMp3Ctrl){};

    void promptHelpRequested();
    void promptTagReadError();
    void promptFolderError();

private:
    Mp3Control_interface *m_pMp3Control{nullptr};
};

#endif // PLAYERROR_H