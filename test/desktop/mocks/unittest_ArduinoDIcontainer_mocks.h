#ifndef UNITTEST_ARDUINODICONTAINER_MOCKS_H
#define UNITTEST_ARDUINODICONTAINER_MOCKS_H

#include <gmock/gmock.h>
#include "../Arduino/Arduino_interface/Arduino_DIcontainer_interface.h"
#include "../Arduino/Arduino_interface/Arduino_interface.h"

#include "unittest_ArduinoIf_mocks.h"

using ::testing::NiceMock;
using ::testing::Return;

class Mock_ArduinoDIcontainer : public Arduino_DIcontainer_interface
{
public:
    MOCK_METHOD(Arduino_interface_pins *, getPins, (), (override));
    MOCK_METHOD(Arduino_interface_delay *, getDelay, (), (override));
    MOCK_METHOD(Arduino_interface_com *, getSerial, (), (override));
    MOCK_METHOD(Arduino_interface_random *, getRandom, (), (override));
    MOCK_METHOD(Arduino_interface_eeprom *, getEeprom, (), (override));

    // "Tunnels" DIcontainer.
    // Returns a mock instance of any of the corresponding parent methods.
    void DelegateToMockPins(Mock_pinCtrl &pins)
    {
        m_pins = pins;
        ON_CALL(*this, getPins).WillByDefault(Return(m_pins()));
    };
    void DelegateToMockDelay(Mock_delay &delay)
    {
        m_delay = delay;
        ON_CALL(*this, getDelay).WillByDefault(Return(m_delay));
    };
    void DelegateToMockSerial(Mock_serial &serial)
    {
        m_serial = serial;
        ON_CALL(*this, getSerial).WillByDefault(Return(m_serial));
    };
    void DelegateToMockRandom(Mock_random random)
    {
        m_random = random;
        ON_CALL(*this, getRandom).WillByDefault(Return(m_random));
    };
    void DelegateToMockEeprom(Mock_eeprom eeprom)
    {
        m_eeprom = eeprom;
        ON_CALL(*this, getEeprom).WillByDefault(Return(m_eeprom));
    };

private:
    Mock_pinCtrl *m_pins{nullptr};
    Mock_delay *m_delay{nullptr};
    Mock_serial *m_serial{nullptr};
    Mock_random *m_random{nullptr};
    Mock_eeprom *m_eeprom{nullptr};
};
#endif // UNITTEST_ARDUINODICONTAINER_MOCKS_H