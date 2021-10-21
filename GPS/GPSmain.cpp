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
	Console::WriteLine("ok");
	// create transmission control protocol client object
	int PortNumber = 24000; // create port number
	String^ hostName = "192.168.1.200";
	GPSboi.connect(hostName, PortNumber);
	Console::WriteLine("connected");
	while (!_kbhit()) {
		GPSboi.manageHB();
		Console::WriteLine("managed hb");
		GPSboi.getShutdownFlag();
		Console::WriteLine("managed shutdown");
		GPSboi.getData();
		Console::WriteLine("got data");
		GPSboi.sendDataToSharedMemory();
		Console::WriteLine("sent to shared memory");
		GPSboi.checkData();
		Console::WriteLine("data printed");
		Thread::Sleep(1000);
	}
	return 0;
}