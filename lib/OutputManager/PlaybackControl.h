#ifndef PLAYBACKCONTROL_H
#define PLAYBACKCONTROL_H

#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"
#include "../PowerManager/PowerManager_interface/PowerManager_interface.h"
#include "../Nfc/NfcControl/NfcControl.h"
#include "../UserInput/UserInput_interface/UserInput_interface.h"
#include "../Mp3/Mp3Control/Mp3Control_interface.h"
#include "../ErrorHandler/ErrorHandler_interface.h"

class PlaybackControl
{
public:
    PlaybackControl(Arduino_DIcontainer_interface *pArduHal,
                    PowerManager_interface *pPwrCtrl,
                    NfcControl_interface *pNfcCtrl,
                    Mp3Control_interface *pMp3Ctrl,
                    ErrorHandler_interface *pError) : m_pArduinoHal(pArduHal),
                                                      m_pSystemPower(pPwrCtrl),
                                                      m_pNfcControl(pNfcCtrl),
                                                      m_pMp3Ctrl(pMp3Ctrl),
                                                      m_pErrorHandler(pError){};

public:
    void setUserInput(UserInput::eUserRequest input);
    void loop();

private:
    void getTagState();

    // Dispatches cardState and userInput commands, calling downstream methods.
    void dispatchInputs();
    // Updates folder information on NFC card if necessary based on MP3 player read
    void updateFolderInformation();

    // ----- Wrapper methods to call target object's methods -----
    // Reads NFC tag and starts playback
    void read();
    // No action performed
    void none(){};
    // Play/pause
    void plPs() { m_pMp3Ctrl->play_pause(); };
    // next track
    void next() { m_pMp3Ctrl->next_track(); };
    // previous track
    void prev() { m_pMp3Ctrl->prev_track(); };
    // increase volume
    void incV() { m_pMp3Ctrl->volume_up(); };
    // decrease volume
    void decV() { m_pMp3Ctrl->volume_down(); };
    // play help prompt
    void help() { m_pErrorHandler->setHelpRequested(); };

private:
    typedef void (PlaybackControl::*dispatcher)(); // table of function pointers
    // members by dependency injection
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    PowerManager_interface *m_pSystemPower{nullptr};
    NfcControl_interface *m_pNfcControl{nullptr};
    Mp3Control_interface *m_pMp3Ctrl{nullptr};
    ErrorHandler_interface *m_pErrorHandler{nullptr};

    // Member objects
    Folder m_currentFolder{};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};
    UserInput::eUserRequest m_eUserInput{UserInput::NO_ACTION};
};

#endif // PLAYBACKCONTROL_H