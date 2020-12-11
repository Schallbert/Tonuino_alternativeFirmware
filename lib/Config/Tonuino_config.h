#ifndef TONUINO_CONFIG_H
#define TONUINO_CONFIG_H

//DEBUG OUTPUT [just write "false" to disable debug output on serial]
#define DEBUGSERIAL true
#define DEBUGSERIAL_BAUDRATE 9600

//Tonuino mp3 prompt IDs -----------------------
#define MSG_ERROR_CARDWRITE 401
#define MSG_ERROR_CARDREAD 402
#define MSG_ERROR_FOLDER 403
#define MSG_STARTUP 500 
#define MSG_SHUTDOWN 501
#define MSG_SELECT_FOLDERID 300
#define MSG_SELECT_PLAYMODE 310
#define MSG_HELP 330
#define MSG_TAGCONFSUCCESS 400 
#define MSG_DELETETAG 700
#define MSG_CONFIRM_DELETION 710
#define MSG_ABORTED 802

//StatusLEDs interaction outputs
#define FLASHSLOWMS 500
#define FLASHQUICKMS 200

//Folder specific data
#define MAXFOLDERCOUNT 99 // up to 255 supported by player

#define VOLUME_MAX 22                                          // Absolute max: 30
#define VOLUME_MIN 2                                           // Absolute min: 0
#define VOLUME_INIT 7                                          // Start volume. Anything between 0 and 30.
#define DFMINI_EQ_SETTING DfMiniMp3_interface::DfMp3_Eq_Normal // TODO How to solve for enum belonging to foreign file?

//Timer1 interrupt task
#define TIMERONE_TASK_INTERVAL_USEC 1000 // 1ms
#define IDLE_TIMEOUT_SECS 20             // System will automatically switch off when paused
#define LULLABYE_TIMEOUT_ACTIVE true     // will shut the system down after a pre-defined time
#define LULLABYE_TIMEOUT_SECS 20 * 60    // default is 20 minutes (effective when track finished)
#define MENU_TIMEOUT_SECS 20             // menu will be automatically aborted if no input within this time

//MP3 defines
#define WAIT_DFMINI_READY 1      // 1 sec
#define TIMEOUT_PROMPT_PLAYED 10 // 10 sec

//NFC defines
#define NFCTAG_MEMORY_TO_OCCUPY 16 //bytes

//Clickencoder / button defines
#undef ENC_BUTTONINTERVAL
#undef ENC_DOUBLECLICKTIME
#undef ENC_HOLDTIME
#define ENC_BUTTONINTERVAL 20           // check button every x milliseconds, also debouce time
#define ENC_DOUBLECLICKTIME 400         // second click within x ms
#define ENC_HOLDTIME 1000               // report held button after x ms
#define ENC_LONGPRESSREPEATINTERVAL 400 // reports repeating-held every x ms
#define ENC_STEPSPERNOTCH 4             // resolution of encoder.
#define USERINPUTACTIVE_STATE LOW       // button's "pressed" state

#endif // TONUINO_CONFIG_H
