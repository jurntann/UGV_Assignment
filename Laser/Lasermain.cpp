#include "Laser.h"
#include <string>
#include <string.h>
#include <array>
#include "SMStructs.h"
#include "SMObject.h"
#include <direct.h>
#include <iostream>

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
	// Declare an SM Object instance
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	// SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	Laser Laserboi;
	Laserboi.setupSharedMemory();

	// create transmission control protocol client object
	int PortNumber = 23000; // create port number
	String^ hostName = "192.168.1.200";
	Laserboi.connect(hostName, PortNumber);

	// Initialise all send and receive data for authentication 


	// authentication of zID by laser 
	String^ Message = gcnew String("5261433\n"); // characters that can be read
	String^ AskScan = gcnew String("sRN LMDscandata");
	Laserboi.sendData(Message);
	Thread::Sleep(10); // wait for authentication

	Laserboi.getData();
	Console::WriteLine("authenticated");

	// Laser loop
	while (!_kbhit()) {

		Laserboi.sendData(AskScan);
		Console::WriteLine("sent request");
		Thread::Sleep(10);
		Laserboi.getData();
		Laserboi.processData();
		Laserboi.sendDataToSharedMemory();
		// By this time, RecvData has data to fit GPS type object
		// Binary to String Decod
		// Heartbeat stuff
		Laserboi.setHeartbeat(PMData->Heartbeat.Flags.Laser);
		if (PMData->Shutdown.Status)
			break;
		Thread::Sleep(1000);
		
	}
	return 0;
}

