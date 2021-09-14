#ifndef SMLASER_H
#define SMLASER_H

#include <Windows.h>
#include <tchar.h>
#include "SMfcn.h"

class SMObject
{
	HANDLE CreateHandle;
	HANDLE AccessHandle;
	TCHAR *szName;
	int Size;
public:
	void *pData;
	int SMCreateError;
	int SMAccessError;
public:
	SMObject();
	SMObject(TCHAR* szname, int size);
	~SMObject();
	int SMCreate();
	int SMAccess();
	void SMObject::SetSzname(TCHAR* szname);
	void SMObject::SetSize(int size);
};
#endif


