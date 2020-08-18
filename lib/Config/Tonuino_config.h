#ifndef TONUINO_CONFIG_H
#define TONUINO_CONFIG_H

#include <DFMiniMp3_interface.h>

//DEBUG OUTPUT [just write "false" to disable debug output on serial]
#define DEBUGSERIAL false

//Tonuino mp3 prompt IDs -----------------------
#define MSG_DELETETAG 800
#define MSG_ABORTEED 802
#define MSG_ERROR 401
#define MSG_ERROR_CARDREAD 402
#define MSG_ERROR_FOLDER 403
#define MSG_TAGCONFSUCCESS 400
#define MSG_UNKNOWNTAG 300 // TODO: rename o "select folderId?"
#define MSG_TAGLINKED 310 // TODO: Rename to "select playMode"
#define MSG_SELECTFILE 320
#define MSG_HELP 330
#define MSG_CONFIRMED 340

//StatusLEDs interaction outputs
#define FLASHSLOWMS 500
#define FLASHQUICKMS 200

//Folder specific data
#define MAXFOLDERCOUNT 99 // up to 255 supported by player

#define VOLUME_MAX 22 // Absolute max: 30
#define VOLUME_MIN 2 // Absolute min: 0
#define VOLUME_INIT 7 // Start volume. Anything between 0 and 30.
#define DFMINI_EQ_SETTING DfMiniMp3_interface::DfMp3_Eq_Normal // TODO How to solve for enum belonging to foreign file?

//Timer1 interrupt task
#define TIMERONE_TASK_INTERVAL_USEC 1000 // 1ms
#define IDLE_TIMEOUT_SECS 60 // System will automatically switch off when paused
#define LULLABYE_TIMEOUT_SECS 20*60 // default is 20 minutes (effective when track finished)
#define MENU_TIMEOUT_SECS 20 // menu will be automatically aborted if no input within this time

//MP3 defines
#define WAIT_DFMINI_READY 1000 // 1 sec
#define TIMEOUT_PROMPT_PLAYED 10000 // 10 sec

#endif // TONUINO_CONFIG_H
