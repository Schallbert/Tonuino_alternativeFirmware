#ifndef ARDUINO_CONFIG_H
#define ARDUINO_CONFIG_H

// CPU clock Speed
//#define F_CPU 8000000UL
#define MS_TO_S 1000

//Pin defines ------------------------
#define PINANALOG_RNDMGEN 17 //ADC1 24 A1
// MFRC522
#define RST_PIN 9 // PB1 15
#define SS_PIN 10 // PB2 16
//MP3
#define DFMINI_PIN_ISIDLE 4 // PD4 6
#define DFMINI_RX 2   // PD2 4
#define DFMINI_TX 3   // PD3 5
//Serial
// RX 0  PD0 2
// TX 1  PD1 3
//HMI inputs

//User Input variants
#define THREE_BUTTONS 0
#define ONE_ENCODER 1
#define FIVE_BUTTONS 2

// This is the selector of input variants.
// Change it to your preferred variant
// and make sure the pin selection below is
// according to your wishes.
#define USERINPUT_VARIANT THREE_BUTTONS
// USERINPUT VARIANT : CLICK_ENCODER
  #define ENCA 18 // PC5 27
  #define ENCB 17 // PC4 26
  #define ENCSW 19 // PC6 28
// USERINPUT VARIANT : THREE_BUTTONS
#define PINPLPS 18 // PC5 27
#define PINPREV 17 // PC4 26
#define PINNEXT 19 // PC6 28
// USERINPUT VARIANT : FIVE BUTTONS     NOT IMPLEMENTED ! ! !
 //#define PINDECVOL 16
 //#define PINPLPS 18 // PC5 27
 //#define PINPREV 17 // PC4 26
 //#define PINNEXT 19 // PC6 28
 //#define PININCVOL 20

//KeepAlive
#define KEEPALIVE_PIN 16 //PB6 9 A0
#define KEEPALIVE_ACTIVE_STATE LOW
//Feedback LED
#define LED_PIN 13 //for test purposes only, otherwise 16 //PC0 25
#define LED_ACTIVE_STATE HIGH

#endif // ARDUINO_CONFIG_H