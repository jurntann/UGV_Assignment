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
	Stream = Client->GetStream();
	// YOUR CODE HERE
	return 1;
}
int Laser::setupSharedMemory()
{
	SensorData = new SMObject(TEXT("Laser"), sizeof(SM_Laser));
	// SM Creation and seeking access
	SensorData->SMCreate();
	SensorData->SMAccess();
	laserTing = (SM_Laser*)SensorData->pData;
	// YOUR CODE HERE
	return 1;
}
int Laser::getData()
{
	Stream = Client->GetStream();
	ReadData = gcnew array<unsigned char>(5000);
	Stream->Read(ReadData, 0, ReadData->Length);
	data = Encoding::ASCII->GetString(ReadData);
	Console::WriteLine(data); //(remove when it works)
	// YOUR CODE HERE
	return 1;
}
int Laser::sendData()
{ // self written function much wow
	AskScan = gcnew String("sRN LMDscandata");
	SendData = gcnew array<unsigned char>(16);
	SendData = Encoding::ASCII->GetBytes(AskScan);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	// YOUR CODE HERE
	return 1;
}
int Laser::authData()
{
	Message = gcnew String("5261433\n");
	SendData = gcnew array<unsigned char>(16);
	SendData = Encoding::ASCII->GetBytes(Message);
	Stream->Write(SendData, 0, SendData->Length);
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
	for (int i = 0; i < STANDARD_LASER_LENGTH; i++) {
		laserTing->x[i] = RangeX[i];
		laserTing->y[i] = RangeY[i];
		// std::cout << RangeX[i] << std::endl;
		// print data to terminal 
		// std::cout << "did u see anything?" << std::endl;
		// Console::WriteLine("X data is {0,12:F3}", laserTing->x[i]);
		// Console::WriteLine("Y data is {0,12:F3}", laserTing->y[i]);
	}
	// YOUR CODE HERE
	return 1;
}
bool Laser::getShutdownFlag()
{
	return 1;
}

int Laser::shutdown(bool slip) 
{
	if (slip == 1) {
		exit(0);
	}
	return 1;
}
int Laser::setHeartbeat(bool heartbeat)
{
	if (heartbeat == 0) {
		// check that heartbeat has been set to 0 by processmanagement
		// if it has, then set it back to 1 
		heartbeat = 1;
	}
	else {
		std::cout << "process management is dieded" << std::endl;
		//exit(0);
	}
	// YOUR CODE HERE
	return 1;
}
Laser::~Laser()
{
	Stream->Close();
	Client->Close();
	// YOUR CODE HERE
}
