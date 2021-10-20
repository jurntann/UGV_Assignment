#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <array>
#include "SMStructs.h"
#include "SMObject.h"
#include "VehicleControl.h"

using namespace System;
using namespace Net;
using namespace Sockets;
using namespace Text;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Diagnostics;

int main() {
	VehicleControl vehicleboi;
	vehicleboi.setupSharedMemory();

	// create transmission control protocol client object
	int PortNumber = 25000; // create port number
	String^ hostName = "192.168.1.200";
	vehicleboi.connect(hostName, PortNumber);
	vehicleboi.authData();

	Thread::Sleep(10);

	while (!_kbhit()) {	
		vehicleboi.manageHB();
		vehicleboi.getShutdownFlag();
		vehicleboi.sendData();
		Thread::Sleep(1000);
	}
	return 0;
}

