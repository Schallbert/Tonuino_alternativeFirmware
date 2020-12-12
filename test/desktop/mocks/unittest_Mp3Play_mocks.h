#ifndef Mp3Play_MOCKS_H
#define Mp3Play_MOCKS_H

#include <gmock/gmock.h>

#include "../Mp3/Mp3Play/Mp3Play_interface.h"
class Folder;

class Mock_Mp3Play : public Mp3Play_interface
{
public:
    MOCK_METHOD(void, playFolder, (Folder & folder), (override));

    MOCK_METHOD(void, playPrev, (), (override));
    MOCK_METHOD(void, playNext, (), (override));
    MOCK_METHOD(void, autoplay, (), (override));
};

#endif // Mp3Play_MOCKS_H
