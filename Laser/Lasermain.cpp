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

	// Laser loop
	while (!_kbhit()) {

		Laserboi.sendData(AskScan);
		Thread::Sleep(10);
		Laserboi.getData();
		Laserboi.processData();
		Laserboi.sendDataToSharedMemory();
		// By this time, RecvData has data to fit GPS type object
		// Binary to String Decod
		// Heartbeat stuff
		if (PMData->Heartbeat.Flags.Laser == 0) {
			// check that heartbeat has been set to 0 by processmanagement
			// if it has, then set it back to 1 
			PMData->Heartbeat.Flags.Laser = 1;
		}
		else {
			// if the heartbeat is still 1 
			// this means processmanagement has dieded and so everything should stop
			std::cout << "process management is dieded" << std::endl;
			//exit(0);
		}
		if (PMData->Shutdown.Status)
			break;
		Thread::Sleep(1000);
		
	}
	return 0;
}

