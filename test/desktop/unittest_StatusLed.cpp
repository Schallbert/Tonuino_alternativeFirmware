#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Utilities/StatusLed/StatusLed.h"
#include "mocks/unittest_ArduinoIf_mocks.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::Return;

class StatusLedTest : public ::testing::Test
{
protected:
    NiceMock<Mock_pinCtrl> pinCtrl{};
    uint8_t ledPinId{13};
    bool pinActiveState{LOW};
    uint16_t msFlashSlow{6};
    uint16_t msFlashQuick{2};
    StatusLed m_StatusLed{StatusLed(pinCtrl,
                             ledPinId,
                             pinActiveState,
                             msFlashSlow,
                             msFlashQuick)};
};

TEST_F(StatusLedTest, Constructor_InitLedStateIsOff)
{
    InSequence seq;
    EXPECT_CALL(pinCtrl, pin_mode(ledPinId, OUTPUT));
    EXPECT_CALL(pinCtrl, digital_write(ledPinId, !pinActiveState));
    StatusLed testStatusLed(pinCtrl,
                            ledPinId,
                            pinActiveState,
                            msFlashSlow,
                            msFlashQuick);
}

TEST_F(StatusLedTest, service_InitLedStateIsOff)
{
    EXPECT_CALL(pinCtrl, digital_write(ledPinId, !pinActiveState));
    m_StatusLed.led_service();
}

TEST_F(StatusLedTest, setLed_solid_StateIsOn)
{
    EXPECT_CALL(pinCtrl, digital_write(ledPinId, pinActiveState));
    m_StatusLed.set_led_behavior(StatusLed::solid);
    m_StatusLed.led_service();
}

TEST_F(StatusLedTest, setLed_flash_workingCorrectly)
{
    m_StatusLed.set_led_behavior(StatusLed::flash_slow); // if this works, flash_quick is trivial and should also work
    for (uint8_t i = 0; i < msFlashSlow; ++i)
    {
        m_StatusLed.led_service();
    }
    ON_CALL(pinCtrl, digital_read(ledPinId)).WillByDefault(Return(pinActiveState));
    EXPECT_CALL(pinCtrl, digital_write(_, !pinActiveState));
    for (uint8_t i = 0; i < msFlashSlow; ++i)
    {
        m_StatusLed.led_service();
    }
    ON_CALL(pinCtrl, digital_read(ledPinId)).WillByDefault(Return(!pinActiveState));
    EXPECT_CALL(pinCtrl, digital_write(_, pinActiveState));
    m_StatusLed.led_service();
}

TEST_F(StatusLedTest, setLed_dim_workingCorrectly)
{
    m_StatusLed.set_led_behavior(StatusLed::dim);
    {
        InSequence seq;
        EXPECT_CALL(pinCtrl, digital_write(_, !pinActiveState)).Times(8);
        EXPECT_CALL(pinCtrl, digital_write(_, pinActiveState)).Times(1);
    }
    for (uint8_t i = 0; i < 9; ++i)
    {
        m_StatusLed.led_service();
    }
}