#pragma once

#include <UGV_module.h>
#include <smstructs.h>

ref class VehicleControl : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int setupSharedMemory() override;
	int getData() override;
	int checkData() override;
	int sendDataToSharedMemory() override;
	bool getShutdownFlag()override;
	int setHeartbeat(bool heartbeat) override;
	int processData() override;
	int sendData() override;
	int authData() override;
	int manageHB() override;
	~VehicleControl();

protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)
	array<unsigned char>^ SendData;
private:
	SM_VehicleControl* vehicleTing;
	ProcessManagement* PMTing;
	String^ Message; // characters that can be read
	String^ AskScan;
	String^ data;
	array<double>^ Range;
	array<double>^ RangeX;
	array<double>^ RangeY;
};