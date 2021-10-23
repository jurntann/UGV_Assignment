#ifndef __MYVEHICLE_HPP__
#define __MYVEHICLE_HPP__


#include "Vehicle.hpp"
#include <smstructs.h>

class MyVehicle : public Vehicle
{
public:
	MyVehicle();
	virtual void draw();
// edited: added drawLASER and data variables 	
	//virtual void drawLASER();

private: 
	//SM_Laser* laserTing;
	/*
	array<double> XPoints;
	array<double> YPoints;
	*/
};

#endif