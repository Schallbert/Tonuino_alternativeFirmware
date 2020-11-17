#ifndef ERRORHANDLER_MOCKS_H
#define ERRORHANDLER_MOCKS_H

#include <gmock/gmock.h>

#include "../ErrorHandler/ErrorHandler_interface.h"

class Mock_ErrorHandler : public ErrorHandler_interface
{
public:
    MOCK_METHOD(void, handleErrors, (), (override));

    MOCK_METHOD(void, printDebugMessage, (), (override));

    MOCK_METHOD(void, setHelpRequested, (), (override));
    MOCK_METHOD(void, setTagReadError, (), (override));
    MOCK_METHOD(void, setFolderError, (), (override));
};

#endif // ERRORHANDLER_MOCKS_H