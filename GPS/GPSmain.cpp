#include"GPS.h"
using namespace System;
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
	/*
	// create transmission control protocol client object
	TcpClient^ Client;
	int PortNumber = 24000; // create port number
	//instantiate tcpclient object and connect to it
	Client = gcnew TcpClient("192.168.1.200", PortNumber);
	//Configure client
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500; // wait 500 ms before reporting error
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024; // when data comes, set aside 1kb of memory to store data
	Client->SendBufferSize = 1024;

	// Initialise all send and receive data for authentication
	array<unsigned char>^ RecvData;
	array<unsigned char>^ SendData; // these are array of bytes
	RecvData = gcnew array<unsigned char>(5000);
	SendData = gcnew array<unsigned char>(16);
	NetworkStream^ Stream = Client->GetStream(); 
	*/
	int LIMIT = 10;
	int strike = 0;
	while (1) {
		
		//Stream->Read(RecvData, 0, RecvData->Length);
		// By this time, RecvData has data to fit GPS type object
		// Binary to String Decoding
		// String^ GPSData = Encoding::ASCII->GetString(RecvData);

		if (PMData->Heartbeat.Flags.GPS == 0) {
			// check that heartbeat has been set to 0 by processmanagement
			// if it has, then set it back to 1 
			PMData->Heartbeat.Flags.GPS = 1;
			strike = 0;
		} else {
			// if the heartbeat is still 1 
			// this means processmanagement has dieded and so everything should stop
			std::cout << "process management is dieded" << std::endl;
			strike++;
			if (strike > LIMIT) {
				exit(0);
			}
		}
		if (PMData->Shutdown.Status) {
			std::cout << "terminating program" << std::endl;
			break;

		}
		Thread::Sleep(1000);
	}
	return 0;
}