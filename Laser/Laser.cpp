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
	// SM Creation and seeking access for LASER SHARED MEMORY
	SensorData->SMCreate();
	SensorData->SMAccess();
	laserTing = (SM_Laser*)SensorData->pData;
	// SM Creation and seeking access for PROCESS MANAGEMENT SHARED MEMORY
	ProcessManagementData = new SMObject(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// SM Creation and seeking access
	ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMTing = (ProcessManagement*)ProcessManagementData->pData;
	return 1;
}
int Laser::getData()
{
	Stream = Client->GetStream();
	ReadData = gcnew array<unsigned char>(5000);
	Stream->Read(ReadData, 0, ReadData->Length);
	data = Encoding::ASCII->GetString(ReadData);
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
	int IntData;
	try
	{
		IntData = System::Convert::ToInt32(data, 16);
	}
	catch (FormatException^)
	{
		Console::WriteLine("Bad String  " + data);
		IntData = 0;
		Console::ReadKey();
	}
	Console::WriteLine("{0,12:D3} ", IntData);
	//if (IntData == 0 || IntData != 0) {
		double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
		double Resolution = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
		int NumRanges = System::Convert::ToInt32(StringArray[25], 16);
		Range = gcnew array<double>(NumRanges);
		RangeX = gcnew array<double>(NumRanges);
		RangeY = gcnew array<double>(NumRanges);
		for (int i = 0; i < NumRanges; i++) {
			Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
			RangeX[i] = Range[i] * sin(i * Resolution / 180 * 3.14) / 1000.0;
			RangeY[i] = -Range[i] * cos(i * Resolution / 180 * 3.14) / 1000.0;
		}
	//}
	return 1;
}
int Laser::checkData()
{
	array<wchar_t>^ Space = { ' ' };
	array<String^>^ StringArray = data->Split(Space);
	ScanMessage = gcnew String("LMDscandata");
	if (StringArray[2] == ScanMessage) {
		return 1;
	}
	else {
		return 0;
	}
}
int Laser::sendDataToSharedMemory()
{
	for (int i = 0; i < STANDARD_LASER_LENGTH; i++) {
		laserTing->x[i] = RangeX[i];
		laserTing->y[i] = RangeY[i];
		// std::cout << RangeX[i] << std::endl;
		// print data to terminal 
		// std::cout << "did u see anything?" << std::endl;
		 Console::WriteLine("X data is {0,12:F3}", laserTing->x[i]);
		 Console::WriteLine("Y data is {0,12:F3}", laserTing->y[i]);
	}
	// YOUR CODE HERE
	return 1;
}
bool Laser::getShutdownFlag()
{
	if (PMTing->Shutdown.Status) {
		std::cout << "terminating program" << std::endl;
		exit(0);
	}
	return 1;
}

int Laser::setHeartbeat(bool heartbeat)
{
	return 1;
}

int Laser::manageHB()
{
	// timer waiting not implemented yet
	// method: a counter int and limit int as private members of class
	// int counter; int LIMIT; 
	// a setter function to set counter to 0 
	if (PMTing->Heartbeat.Flags.Laser == 0) {
		// check that heartbeat has been set to 0 by processmanagement
		// if it has, then set it back to 1 
		PMTing->Heartbeat.Flags.Laser = 1;
		counter = 0;
	}
	else {
		// if the heartbeat is still 1 
		// this means processmanagement has dieded and so everything should stop
		std::cout << "process management is dieded: "<< counter << std::endl;
		counter++ ;
		if (counter > LIMIT) {
			exit(0);
		}
	}
	return 1;
}

int Laser::setTimer()
{
	counter = 0;
	LIMIT = 500;
	return 1;
}
Laser::~Laser()
{
	Stream->Close();
	Client->Close();
	// YOUR CODE HERE
}
