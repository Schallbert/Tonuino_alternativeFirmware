
// NFC
#include "../Nfc/Nfc/Nfc_implementation.h"
#include "../Nfc/NfcControl/NfcControl.h"
// MP3
#include "../Mp3/Mp3Play/Mp3Play_implementation.h"
#include "../Mp3/Mp3Control/Mp3Control_implementation.h"
#include "../Mp3/Mp3Prompt/Mp3Prompt_implementation.h"
// MISC
#include "../PowerManager/PowerManager_implementation.h"
#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

#include "../Tonuino/Tonuino.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mocks/unittest_UserInput_mocks.h"
#include "mocks/unittest_DfMiniMp3_mocks.h"
#include "mocks/unittest_Mp3Prompt_mocks.h"
#include "mocks/unittest_MessageHandler_mocks.h"
#include "mocks/unittest_ArduinoIf_mocks.h"
#include "mocks/unittest_ArduinoDIcontainer_mocks.h"
#include "mocks/unittest_Nfc_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

// Fixture
class SystemTest : public ::testing::Test
{
protected:
    // Mock all dependencies to Hardware ONLY. Apart from that,
    // This is a "copy" the Loader module.
    virtual void SetUp()
    {
        // relay Arduino Hal Calls to respective specialized mocks
        m_ArduinoHalMock.DelegateToMockSerial(&m_SerialMock);
        m_ArduinoHalMock.DelegateToMockPins(&m_PinsMock);
        m_ArduinoHalMock.DelegateToMockEeprom(&m_EepromMock);
        m_ArduinoHalMock.DelegateToMockRandom(&m_RandomMock);

        // Use production code implementation for business logic system tests
        m_pPwrCtrl = new PowerManager(m_ArduinoHalMock.getPins(), m_IdleTimer);

        m_pMp3Play = new Mp3Play_implementation(m_ArduinoHalMock, m_DfMiniMp3Mock, m_Mp3PromptMock, m_LullabyeTimer, m_MessageHandlerMock);
        m_pMp3Control = new Mp3Control(m_DfMiniMp3Mock, *m_pMp3Play, m_Mp3PromptMock, *m_pPwrCtrl, m_MessageHandlerMock);

        m_pNfc = new Nfc_implementation(m_Mfrc522Mock, m_MessageHandlerMock);
        m_pNfcControl = new NfcControl(*m_pNfc, m_MessageHandlerMock);

        m_pVoiceMenu = new VoiceMenu(*m_pMp3Play, m_Mp3PromptMock, *m_pNfcControl, m_MessageHandlerMock, *m_pPwrCtrl, m_MenuTimer);
        m_pTonuino = new Tonuino(&m_UserInputMock, *m_pNfcControl, *m_pMp3Control, *m_pVoiceMenu);
    }

    virtual void TearDown()
    {
        delete m_pPwrCtrl;
        delete m_pMp3Play;
        delete m_pMp3Control;
        delete m_pNfc;
        delete m_pNfcControl;
        delete m_pVoiceMenu;
        delete m_pTonuino;

        m_pPwrCtrl = nullptr;
        m_pMp3Play = nullptr;
        m_pMp3Control = nullptr;
        m_pNfc = nullptr;
        m_pNfcControl = nullptr;
        m_pVoiceMenu = nullptr;
        m_pTonuino = nullptr;
    }

protected:
    // Mocks used
    NiceMock<Mock_ArduinoDIcontainer> m_ArduinoHalMock{};
    NiceMock<Mock_serial> m_SerialMock{};
    NiceMock<Mock_pinCtrl> m_PinsMock{};
    NiceMock<Mock_eeprom> m_EepromMock{};
    NiceMock<Mock_random> m_RandomMock{};

    NiceMock<Mock_UserInput> m_UserInputMock{};
    NiceMock<Mock_DfMiniMp3> m_DfMiniMp3Mock{};
    NiceMock<Mock_MessageHandler> m_MessageHandlerMock{};
    NiceMock<Mock_Mp3Prompt> m_Mp3PromptMock{};
    NiceMock<Mock_MFRC522> m_Mfrc522Mock{};

    // Real implementations without dependencies used
    SimpleTimer m_MenuTimer{SimpleTimer()};
    SimpleTimer m_LullabyeTimer{SimpleTimer()};
    SimpleTimer m_IdleTimer{SimpleTimer()};
    SimpleTimer m_DfMiniPromptTimer{SimpleTimer()};

    // Real implementations that depend on initialized objects (realized as pointers)
    PowerManager *m_pPwrCtrl{nullptr};

    Mp3Play_implementation *m_pMp3Play{nullptr};
    Mp3Control *m_pMp3Control{nullptr};

    Nfc_implementation *m_pNfc{nullptr};
    NfcControl *m_pNfcControl{nullptr};

    VoiceMenu *m_pVoiceMenu{nullptr};
    Tonuino *m_pTonuino{nullptr};
};

// TESTS are some selected "happy paths" through the system that make sure that the modules
// are glued together nicely. Mocks only engage on the last abstraction layer before hardware.

TEST_F(SystemTest, NewKnownTag_noUserInput_playsFolder)
{
    // Should return "NEWKNOWNTAG" on MIFARE_1k, will respond with fake tag on read() call.
    ON_CALL(m_Mfrc522Mock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    ON_CALL(m_DfMiniMp3Mock, getFolderTrackCount(_)).WillByDefault(Return(5));
    m_Mfrc522Mock.DelegateToFakeMini1k4k();

    EXPECT_CALL(m_DfMiniMp3Mock, playFolderTrack(fakeBufferData[4], 1));
    m_pTonuino->loop();
}

TEST_F(SystemTest, PlayerPlaying_setsLedState)
{
    ON_CALL(m_DfMiniMp3Mock, isPlaying()).WillByDefault(Return(true));

    EXPECT_CALL(m_PinsMock, digital_write(LED_PIN, LED_ACTIVE_STATE));
    m_pTonuino->loop();
    m_pPwrCtrl->service1msLed(); // simulate 1ms Timer tick to actually make call to LED service
}

TEST_F(SystemTest, NoTag_ppLongPress_playsHelp)
{
    // Should return "NOTAG"
    ON_CALL(m_Mfrc522Mock, isTagPresent()).WillByDefault(Return(false));
    ON_CALL(m_UserInputMock, getUserRequest()).WillByDefault(Return(UserInput_interface::PP_LONGPRESS));

    VoicePrompt expectedPrompt{VoicePrompt(VoicePrompt::MSG_HELP, true)};
    EXPECT_CALL(m_Mp3PromptMock, playPrompt(expectedPrompt));
    m_pTonuino->loop();
}

TEST_F(SystemTest, NewUnknownTag_InvokesLinkMenu)
{
    ON_CALL(m_Mfrc522Mock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    // No delegate to fake, thus folder data will be empty and Tag should be set to UNKNOWN

    m_pTonuino->loop(); // enters Link Menu

    VoicePrompt expectedPrompt{VoicePrompt(VoicePrompt::MSG_SELECT_FOLDERID, true)};
    EXPECT_CALL(m_Mp3PromptMock, playPrompt(expectedPrompt));
    m_pTonuino->loop(); // once entered, prompt will play in next call
}

TEST_F(SystemTest, ActiveTag_ppLongPress_InvokesDeleteMenu)
{
    // Should return "KNOWNTAG" on MIFARE_1k, will respond with fake tag on read() call.
    ON_CALL(m_Mfrc522Mock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, setTagActive()).WillByDefault(Return(false));
    ON_CALL(m_Mfrc522Mock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    m_Mfrc522Mock.DelegateToFakeMini1k4k();
    ON_CALL(m_DfMiniMp3Mock, getFolderTrackCount(_)).WillByDefault(Return(5));
    ON_CALL(m_UserInputMock, getUserRequest()).WillByDefault(Return(UserInput_interface::PP_LONGPRESS));

    m_pTonuino->loop(); // enters Delete Menu
    ON_CALL(m_UserInputMock, getUserRequest()).WillByDefault(Return(UserInput_interface::NO_ACTION));
    VoicePrompt expectedPrompt{VoicePrompt(VoicePrompt::MSG_DELETETAG, true)};
    EXPECT_CALL(m_Mp3PromptMock, playPrompt(expectedPrompt));
    m_pTonuino->loop(); // once entered, prompt will play in next call
}

TEST_F(SystemTest, VoiceMenuActive_blocksNormalPlayback)
{
    ON_CALL(m_Mfrc522Mock, isTagPresent()).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, setTagActive()).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, tagLogin(_)).WillByDefault(Return(true));
    ON_CALL(m_Mfrc522Mock, getTagType()).WillByDefault(Return(MFRC522_interface::PICC_TYPE_MIFARE_1K));
    // No delegate to fake, thus folder data will be empty and Tag should be set to UNKNOWN

    m_pTonuino->loop(); // enters Link Menu

    EXPECT_CALL(m_DfMiniMp3Mock, isPlaying()).Times(0);
    m_pTonuino->loop(); // once entered, VoiceMenu will block playback
}

// m_MessageHandlerMock.printMessageIdToConsole();