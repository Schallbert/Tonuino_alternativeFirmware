#ifndef NFCCONTROL_MOCKS_H
#define NFCCONTROL_MOCKS_H

#include <gmock/gmock.h>

#include "../Nfc/NfcControl/NfcControl_interface.h"
class Folder;

using ::testing::NiceMock;
using ::testing::_;
using ::testing::Return;

// this object stores nfc tag data
class Mock_NfcControl : public NfcControl_interface
{
public:
    MOCK_METHOD(void,  init, (), (override));
    MOCK_METHOD(NfcControl_interface::eTagState,  getTagPresence, (), (override));
    MOCK_METHOD(bool, readFolderFromTag, (Folder &targetFolder), (override));
    MOCK_METHOD(bool, writeFolderToTag, (const Folder &sourceFolder), (override));
    MOCK_METHOD(bool, eraseTag, (), (override));
};

#endif //NFCCONTROL_MOCKS_H