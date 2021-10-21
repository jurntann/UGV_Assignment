#include"GPS.h"
#include<conio.h>
using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Diagnostics;

int main() {
	GPS GPSboi;
	GPSboi.setupSharedMemory();

	// create transmission control protocol client object
	int PortNumber = 24000; // create port number
	String^ hostName = "192.168.1.200";
	GPSboi.connect(hostName, PortNumber);
	while (!_kbhit()) {
		GPSboi.manageHB();
		GPSboi.getShutdownFlag();
		GPSboi.getData();
		GPSboi.sendDataToSharedMemory();
		GPSboi.checkData();
		Thread::Sleep(1000);
	}
	return 0;
}