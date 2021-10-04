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

	// create transmission control protocol client object
	TcpClient^ Client;
	int PortNumber = 23000; // create port number
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
	array<unsigned char>^ SendData;
	RecvData = gcnew array<unsigned char>(5000);
	SendData = gcnew array<unsigned char>(1024);

	// authentication of zID by laser 
	System::String^ Message = gcnew String("# ");
	Message = Message + "5261433\n";
	SendData = Encoding::ASCII->GetBytes(Message);
	NetworkStream^ Stream = Client->GetStream();
	Stream->Write(SendData, 0, SendData->Length);
	Thread::Sleep(10000); // wait for authentication

	array<unsigned char>^ authData;
	authData = gcnew array<unsigned char>(1024);
	Stream = Client->GetStream();
	Stream->Read(authData, 0, authData->Length);
	System::String^ LaserData = Encoding::ASCII->GetString(authData);
	array<wchar_t>^ newLine = { '\n' };
	array<String^>^ authArray = LaserData->Split(newLine);
	if (authArray[0] == "OK") {
		std::cout << "authenticated" << std::endl;
	}


	// Laser loop
	while (1) {
		// Get network stream
		Stream = Client->GetStream();
		Stream->Read(RecvData, 0, RecvData->Length);

		// This may not read all your data in one read, use a loop
		int NumData = 0;
		while (NumData != sizeof(RecvData->Length)) {
			NumData += Stream->Read(RecvData, NumData, sizeof(RecvData->Length) - NumData);
		}
		// By this time, RecvData has data to fit GPS type object
		// Binary to String Decoding
		// what is LaserData type??
		System::String^ LaserData = Encoding::ASCII->GetString(RecvData);
		array<wchar_t>^ Space = { ' ' };
		array<String^>^ StringArray = LaserData->Split(Space);

		double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
		double Resolution = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
		int NumRanges = System::Convert::ToInt32(StringArray[25], 16);
		array<double>^ Range = gcnew array<double>(NumRanges);
		array<double>^ RangeX = gcnew array<double>(NumRanges);
		array<double>^ RangeY = gcnew array<double>(NumRanges);
		for (int i = 0; i < NumRanges; i++) {
			Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
			RangeX[i] = Range[i] * sin(i * Resolution);
			RangeY[i] = -Range[i] * cos(i * Resolution);
		}
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

