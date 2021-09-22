#include"GPS.h"

int main() {
	// Declare an SM Object instance
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	while (1) {
		std::cout << "GPS heartbeat:" << (int)PMData->Heartbeat.Flags.GPS << std::endl;
		if (PMData->Shutdown.Status)
			break;
	}
	return 0;
}