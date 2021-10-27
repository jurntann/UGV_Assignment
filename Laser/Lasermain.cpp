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
	Laser Laserboi;
	Laserboi.setupSharedMemory();

	// create transmission control protocol client object
	int PortNumber = 23000; // create port number
	String^ hostName = "192.168.1.200";
	Laserboi.connect(hostName, PortNumber);

	// Initialise all send and receive data for authentication 


	// authentication of zID by laser 
	Laserboi.authData();
	Thread::Sleep(10); // wait for authentication

	Laserboi.getData();
	Console::WriteLine("authenticated");
	Laserboi.setTimer();
	// Laser loop
	while (!_kbhit()) {
		Laserboi.manageHB();
		Laserboi.getShutdownFlag();
		Laserboi.sendData();
		Console::WriteLine("sent request");
		Thread::Sleep(10);
		Laserboi.getData();
		Console::WriteLine("got data");
		if (Laserboi.checkData() == 1) {
			Laserboi.processData();
			Console::WriteLine("process data");
			Laserboi.sendDataToSharedMemory();
			Console::WriteLine("sent to shared memory data");
		} else {
			Console::WriteLine("nah bro it aint");
		}
		/*Heartbeat stuff
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
		*/

		Thread::Sleep(50);		
	}
	return 0;
}

