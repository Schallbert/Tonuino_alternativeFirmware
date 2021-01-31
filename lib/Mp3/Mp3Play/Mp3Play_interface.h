#ifndef MP3PLAY_INTERFACE_H
#define MP3PLAY_INTERFACE_H

class Folder;

class Mp3Play_interface
{
public:
    virtual ~Mp3Play_interface(){};
    virtual void playFolder(Folder &folder) = 0;

    virtual void playPrev() = 0;
    virtual void playNext() = 0;
    virtual void autoplay() = 0;
};

#endif // MP3PLAY_INTERFACE_H