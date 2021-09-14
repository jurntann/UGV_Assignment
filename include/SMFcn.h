#ifndef SMFCN_H
#define SMFCN_H

#include <Windows.h>
#include <tchar.h>

//Code framework by A/Prof. J.Katupitiya(c)2013

#define ERR_CREATE_FILEMAP 4
#define ERR_MAP_FILEVIEW   5
#define ERR_ACCESS_FILEVIEW 6

//Share memory functions
HANDLE SMCreateReadWrite(int objectSize, TCHAR szName[]);
HANDLE SMOpenReadWrite(TCHAR szName[]);
void* SMAccessReadWrite(HANDLE openedHandle, int objectSize);
#endif
