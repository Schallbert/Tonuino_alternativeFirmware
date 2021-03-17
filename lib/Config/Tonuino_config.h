#ifndef TONUINO_CONFIG_H
#define TONUINO_CONFIG_H

//DEBUG OUTPUT [just write "false" to disable debug output on serial]
#define DEBUGSERIAL false
#define DEBUGSERIAL_BAUDRATE 9600
#define DEBUG_MAXMESSAGELENGTH 33 // max number of characters in a message

// This is the selector of input variants.
// Change it to your preferred variant
// and make sure the pin selection below is
// according to your wishes.
#define three_buttons 420
#define one_encoder 425
#define USERINPUT_VARIANT three_buttons

//StatusLEDs interaction outputs
#define FLASHSLOWMS 1000
#define FLASHQUICKMS 400

//Folder specific data
#define MAXFOLDERCOUNT 99 // up to 255 supported by player
#define MAXTRACKSPERFOLDER 22 // up to 255 supported by player BUT limited due to RAM constraints!

#define VOLUME_MAX 15                                          // Absolute max: 30
#define VOLUME_MIN 1                                           // Absolute min: 0
#define VOLUME_INIT 5                                          // Start volume. Anything between 0 and 30.
#define DFMINI_EQ_SETTING DfMiniMp3_interface::DfMp3_Eq_Normal

//Timer1 interrupt task
#define TIMERONE_TASK_INTERVAL_USEC 1000 // 1ms
#define IDLE_TIMEOUT_SECS 60             // System will automatically switch off when paused
#define LULLABYE_TIMEOUT_ACTIVE true     // will shut the system down after a pre-defined time
#define LULLABYE_TIMEOUT_SECS 30 * 60    // default is x minutes (effective when track finished)
#define MENU_TIMEOUT_SECS 30             // menu will be automatically aborted if no input within this time

//MP3 defines
#define WAIT_DFMINI_READY 2      // sec
#define TIMEOUT_PROMPT_PLAYED 15 // sec

//NFC defines
#define NFCTAG_BYTES_TO_WRITE 6 //bytes

//Clickencoder / button defines
#define ENC_STEPSPERNOTCH 4             // resolution of encoder.
#define USERINPUTACTIVE_STATE LOW       // button's "pressed" state

#endif // TONUINO_CONFIG_H
