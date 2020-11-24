#ifndef MP3PLAYERCONTROL_IMPLEMENTATION_H
#define MP3PLAYERCONTROL_IMPLEMENTATION_H

#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"

#include "Mp3Control/Mp3Control_interface.h"
#include "Mp3Play/Mp3Play_interface.h"
#include "DfMiniMp3/DFMiniMp3_interface.h"
#include "Folder.h"
#include "../ErrorHandler/ErrorHandler_interface.h"

#include "../Config/Tonuino_config.h"
#include "../Config/Arduino_config.h"
#include "../Utilities/SimpleTimer.h"

// Implementation of Player controller interface.
// Uses Software Serial to communicate with DfMiniMp3 player.
// Dependencies use interfaces themselves for
// Hardware abstraction and better testabililty.
class Mp3Control : public Mp3Control_interface
{
public:
    Mp3Control(DfMiniMp3_interface *pDfMini,
               Mp3Play_interface *pPlayer,
               SimpleTimer *pLullabyeTimer,
               ErrorHandler_interface *pError);

    void loop() override;

    void play() override;
    void pause() override;
    void togglePlayPause() override;
    void nextTrack() override;
    void prevTrack() override;

    void volumeUp() override;
    void volumeDown() override;

private:
    void autoplay();

private:
    // Solution for constructor error found here: https://stackoverflow.com/questions/35762196/expected-a-type-specifier-error-when-creating-an-object-of-a-class-inside-anot
    //SoftwareSerial m_Mp3SwSerial{SoftwareSerial(DFMINI_RX, DFMINI_TX)}; // Does not work with m_Mp3SwSerial(DFMINI_RX, DFMINI_TX) because compiler interprets this as a class method call
    //DFMiniMp3<SoftwareSerial, Mp3Notify> m_dfMiniMp3{DFMiniMp3<SoftwareSerial, Mp3Notify>(m_Mp3SwSerial)};
    DfMiniMp3_interface *m_pDfMiniMp3{nullptr};
    Mp3Play_interface *m_pMp3Player{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    ErrorHandler_interface *m_pErrorHandler{nullptr};
};

#endif // MP3PLAYERCONTROL_IMPLEMENTATION_H