#pragma once

#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class GPS : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int setupSharedMemory() override;
	int getData() override;
	int sendData() override;
	int checkData() override;
	int sendDataToSharedMemory() override;
	bool getShutdownFlag() override;
	int setHeartbeat(bool heartbeat) override;
	int manageHB() override;
	int setTimer() override;
	~GPS();

protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
private: 
	SM_GPS* GPSTing;
	ProcessManagement* PMTing;
	String^ Message; // characters that can be read
	String^ data;
	String^ bruh;
	array<double>^ Range;
	array<double>^ RangeX;
	array<double>^ RangeY;
	int counter;
	int LIMIT;

	// GPS Particulars
	unsigned int Checksum;
	double Northing;
	double Easting;
	double Height;
	String^ AskScan;
	unsigned long valueCRC_CALC;

};


