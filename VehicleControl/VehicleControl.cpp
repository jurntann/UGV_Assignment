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
	// only if the flag is up (1), then this function will send data to the robot to move it
	if (vehicleTing->flag == 1) {
		String^ SendSignal = gcnew String("# " + vehicleTing->Steering.ToString() + " " + vehicleTing->Speed.ToString() + " " + vehicleTing->flag.ToString() + " #");
		SendData = gcnew array<unsigned char>(100); // to confirm size
		SendData = Encoding::ASCII->GetBytes(SendSignal);
		Stream->WriteByte(0x02);
		Stream->Write(SendData, 0, SendData->Length);
		Stream->WriteByte(0x03);
	}
	return 1;

}
int VehicleControl::authData()
{
	Message = gcnew String("5261433\n");
	SendData = gcnew array<unsigned char>(16);
	SendData = Encoding::ASCII->GetBytes(Message);
	Stream->Write(SendData, 0, SendData->Length);
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
	if (PMTing->Shutdown.Status) {
		std::cout << "terminating program" << std::endl;
		exit(0);
	}
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

int VehicleControl::manageHB()
{
	// timer waiting not implemented yet
	// method: a counter int and limit int as private members of class
	// int counter; int LIMIT; 
	// a setter function to set counter to 0 
	if (PMTing->Heartbeat.Flags.VehicleControl == 0) {
		// check that heartbeat has been set to 0 by processmanagement
		// if it has, then set it back to 1 
		PMTing->Heartbeat.Flags.VehicleControl = 1;
		counter = 0;
	}
	else {
		// if the heartbeat is still 1 
		// this means processmanagement has dieded and so everything should stop
		std::cout << "process management is dieded: " << counter << std::endl;
		counter++;
		if (counter > LIMIT) {
			exit(0);
		}
	}
	return 1;

}

int VehicleControl::setTimer()
{
	counter = 0;
	LIMIT = 10;
	return 1;
}
VehicleControl::~VehicleControl()
{
	Stream->Close();
	Client->Close();
}