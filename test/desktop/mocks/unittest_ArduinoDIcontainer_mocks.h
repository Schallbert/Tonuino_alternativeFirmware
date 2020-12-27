#ifndef UNITTEST_ARDUINODICONTAINER_MOCKS_H
#define UNITTEST_ARDUINODICONTAINER_MOCKS_H

#include <gmock/gmock.h>
#include "../Arduino_HardwareAbstraction/Arduino_DIcontainer_interface.h"
#include "../Arduino_HardwareAbstraction/Arduino_interface.h"

#include "unittest_ArduinoIf_mocks.h"

using ::testing::NiceMock;
using ::testing::ReturnPointee;

class Mock_ArduinoDIcontainer : public Arduino_DIcontainer_interface
{
public:
    MOCK_METHOD(Arduino_interface_pins &, getPins, (), (override));
    MOCK_METHOD(Arduino_interface_delay &, getDelay, (), (override));
    MOCK_METHOD(Arduino_interface_com &, getSerial, (), (override));
    MOCK_METHOD(Arduino_interface_random &, getRandom, (), (override));
    MOCK_METHOD(Arduino_interface_eeprom &, getEeprom, (), (override));

    // "Tunnels" DIcontainer.
    // Returns a mock instance of any of the corresponding parent methods.
    void DelegateToMockPins(Mock_pinCtrl *pins)
    {
        m_pPins = pins;
        ON_CALL(*this, getPins).WillByDefault(ReturnPointee(m_pPins));
    };
    void DelegateToMockDelay(Mock_delay *delay)
    {
        m_pDelay = delay;
        ON_CALL(*this, getDelay).WillByDefault(ReturnPointee(m_pDelay));
    };
    void DelegateToMockSerial(Mock_serial *serial)
    {
        m_pSerial = serial;
        ON_CALL(*this, getSerial).WillByDefault(ReturnPointee(m_pSerial));
    };
    void DelegateToMockRandom(Mock_random *random)
    {
        m_pRandom = random;
        ON_CALL(*this, getRandom).WillByDefault(ReturnPointee(m_pRandom));
    };
    void DelegateToMockEeprom(Mock_eeprom *eeprom)
    {
        m_pEeprom = eeprom;
        ON_CALL(*this, getEeprom).WillByDefault(ReturnPointee(m_pEeprom));
    };

private:
    Mock_pinCtrl *m_pPins{nullptr};
    Mock_delay *m_pDelay{nullptr};
    Mock_serial *m_pSerial{nullptr};
    Mock_random *m_pRandom{nullptr};
    Mock_eeprom *m_pEeprom{nullptr};
};
#endif // UNITTEST_ARDUINODICONTAINER_MOCKS_H