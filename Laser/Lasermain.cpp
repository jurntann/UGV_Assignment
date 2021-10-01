#include "Laser.h"
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
	//PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	while (1) {
		if (PMData->Heartbeat.Flags.Laser== 0) {
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
		if (PMData->Shutdown.Status) {
			std::cout << "terminating program" << std::endl;
			exit(0);
		}
		Thread::Sleep(1000);
	}
	return 0;
}

