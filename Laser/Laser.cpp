#include"Laser.h"


using namespace System;
using namespace Net;
using namespace Sockets;
using namespace Text;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Diagnostics;


int Laser::connect(String^ hostName, int portNumber)
{
	Client = gcnew TcpClient(hostName, portNumber);
	//Configure client
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500; // wait 500 ms before reporting error
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024; // when data comes, set aside 1kb of memory to store data
	Client->SendBufferSize = 1024;
	
	// YOUR CODE HERE
	return 1;
}
int Laser::setupSharedMemory()
{
	SMObject laserObj (TEXT("Laser"), sizeof(SM_Laser));
	// SM Creation and seeking access
	laserObj.SMCreate();
	laserObj.SMAccess();
	ProcessManagement* laserTing = (ProcessManagement*)laserObj.pData;
	// YOUR CODE HERE
	return 1;
}
int Laser::getData()
{
	ReadData = gcnew array<unsigned char>(5000);
	Stream = Client->GetStream();
	Stream->Read(ReadData, 0, ReadData->Length);
	data = Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(data);
	// YOUR CODE HERE
	return 1;
}
int Laser::sendData(String^ message)
{ // self written function much wow
	Stream = Client->GetStream();
	SendData = gcnew array<unsigned char>(16);
	SendData = Encoding::ASCII->GetBytes(message);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	// YOUR CODE HERE
	return 1;
}
int Laser::processData() 
{ // self written function much wow
	array<wchar_t>^ Space = { ' ' };
	array<String^>^ StringArray = data->Split(Space);
	double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
	double Resolution = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
	int NumRanges = System::Convert::ToInt32(StringArray[25], 16);
	Range = gcnew array<double>(NumRanges);
	RangeX = gcnew array<double>(NumRanges);
	RangeY = gcnew array<double>(NumRanges);
	for (int i = 0; i < NumRanges; i++) {
		Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
		RangeX[i] = Range[i] * sin(i * Resolution);
		RangeY[i] = -Range[i] * cos(i * Resolution);
	}

	return 1;
}
int Laser::checkData()
{
	// YOUR CODE HERE
	return 1;
}
int Laser::sendDataToSharedMemory()
{
	for (int i = 0; i < sizeof(SM_Laser); i++) {
		laserTing.x[i] = RangeX[i];
		laserTing.y[i] = RangeY[i];
		// print data to terminal
		Console::WriteLine("X data is {0,12:F3}", laserTing.x[i]);
		Console::WriteLine("Y data is {0,12:F3}", laserTing.y[i]);
	}
	// YOUR CODE HERE
	return 1;
}
bool Laser::getShutdownFlag()
{
	// YOUR CODE HERE
	return 1;
}
int Laser::setHeartbeat(bool heartbeat)
{
	// YOUR CODE HERE
	return 1;
}
Laser::~Laser()
{
	Stream->Close();
	Client->Close();
	// YOUR CODE HERE
}
