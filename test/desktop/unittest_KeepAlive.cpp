#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Utilities/KeepAlive/KeepAlive.h"
#include "mocks/unittest_ArduinoIf_mocks.h"

using ::testing::_;
using ::testing::Sequence;
using ::testing::NiceMock;
using ::testing::Return;

TEST(keepAlive, Constructor_ActivatesSelfKeepingPowerSupply)
{
    uint8_t keepAlivePinId = 7;
    bool pinActiveState = true;
    NiceMock<Mock_pinCtrl> pinCtrl;
    Sequence seq;
    EXPECT_CALL(pinCtrl, pin_mode(keepAlivePinId, OUTPUT));
    EXPECT_CALL(pinCtrl, digital_write(keepAlivePinId, pinActiveState));
    KeepAlive ka(&pinCtrl, keepAlivePinId, pinActiveState);
}

TEST(keepAlive, keepAlive_writesActiveState_False_Correctly)
{
    NiceMock<Mock_pinCtrl> pinCtrl;
    bool pinActiveState = false;
    KeepAlive ka(&pinCtrl, 1, pinActiveState);
    EXPECT_CALL(pinCtrl, digital_write(1, pinActiveState));
    ka.keep_alive();
}

TEST(keepAlive, keepAlive_writesActiveState_True_Correctly)
{
    NiceMock<Mock_pinCtrl> pinCtrl;
    bool pinActiveState = true;
    KeepAlive ka(&pinCtrl, 1, pinActiveState);
    EXPECT_CALL(pinCtrl, digital_write(1, pinActiveState));
    ka.keep_alive();
}

TEST(keepAlive, getShutdownRequest_returnsFalseByDefault)
{
    NiceMock<Mock_pinCtrl> pinCtrl;
    bool pinActiveState = true;
    KeepAlive ka(&pinCtrl, 1, pinActiveState);
    EXPECT_FALSE(ka.get_shutdown_request());
}

TEST(keepAlive, getShutdownRequest_returnsTrueWhenRequested)
{
    NiceMock<Mock_pinCtrl> pinCtrl;
    bool pinActiveState = true;
    KeepAlive ka(&pinCtrl, 1, pinActiveState);
    ka.request_shutdown();
    EXPECT_TRUE(ka.get_shutdown_request());
}

TEST(keepAlive, allowShutdown_notRequested_willNotShutdown)
{
    NiceMock<Mock_pinCtrl> pinCtrl;
    bool pinActiveState = true;
    KeepAlive ka(&pinCtrl, 1, pinActiveState);
    EXPECT_CALL(pinCtrl, digital_write(1, !pinActiveState)).Times(0);
    ka.allow_shutdown();
}

TEST(keepAlive, allowShutdown_Requested_willShutdown)
{
    NiceMock<Mock_pinCtrl> pinCtrl;
    bool pinActiveState = true;
    KeepAlive ka(&pinCtrl, 1, pinActiveState);
    ka.request_shutdown();
    EXPECT_CALL(pinCtrl, digital_write(1, !pinActiveState));
    ka.allow_shutdown();
}

