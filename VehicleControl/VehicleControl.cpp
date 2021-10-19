#include"VehicleControl.h"


using namespace System;
using namespace Net;
using namespace Sockets;
using namespace Text;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Diagnostics;

int VehicleControl::connect(String^ hostName, int portNumber)
{
	Client = gcnew TcpClient(hostName, portNumber);
	//Configure client
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500; // wait 500 ms before reporting error
	Client->SendTimeout = 500;
	Client->ReceiveBufferSize = 1024; // when data comes, set aside 1kb of memory to store data
	Client->SendBufferSize = 1024;
	Stream = Client->GetStream();
	return 1;

}
int VehicleControl::setupSharedMemory()
{
	SensorData = new SMObject(TEXT("VehicleControl"), sizeof(SM_VehicleControl));
	// SM Creation and seeking access for LASER SHARED MEMORY
	SensorData->SMCreate();
	SensorData->SMAccess();
	vehicleTing = (SM_VehicleControl*)SensorData->pData;
	// SM Creation and seeking access for PROCESS MANAGEMENT SHARED MEMORY
	ProcessManagementData = new SMObject(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// SM Creation and seeking access
	ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMTing = (ProcessManagement*)ProcessManagementData->pData;
	return 1;

}
int VehicleControl::getData()
{
	return 1;

}
int VehicleControl::sendData()
{
	String^ SendSignal = gcnew String("# " + vehicleTing->Steering.ToString() + " " + vehicleTing->Speed.ToString() + " " + vehicleTing->flag.ToString() + " #");
	SendData = gcnew array<unsigned char>(16);
	SendData = Encoding::ASCII->GetBytes(SendSignal);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	return 1;

}

int VehicleControl::checkData()
{
	return 1;

}
int VehicleControl::sendDataToSharedMemory()
{
	return 1;

}
bool VehicleControl::getShutdownFlag()
{
	return 1;

}
int VehicleControl::setHeartbeat(bool heartbeat)
{
	return 1;

}
int VehicleControl::processData()
{
	return 1;

}
int VehicleControl::sendData()
{
	return 1;

}
int VehicleControl::authData()
{
	return 1;

}
int VehicleControl::manageHB()
{
	return 1;

}
VehicleControl::~VehicleControl()
{
	// YOUR CODE HERE
}