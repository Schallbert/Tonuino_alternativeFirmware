//#define F_CPU 8000000UL

//DEBUG OUTPUT [just comment out this define to disable all serial outputs]
#define DEBUGSERIAL false

//mp3 Messages -----------------------
#define MSG_DELETETAG 800
#define MSG_ABORTEED 802
#define MSG_ERROR 401
#define MSG_TAGCONFSUCCESS 400
#define MSG_UNKNOWNTAG 300
#define MSG_TAGLINKED 310
#define MSG_SELECTFILE 320
#define MSG_HELP 330

//Pin defines ------------------------
#define PINANALOG_RNDMGEN A1 //ADC1 24
// MFRC522
#define RST_PIN 9 // PB1 15
#define SS_PIN 10 // PB2 16
//MP3
#define DFMINI_BUSY 4 // PD4 6
#define DFMINI_RX 2   // PD2 4
#define DFMINI_TX 3   // PD3 5
//Serial
// RX 0  PD0 2
// TX 1  PD1 3
//HMI inputs
//   #define ENCA 18 // PC5 27
//   #define ENCB 17 // PC4 26
//   #define ENCSW 19 // PC6 28
#define PINPLPS 18 // PC5 27
#define PINPREV 17 // PC4 26
#define PINNEXT 19 // PC6 28
//KeepAlive
#define KEEPALIVE A0 //PB6 9
//Feedback LED
#define LED_PIN 16 //PC0 25

//KeepAlive interaction outputs
#define MAXIDLE 0x400
#define LEDFLASHINTVL 0x20

//Timer1 interrupt task
#define TIMERONE_TASK_INTERVAL_USEC 1000 //1ms

// Includes -------------------------
#include <Arduino.h>
#include <avr/sleep.h>
#include <EEPROM.h>
#include <MFRC522.h>
#include <DFMiniMp3.h>
#include <EEPROM.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <TimerOne.h>


// LPREUSSE personal includes -------
#include <StatusLed.h>
#include <interface_KeepAlive.h>
#include <interface_UserInput.h>

// ____ TONUINO LOGIC SPECIFICS_____
// Play modes
enum playMode
{
  ONELARGETRACK = 1, //So-called Hörspielmodus TODO: RE-RECORD VOICE OUTPUT
  ALBUM = 2,
  SAVEPROGRESS = 5, //TODO: RE-RECORD VOICE OUTPUT
};

//Folders
struct folderSettings
{
  uint8_t folder;
  uint8_t mode;
  uint8_t special;
  uint8_t special2;
};

// this object stores nfc tag data
struct nfcTagObject
{
  uint32_t cookie;
  uint8_t version;
  folderSettings nfcFolderSettings;
};

// admin settings stored in eeprom
struct adminSettings
{
  uint32_t cookie;
  byte version;
  uint8_t maxVolume;
  uint8_t minVolume;
  uint8_t initVolume;
  uint8_t eq;
  bool locked;
  bool invertVolumeButtons;
  folderSettings shortCuts[4];
};

//function prototypes -------------------------------
void writeSettingsToFlash();
void resetSettings();
void migrateSettings(int oldVersion);
void loadSettingsFromFlash();
// UserInput related functions
void play_pause();
static void prev_track();
static void next_track();
void volume_up();
void volume_down();
void resetCard();
void setupCard();
// end of UserInput related functions
void waitForTrackToFinish();
void playFolder();
bool setupFolder(folderSettings *theFolder);
bool readCard(nfcTagObject *nfcTag);
uint8_t voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
                  bool preview = false, int previewFromFolder = 0, int defaultValue = 0, bool exitWithLongPress = false);
bool isPlaying();
void writeCard(nfcTagObject nfcTag);
void dump_byte_array(byte *buffer, byte bufferSize);
void init_random_generator(uint8_t rndmPin);
void timer1_task_1ms();

bool authenticate_card(MFRC522::PICC_Type mifareType);

// Classes
// implement a DFMiniMp3 Serial notification class,
// its member methods will get called
class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
// see DfMp3_Error for code meaning
#if DEBUGSERIAL
    Serial.println();
    Serial.print("DFMiniMp3 Com Error ");
    Serial.println(errorCode);
#endif
  }
  static void OnPlayFinished(uint16_t track)
  {
    next_track();
  }
  static void OnCardOnline(uint16_t code)
  {
#if DEBUGSERIAL
    Serial.println(F("SD Karte online "));
#endif
  }
  static void OnCardInserted(uint16_t code)
  {
#if DEBUGSERIAL
    Serial.println(F("SD Karte bereit "));
#endif
  }
  static void OnCardRemoved(uint16_t code)
  {
#if DEBUGSERIAL
    Serial.println(F("SD Karte entfernt "));
#endif
  }
  static void OnUsbOnline(uint16_t code)
  {
#if DEBUGSERIAL
    Serial.println(F("USB online "));
#endif
  }
  static void OnUsbInserted(uint16_t code)
  {
#if DEBUGSERIAL
    Serial.println(F("USB bereit "));
#endif
  }
  static void OnUsbRemoved(uint16_t code)
  {
#if DEBUGSERIAL
    Serial.println(F("USB entfernt "));
#endif
  }
};

// Global variables & objects -------------------------------

// Settings, folders etc.
adminSettings mySettings;
nfcTagObject myCard;
folderSettings *myFolder;
uint16_t idleCount = 0;

//MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522
MFRC522::MIFARE_Key key;
bool knownCard = false;
bool successRead;
byte sector = 1;
byte blockAddr = 4;
byte trailerBlock = 7;

static const uint32_t cardCookie = 322417479; //TODO: Magic number?

// DFPlayer Mini
SoftwareSerial mySoftwareSerial(DFMINI_RX, DFMINI_TX); // RX, TX
uint16_t numTracksInFolder;
uint16_t currentTrack;
uint8_t volume;
static DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(mySoftwareSerial);

// keepAlive interface
KeepAlive aKeepAlive = KeepAlive(KEEPALIVE, false);

// userInput interface
UserInput *aUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);

StatusLed aLed = StatusLed(LED_PIN, 500, 200, HIGH);

// SETUP ROUTINE --------------------------------------------------------------
//-----------------------------------------------------------------------------

void setup()
{
  //Activate KeepAlive to maintain power supply to circuits
  aKeepAlive.keep_alive();
  aLed.set_led_behavior(StatusLed::solid);
#if DEBUGSERIAL
  Serial.begin(9600); // Es gibt ein paar Debug Ausgaben über die serielle Schnittstelle
#endif
  // Wert für randomSeed() erzeugen durch das mehrfache Sammeln von rauschenden LSBs eines offenen Analogeingangs
  init_random_generator(PINANALOG_RNDMGEN);
#if DEBUGSERIAL
  Serial.println(F("Tonuino started."));
#endif

  // Busy Pin
  pinMode(DFMINI_BUSY, INPUT);
  // load Settings from EEPROM
  loadSettingsFromFlash();

  // Init DFPlayer Mini
  mp3.begin();
  // Zwei Sekunden warten bis der DFPlayer Mini initialisiert ist
  delay(2000);
  volume = mySettings.initVolume;
  mp3.setVolume(volume);
  mp3.setEq(static_cast<DfMp3_Eq>(mySettings.eq - 1));

  // NFC Leser initialisieren
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  mfrc522
      .PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }

  //Init Timer1 for Encoder read
  Timer1.initialize(TIMERONE_TASK_INTERVAL_USEC);
  Timer1.attachInterrupt(timer1_task_1ms);

  //init UserInput
  //aUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
  //aUserInput->init();
}

// LOOP ROUTINE --------------------------------------------------------------
//----------------------------------------------------------------------------
void loop()
{
  //UserInput::UserRequest userAction = eNoAction;
  do
  {
    mp3.loop();

    //Set LED
    if (isPlaying())
    {
      aLed.set_led_behavior(StatusLed::solid);
    }
    else
    {
      idleCount++; // TODO: Solve via interrupt
      aLed.set_led_behavior(StatusLed::flash_slow);
    }

    //Check shutdown timer and shutdown if necessary
    if (idleCount > MAXIDLE)
    {
      idleCount = 0;
      //Deactivate KeepAlive to cut power supply to circuits
      //aKeepAlive.shut_down();
    }

    UserInput::UserRequest_e userAction;
      userAction = aUserInput->get_user_request(mfrc522.PICC_ReadCardSerial()); //ReadCardSerial() only returns true if a known card is present.
      switch (userAction)
      {
          case UserInput::NoAction:
              break;
          case UserInput::PlayPause:
              play_pause();
              break;
          case UserInput::NextTrack:
              next_track();
              break;
          case UserInput::PrevTrack:
              prev_track();
              break;
          case UserInput::IncVolume:
              volume_up();
              break;
          case UserInput::DecVolume:
              volume_down();
              break;
          case UserInput::DelCard:
              resetCard();
              break;
          case UserInput::Help:
              mp3.playMp3FolderTrack(MSG_HELP);
              break;
      }

  } while (!mfrc522.PICC_IsNewCardPresent());
  // new card present
  if (!mfrc522.PICC_ReadCardSerial())
  {
    #if DEBUGSERIAL
        Serial.println(F("Couldn't read card. Error."));
    #endif
    return; // if it couldn't be read
  }
  if (readCard(&myCard) == true)
  {
    if (myCard.cookie == cardCookie && myCard.nfcFolderSettings.folder != 0 && myCard.nfcFolderSettings.mode != 0)
    {
      // known card
      playFolder();
    }
    else if (myCard.cookie != cardCookie)
    {
      // unknown card: configure
      knownCard = false;
      mp3.playMp3FolderTrack(MSG_UNKNOWNTAG);
      waitForTrackToFinish();
      setupCard();
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
// END OF LOOP() ----------------------------------------------------------------------------------

bool isPlaying()
{
  return !digitalRead(DFMINI_BUSY);
}

void play_pause()
{
  if (isPlaying())
  {
    mp3.pause();
  }
  else if (knownCard)
  {
    mp3.start();
  }
}

//Blocker function to make feature wait until voice prompt has played
//To ensure following voice prompts do not overwrite current
//Better solution would be a queue ;)
void waitForTrackToFinish()
{
  unsigned long currentTime = millis();
#define TIMEOUT 1000
  while (!isPlaying() && millis() < (currentTime + TIMEOUT))
  {
    mp3.loop(); //wait for track to start (until timeout kicks in)
  }
  while (isPlaying())
  {
    mp3.loop(); //wait for track to finish
  }
}

// Module doesn't support queues. This is handled here as well.
static void next_track()
{

  if (knownCard == false)
  {
    return; // Cannot play a track if the card is not linked.
  }

#if DEBUGSERIAL
  Serial.println(F("=== next_track()"));
#endif

  if (myFolder->mode == ONELARGETRACK)
  {
#if DEBUGSERIAL
    Serial.println(F("Hörspielmodus ist aktiv -> keinen neuen Track spielen"));
#endif
    return;
  }
  else if ((myFolder->mode == ALBUM) && (currentTrack < numTracksInFolder))
  {
    currentTrack++;
    mp3.playFolderTrack(myFolder->folder, currentTrack);
#if DEBUGSERIAL
    Serial.print(F("Albummodus ist aktiv -> nächster Track: "));
    Serial.print(currentTrack);
#endif
  }
  else if (myFolder->mode == SAVEPROGRESS)
  {
    if (currentTrack < numTracksInFolder)
    {
      currentTrack = currentTrack + 1;
#if DEBUGSERIAL
      Serial.print(F("Hörbuch Modus ist aktiv -> nächster Track und "
                     "Fortschritt speichern"));
      Serial.println(currentTrack);
#endif
      mp3.playFolderTrack(myFolder->folder, currentTrack);
      // Fortschritt im EEPROM abspeichern
      EEPROM.update(myFolder->folder, currentTrack);
    }
    else
    {
      // Fortschritt zurück setzen
      EEPROM.update(myFolder->folder, 1);
    }
  }
}

static void prev_track()
{
#if DEBUGSERIAL
  Serial.println(F("=== prev_track()"));
#endif
  if (myFolder->mode == ALBUM)
  {
#if DEBUGSERIAL
    Serial.println(F("Albummodus ist aktiv -> vorheriger Track"));
#endif
    if (currentTrack > 1)
    {
      currentTrack--;
    }
    mp3.playFolderTrack(myFolder->folder, currentTrack);
  }
  else if (myFolder->mode == SAVEPROGRESS)
  {
#if DEBUGSERIAL
    Serial.println(F("Hörbuch Modus ist aktiv -> vorheriger Track und "
                     "Fortschritt speichern"));
#endif
    if (currentTrack > 1)
    {
      currentTrack--;
      mp3.playFolderTrack(myFolder->folder, currentTrack);
      // Fortschritt im EEPROM abspeichern
      EEPROM.update(myFolder->folder, currentTrack);
    }
  }
}

void volume_up()
{
#if SERIAL
  Serial.println(F("=== volumeUp()"));
  Serial.println(volume);
#endif
  if (volume < mySettings.maxVolume)
  {
    mp3.increaseVolume();
    volume++;
  }
}

void volume_down()
{
#if DEBUGSERIAL
  Serial.println(F("=== volumeDown()"));
  Serial.println(volume);
#endif
  if (volume > mySettings.minVolume)
  {
    mp3.decreaseVolume();
    volume--;
  }
}

void playFolder()
{
  knownCard = true;
  numTracksInFolder = mp3.getFolderTrackCount(myFolder->folder);
#if DEBUGSERIAL
  Serial.println(F("== playFolder()"));
  Serial.print(numTracksInFolder);
  Serial.print(F(" Dateien in Ordner "));
  Serial.println(myFolder->folder);
#endif

  // Hörspielmodus: eine zufällige Datei aus dem Ordner
  if (myFolder->mode == ONELARGETRACK)
  {
    currentTrack = random(1, numTracksInFolder + 1);
    mp3.playFolderTrack(myFolder->folder, currentTrack);
#if DEBUGSERIAL
    Serial.println(F("Hörspielmodus -> zufälligen Track wiedergeben"));
    Serial.println(currentTrack);
#endif
  }
  // Album Modus: kompletten Ordner spielen
  if (myFolder->mode == ALBUM)
  {
#if DEBUGSERIAL
    Serial.println(F("Album Modus -> kompletten Ordner wiedergeben"));
#endif
    currentTrack = 1;
    mp3.playFolderTrack(myFolder->folder, currentTrack);
  }
  // Hörbuch Modus: kompletten Ordner spielen und Fortschritt merken
  if (myFolder->mode == SAVEPROGRESS)
  {
#if DEBUGSERIAL
    Serial.println(F("Hörbuch Modus -> kompletten Ordner spielen und "
                     "Fortschritt merken"));
#endif
    currentTrack = EEPROM.read(myFolder->folder);
    if (currentTrack == 0 || currentTrack > numTracksInFolder)
    {
      currentTrack = 1;
    }
    mp3.playFolderTrack(myFolder->folder, currentTrack);
  }
}

uint8_t voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
                  bool preview, int previewFromFolder, int defaultValue, bool exitWithLongPress)
{
  uint8_t returnValue = defaultValue;
  uint8_t lastReturnValue = defaultValue;
  //UserRequest userAction = UserInput::UserRequest::eNoAction;
  if (startMessage != 0)
  {
    mp3.playMp3FolderTrack(startMessage);
  }
#if DEBUGSERIAL
  Serial.print(F("=== voiceMenu() ("));
  Serial.print(numberOfOptions);
  Serial.println(F(" Options)"));
#endif
  while (true)
  {
    lastReturnValue = returnValue;
#if DEBUGSERIAL
    //get command from Serial
    if (Serial.available() > 0)
    {
      int optionSerial = Serial.parseInt();
      if (optionSerial != 0 && optionSerial <= numberOfOptions)
        return optionSerial;
    }
#endif

    //userAction = aUserInput->get_user_request(true); //input acts as if a card was present: needed to detect Abort signal
    mp3.loop();

    /*
        switch userAction
        {
            case UserInput::UserRequest::eAbort :
                mp3.playMp3FolderTrack(MSG_ABORTEED);
                return defaultValue;
            case UserInput::UserRequest::ePlayPause :
                if (returnValue != 0) 
                {
                    #if DEBUGSERIAL
                        Serial.print(F("=== "));
                        Serial.print(returnValue);
                        Serial.println(F(" ==="));
                    #endif
                    return returnValue;
                }
                break;
            case UserInput::UserRequest::eNextTrack :
                returnValue = min(returnValue + 1, numberOfOptions);
                break;
            case UserInput::UserRequest::ePrevTrack :
                returnValue = max(returnValue - 1 , 1);
                break;
            case else:
                break;
        } 
        */

    if (lastReturnValue != returnValue)
    {
#if DEBUGSERIAL
      Serial.println(returnValue);
#endif
      // play number of current choice, e.g. "one".
      mp3.playMp3FolderTrack(messageOffset + returnValue);
      if (preview)
      {
        waitForTrackToFinish();
        if (!previewFromFolder)
        {
          mp3.playFolderTrack(returnValue, 1);
        }
        else
        {
          mp3.playFolderTrack(previewFromFolder, returnValue);
        }
      }
    }
  }
}

void writeSettingsToFlash()
{
#if DEBUGSERIAL
  Serial.println(F("=== writeSettingsToFlash()"));
#endif
  int address = sizeof(myFolder->folder) * 100;
  EEPROM.put(address, mySettings);
}

void resetSettings()
{
#if DEBUGSERIAL
  Serial.println(F("=== resetSettings()"));
#endif
  mySettings.cookie = cardCookie;
  mySettings.version = 2;
  mySettings.maxVolume = 25;
  mySettings.minVolume = 2;
  mySettings.initVolume = 10;
  mySettings.eq = 1;
  mySettings.locked = false;
  //mySettings.standbyTimer = 0;
  mySettings.invertVolumeButtons = true;
  mySettings.shortCuts[0].folder = 0;
  mySettings.shortCuts[1].folder = 0;
  mySettings.shortCuts[2].folder = 0;
  mySettings.shortCuts[3].folder = 0;

  writeSettingsToFlash();
}

void migrateSettings(int oldVersion)
{
  if (oldVersion == 1)
  {
#if DEBUGSERIAL
    Serial.println(F("=== migrateSettings()"));
    Serial.println(F("1 -> 2"));
#endif
    mySettings.version = 2;
    writeSettingsToFlash();
  }
}

void loadSettingsFromFlash()
{
  int address = sizeof(myFolder->folder) * 100;
  EEPROM.get(address, mySettings);
  if (mySettings.cookie != cardCookie)
    resetSettings();
  migrateSettings(mySettings.version);
#if DEBUGSERIAL
  Serial.println(F("=== loadSettingsFromFlash()"));
  Serial.print(F("Version: "));
  Serial.println(mySettings.version);
  Serial.print(F("Maximal Volume: "));
  Serial.println(mySettings.maxVolume);
  Serial.print(F("Minimal Volume: "));
  Serial.println(mySettings.minVolume);
  Serial.print(F("Initial Volume: "));
  Serial.println(mySettings.initVolume);
  Serial.print(F("EQ: "));
  Serial.println(mySettings.eq);
  Serial.print(F("Locked: "));
  Serial.println(mySettings.locked);
  Serial.print(F("Inverted Volume Buttons: "));
  Serial.println(mySettings.invertVolumeButtons);
#endif
}

//Hold to abort card reset
void resetCard()
{
  //UserRequest userAction = UserInput::UserRequest::eNoAction;
  mp3.playMp3FolderTrack(MSG_DELETETAG);
  while (!mfrc522.PICC_IsNewCardPresent())
  {
    // Wait for new card to be detected, meanwhile allow to abort
    /*userAction = aUserInput->get_user_request(true);
        if (userAction == UserInput::UserRequest::eAbort)
        {
            #if DEBUGSERIAL
                Serial.print(F("Aborted!"));
            #endif
            mp3.playMp3FolderTrack(MSG_ABORTEED);
            return;
        }*/
  }

  if (!mfrc522.PICC_ReadCardSerial())
  {
    return; //Card cannot be read
  }
#if DEBUGSERIAL
  Serial.print(F("Karte wird neu konfiguriert!"));
#endif
  mp3.playMp3FolderTrack(MSG_UNKNOWNTAG);
  waitForTrackToFinish();
  setupCard();
}

void setupCard()
{
  mp3.pause();
#if DEBUGSERIAL
  Serial.println(F("=== setupCard()"));
#endif
  nfcTagObject newCard;
  if (setupFolder(&newCard.nfcFolderSettings) == true)
  {
    // Karte ist konfiguriert -> speichern
    mp3.pause();
    waitForTrackToFinish();
    writeCard(newCard);
  }
  //delay(1000); //TODO: Test whether this delay makes any sense
}

void writeCard(nfcTagObject nfcTag)
{
  MFRC522::StatusCode status;
  MFRC522::PICC_Type mifareType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  byte buffer[16] =
      {
          0x13, 0x37, 0xb3, 0x47,           // 0x1337 0xb347 magic cookie toidentify our nfc tags
          0x02,                             // version 1
          nfcTag.nfcFolderSettings.folder,  // the folder picked by the user
          nfcTag.nfcFolderSettings.mode,    // the playback mode picked by the user
          nfcTag.nfcFolderSettings.special, // track or function for admin cards
          nfcTag.nfcFolderSettings.special2,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  //byte size = sizeof(buffer);
  if (!authenticate_card(mifareType))
  {
    return; //authentication error. Stop.
  }

// Write data to the block
#if DEBUGSERIAL
  Serial.print(F("Writing data into block "));
  Serial.print(blockAddr);
  Serial.println(F(" ..."));
#endif
  dump_byte_array(buffer, 16);

  if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
      (mifareType == MFRC522::PICC_TYPE_MIFARE_1K) ||
      (mifareType == MFRC522::PICC_TYPE_MIFARE_4K))
  {
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, buffer, 16);
  }
  else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL)
  {
    byte buffer2[16];
    byte size2 = sizeof(buffer2);

    memset(buffer2, 0, size2);
    memcpy(buffer2, buffer, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(8, buffer2, 16);

    memset(buffer2, 0, size2);
    memcpy(buffer2, buffer + 4, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(9, buffer2, 16);

    memset(buffer2, 0, size2);
    memcpy(buffer2, buffer + 8, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(10, buffer2, 16);

    memset(buffer2, 0, size2);
    memcpy(buffer2, buffer + 12, 4);
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(11, buffer2, 16);
  }

  if (status != MFRC522::STATUS_OK)
  {
#if DEBUGSERIAL
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
#endif
    mp3.playMp3FolderTrack(MSG_ERROR);
  }
  else
  {
    mp3.playMp3FolderTrack(MSG_TAGCONFSUCCESS);
  }
  delay(2000); // What is this delay for?
}

bool authenticate_card(MFRC522::PICC_Type mifareType)
{
  MFRC522::StatusCode status;
  // Authenticate using key B
  //authentificate with the card and set card specific parameters
  if ((mifareType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
      (mifareType == MFRC522::PICC_TYPE_MIFARE_1K) ||
      (mifareType == MFRC522::PICC_TYPE_MIFARE_4K))
  {
#if DEBUGSERIAL
    Serial.println(F("Authenticating again using key A..."));
#endif
    status = mfrc522.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  }
  else if (mifareType == MFRC522::PICC_TYPE_MIFARE_UL)
  {
    byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag
// Authenticate using key A
#if DEBUGSERIAL
    Serial.println(F("Authenticating UL..."));
#endif
    status = mfrc522.PCD_NTAG216_AUTH(key.keyByte, pACK);
  }

  if (status != MFRC522::STATUS_OK)
  {
#if DEBUGSERIAL
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
#endif
    mp3.playMp3FolderTrack(MSG_ERROR);
    return false;
  }
  else
  {
    return true;
  }
}

bool readCard(nfcTagObject *nfcTag)
{
  nfcTagObject tempCard;
  MFRC522::StatusCode status;
  // Show some details of the PICC (that is: the tag/card)
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
#if DEBUGSERIAL
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
#endif

  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
      (piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
      (piccType == MFRC522::PICC_TYPE_MIFARE_4K))
  {
#if DEBUGSERIAL
    Serial.println(F("Authenticating Classic using key A..."));
#endif
    status = mfrc522.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  }
  else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL)
  {
    byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the tempCard
// Authenticate using key A
#if DEBUGSERIAL
    Serial.println(F("Authenticating MIFARE UL..."));
#endif
    status = mfrc522.PCD_NTAG216_AUTH(key.keyByte, pACK);
  }

  if (status != MFRC522::STATUS_OK)
  {
#if DEBUGSERIAL
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
#endif
    return false;
  }

  // Show the whole sector as it currently is
  // Serial.println(F("Current data in sector:"));
  // mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  // Serial.println();

  // Read data from the block
  if ((piccType == MFRC522::PICC_TYPE_MIFARE_MINI) ||
      (piccType == MFRC522::PICC_TYPE_MIFARE_1K) ||
      (piccType == MFRC522::PICC_TYPE_MIFARE_4K))
  {
#if DEBUGSERIAL
    Serial.print(F("Reading data from block "));
    Serial.print(blockAddr);
    Serial.println(F(" ..."));
#endif
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
#endif
      return false;
    }
  }
  else if (piccType == MFRC522::PICC_TYPE_MIFARE_UL)
  {
    byte buffer2[18];
    byte size2 = sizeof(buffer2);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(8, buffer2, &size2);
    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
      Serial.print(F("MIFARE_Read_1() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
#endif
      return false;
    }
    memcpy(buffer, buffer2, 4);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(9, buffer2, &size2);
    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
      Serial.print(F("MIFARE_Read_2() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
#endif
      return false;
    }
    memcpy(buffer + 4, buffer2, 4);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(10, buffer2, &size2);
    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
      Serial.print(F("MIFARE_Read_3() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
#endif
      return false;
    }
    memcpy(buffer + 8, buffer2, 4);

    status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(11, buffer2, &size2);
    if (status != MFRC522::STATUS_OK)
    {
#if DEBUGSERIAL
      Serial.print(F("MIFARE_Read_4() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
#endif
      return false;
    }
    memcpy(buffer + 12, buffer2, 4);
  }

#if DEBUGSERIAL
  Serial.print(F("Data on Card "));
  Serial.println(F(":"));
  dump_byte_array(buffer, 16);
  Serial.println();
  Serial.println();
#endif

  uint32_t tempCookie;
  tempCookie = (uint32_t)buffer[0] << 24;
  tempCookie += (uint32_t)buffer[1] << 16;
  tempCookie += (uint32_t)buffer[2] << 8;
  tempCookie += (uint32_t)buffer[3];

  tempCard.cookie = tempCookie;
  tempCard.version = buffer[4];
  tempCard.nfcFolderSettings.folder = buffer[5];
  tempCard.nfcFolderSettings.mode = buffer[6];
  tempCard.nfcFolderSettings.special = buffer[7];
  tempCard.nfcFolderSettings.special2 = buffer[8];

  if (tempCard.cookie == cardCookie)
  {
    if (tempCard.nfcFolderSettings.folder == 0)
    {
      if (tempCard.nfcFolderSettings.mode != 0 && tempCard.nfcFolderSettings.mode != 255)
      {
        if (isPlaying())
        {
          mp3.playAdvertisement(260); //Chime
        }
        else
        {
          mp3.start();
          delay(100);
          mp3.playAdvertisement(260); //Chime
          delay(100);
          mp3.pause();
        }
      }
      switch (tempCard.nfcFolderSettings.mode)
      {
      case 0:
      case 255:
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        break;
      }
      delay(2000);
      return false;
    }
    else
    {
      memcpy(nfcTag, &tempCard, sizeof(nfcTagObject));
      myFolder = &nfcTag->nfcFolderSettings;
#if DEBUGSERIAL
      Serial.println(nfcTag->nfcFolderSettings.folder);
      Serial.println(myFolder->folder);
#endif
    }
    return true;
  }
  else
  {
    memcpy(nfcTag, &tempCard, sizeof(nfcTagObject));
    return true;
  }
}

bool setupFolder(folderSettings *theFolder)
{
  // Ordner abfragen
  theFolder->folder = voiceMenu(99, 301, 0, true, 0, 0, true); //TODO: Shouldn't this be 300? or 310?
  if (theFolder->folder == 0)
    return false;

  // Wiedergabemodus abfragen
  theFolder->mode = voiceMenu(9, MSG_TAGLINKED, MSG_TAGLINKED, false, 0, 0, true);
  if (theFolder->mode == 0)
    return false;

  //  // Hörbuchmodus -> Fortschritt im EEPROM auf 1 setzen
  //  EEPROM.update(theFolder->folder, 1);

  // Einzelmodus -> Datei abfragen
  if (theFolder->mode == 4)
    theFolder->special = voiceMenu(mp3.getFolderTrackCount(theFolder->folder), MSG_SELECTFILE, 0,
                                   true, theFolder->folder);
  // Admin Funktionen
  if (theFolder->mode == 6)
  {
    //theFolder->special = voiceMenu(3, MSG_SELECTFILE, MSG_SELECTFILE);
    theFolder->folder = 0;
    theFolder->mode = 255;
  }
  // Spezialmodus Von-Bis
  if (theFolder->mode == 7 || theFolder->mode == 8 || theFolder->mode == 9)
  {
    theFolder->special = voiceMenu(mp3.getFolderTrackCount(theFolder->folder), 321, 0,
                                   true, theFolder->folder);
    theFolder->special2 = voiceMenu(mp3.getFolderTrackCount(theFolder->folder), 322, 0,
                                    true, theFolder->folder, theFolder->special);
  }
  return true;
}

/**
  Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void init_random_generator(uint8_t rndmPin)
{
  uint32_t ADC_LSB;
  uint32_t ADCSeed;
  for (uint8_t i = 0; i < 128; i++)
  {
    ADC_LSB = analogRead(rndmPin) & 0x1;
    ADCSeed ^= ADC_LSB << (i % 32);
  }
  randomSeed(ADCSeed); // Zufallsgenerator initialisieren
}

void timer1_task_1ms()
{
  aLed.led_service();
  //aUserInput.userinput_service_isr();
}