#include"GPS.h"

int main() {
	// Declare an SM Object instance
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// SM Creation and seeking access
	//PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	while (1) {
		if (PMData->Heartbeat.Flags.GPS == 0) {
			// check that heartbeat has been set to 0 by processmanagement
			// if it has, then set it back to 1 
			PMData->Heartbeat.Flags.GPS = 1;
		} else {
			// if the heartbeat is still 1 
			// this means processmanagement has dieded and so everything should stop
			std::cout << "process management is dieded" << std::endl;
			exit(0);
			}
		if (PMData->Shutdown.Status)
			break;
	}
	return 0;
}