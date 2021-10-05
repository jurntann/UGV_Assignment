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
	array<unsigned char>^ SendData; // these are array of bytes
	RecvData = gcnew array<unsigned char>(5000);
	SendData = gcnew array<unsigned char>(16);

	// authentication of zID by laser 
	String^ Message = gcnew String("5261433\n"); // characters that can be read
	String^ AskScan = gcnew String("sRN LMDscandata");
	String^ response;
	SendData = Encoding::ASCII->GetBytes(Message);
	NetworkStream^ Stream = Client->GetStream();
	Stream->Write(SendData, 0, SendData->Length);
	Thread::Sleep(10); // wait for authentication

	Stream->Read(RecvData, 0, RecvData->Length);
	response = Encoding::ASCII->GetString(RecvData);
	Console::WriteLine(response);

	SendData = Encoding::ASCII->GetBytes(AskScan);

	// Laser loop
	while (!_kbhit()) {

		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
		Thread::Sleep(10);
		Stream->Read(RecvData, 0, RecvData->Length);

		// By this time, RecvData has data to fit GPS type object
		// Binary to String Decoding
		String^ LaserData = Encoding::ASCII->GetString(RecvData);
		
		Console::WriteLine(LaserData);
		
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
			//SM_Laser.x[i] = RangeX[i];
			//SM_Laser.y[i] = RangeY[i];
		}
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
	Stream->Close();
	Client->Close();
	return 0;
}

