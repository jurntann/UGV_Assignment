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
	// YOUR CODE HERE
	return 1;
}
int Laser::sendData(String^ message)
{
	Stream = Client->GetStream();
	SendData = gcnew array<unsigned char>(16);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	// YOUR CODE HERE
	return 1;
}
int Laser::processData() { // self written function much wow


	return 1;
}
int Laser::checkData()
{
	// YOUR CODE HERE
	return 1;
}
int Laser::sendDataToSharedMemory()
{
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
	// YOUR CODE HERE
}
