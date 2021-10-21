#pragma once

#include <UGV_module.h>
#include <smstructs.h>

ref class VehicleControl : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;
	int setupSharedMemory() override;
	int getData() override;
	int sendData() override;
	int checkData() override;
	int sendDataToSharedMemory() override;
	bool getShutdownFlag()override;
	int setHeartbeat(bool heartbeat) override;
	int processData() override;
	int manageHB() override;
	int setTimer() override;
	int authData() override;
	~VehicleControl();

protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)
	array<unsigned char>^ SendData;
	array<unsigned char>^ ReadData;
	SM_VehicleControl* vehicleTing;
	ProcessManagement* PMTing;
	String^ SendSignal;
	int counter;
	int LIMIT;
	String^ Message;
	int flag;
private:

};