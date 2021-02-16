#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../Utilities/KeepAlive/KeepAlive.h"
#include "mocks/unittest_ArduinoIf_mocks.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Sequence;

class KeepAliveTest : public ::testing::Test
{

protected:
    NiceMock<Mock_pinCtrl> pinCtrl{}; 
    uint8_t keepAlivePinId{7};
    bool pinActiveState{true};
    KeepAlive m_keepAlive{pinCtrl, keepAlivePinId, pinActiveState};
};

TEST_F(KeepAliveTest, Constructor_ActivatesSelfKeepingPowerSupply)
{
    Sequence seq;
    EXPECT_CALL(pinCtrl, pin_mode(keepAlivePinId, OUTPUT));
    EXPECT_CALL(pinCtrl, digital_write(keepAlivePinId, pinActiveState));
    KeepAlive testSpecificKa(pinCtrl, keepAlivePinId, pinActiveState);
}

TEST_F(KeepAliveTest, keepAlive_writesActiveState_False_Correctly)
{
    EXPECT_CALL(pinCtrl, digital_write(keepAlivePinId, pinActiveState));
    m_keepAlive.keep_alive();
}

TEST_F(KeepAliveTest, keepAlive_writesActiveState_True_Correctly)
{
    bool testSpecificPinActiveState = true;
    KeepAlive testSpecificKa(pinCtrl, keepAlivePinId, testSpecificPinActiveState);
    EXPECT_CALL(pinCtrl, digital_write(keepAlivePinId, testSpecificPinActiveState));
    testSpecificKa.keep_alive();
}

TEST_F(KeepAliveTest, getShutdownRequest_returnsFalseByDefault)
{
    EXPECT_FALSE(m_keepAlive.isShutdownRequested());
}

TEST_F(KeepAliveTest, getShutdownRequest_returnsTrueWhenRequested)
{
    m_keepAlive.requestShutdown();
    EXPECT_TRUE(m_keepAlive.isShutdownRequested());
}

TEST_F(KeepAliveTest, allowShutdown_notRequested_willNotShutdown)
{
    EXPECT_CALL(pinCtrl, digital_write(keepAlivePinId, !pinActiveState)).Times(0);
    m_keepAlive.allowShutdown();
}

TEST_F(KeepAliveTest, allowShutdown_Requested_willShutdown)
{
    m_keepAlive.requestShutdown();
    EXPECT_CALL(pinCtrl, digital_write(keepAlivePinId, !pinActiveState));
    m_keepAlive.allowShutdown();
}
