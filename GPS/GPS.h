#pragma once

#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class GPS : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override
	{
		// YOUR CODE HERE
		return 1;
	}
	int setupSharedMemory() override
	{
		// YOUR CODE HERE
		return 1;
	}
	int getData() override
	{
		// YOUR CODE HERE
		return 1;
	}
	int checkData() override
	{
		// YOUR CODE HERE
		return 1;
	}
	int sendDataToSharedMemory() override
	{
		// YOUR CODE HERE
		return 1;
	}
	bool getShutdownFlag() override
	{
		// YOUR CODE HERE
		return 1;
	}
	int setHeartbeat(bool heartbeat) override
	{
		// YOUR CODE HERE
		return 1;
	}
	~GPS()
	{
		// YOUR CODE HERE
	}

protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)

};


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