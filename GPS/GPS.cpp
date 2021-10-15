#include"GPS.h"
using namespace System::IO::Ports;
using namespace System;
#using <System.dll>

using namespace Net;
using namespace Sockets;
using namespace Text;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Diagnostics;

int GPS::connect(String^ hostName, int portNumber) 
{
	Port = gcnew SerialPort;
	PortName = gcnew String("COM1");
	SendData = gcnew array<unsigned char>(16);

	// CONFIGURATION
	Port->PortName = PortName;
	Port->BaudRate = 115200;
	Port->StopBits = StopBits::One;
	Port->DataBits = 8;
	Port->Parity = Parity::None;
	Port->Handshake = Handshake::None;
	// Set the read/write timeouts & buffer size
	Port->ReadTimeout = 500;
	Port->WriteTimeout = 500;
	Port->WriteBufferSize = 1024;

	return 1;
}
int GPS::setupSharedMemory() 
{
	SensorData = new SMObject(TEXT("GPS"), sizeof(SM_GPS));
	// SM Creation and seeking access for LASER SHARED MEMORY
	SensorData->SMCreate();
	SensorData->SMAccess();
	GPSTing = (SM_GPS*)SensorData->pData;
	// SM Creation and seeking access for PROCESS MANAGEMENT SHARED MEMORY
	ProcessManagementData = new SMObject(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// SM Creation and seeking access
	ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMTing = (ProcessManagement*)ProcessManagementData->pData;
	return 1;
}
int GPS::getData() 
{
	Port->Open();
	Port->Read(ReadData, 0, sizeof(SM_GPS));
	return 1;
}
int GPS::checkData() 
{
	// YOUR CODE HERE
	return 1;
}
int GPS::sendDataToSharedMemory() 
{
	unsigned int Header = 0;
	int i = 0;
	int Start; //Start of data
	do
	{
		data = ReadData[i++]; // need help 
		Header = ((Header << 8) || data);
	} while (Header != 0xaa44121c);
	Start = i - 4;
	unsigned char* BytePtr = nullptr;
	BytePtr = (unsigned char*) &GPSTing;
	for (int i = Start; i < Start + sizeof(SM_GPS); i++)
	{
		*(BytePtr++) = ReadData[i];
	}
	return 1;
}
bool GPS::getShutdownFlag() 
{
	if (PMTing->Shutdown.Status) {
		std::cout << "terminating program" << std::endl;
		exit(0);
	}
	return 1;
}
int GPS::setHeartbeat(bool heartbeat)
{
	// YOUR CODE HERE
	return 1;
}
int GPS::manageHB()
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
		std::cout << "process management is dieded: " << counter << std::endl;
		counter++;
		if (counter > LIMIT) {
			exit(0);
		}
	}
	return 1;
}
int GPS::setTimer()
{
	counter = 0;
	LIMIT = 10;
	return 1;
}
GPS::~GPS()
{
	// YOUR CODE HERE
}

unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}