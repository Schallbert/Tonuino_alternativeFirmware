#ifndef UNITTEST_POWERMANAGER_MOCKS_H
#define UNITTEST_POWERMANAGER_MOCKS_H

#include <gmock/gmock.h>
#include "../Arduino/Arduino_interface/Arduino_types.h"
#include <PowerManager_interface.h>

class Mock_PowerManager : public PowerManager_interface
{
public:
    MOCK_METHOD(void, set_playback, (bool isPlaying), (override));
    MOCK_METHOD(void, set_delMenu, (), (override));
    MOCK_METHOD(void, set_linkMenu, (), (override));
    MOCK_METHOD(void, request_shutdown, (), (override));
    MOCK_METHOD(bool, get_shutdown_request, (), (override));
    MOCK_METHOD(void, allow_shutdown, (), (override));
    MOCK_METHOD(void, notify_timer_tick, (), (override));
};

#endif // UNITTEST_POWERMANAGER_MOCKS_H