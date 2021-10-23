#ifndef MTRN3500_HUD_H_
#define MTRN3500_HUD_H_

#ifdef __APPLE__
	#include <unistd.h>
#elif defined(WIN32)
	#include <Windows.h>
#else
	#include <unistd.h>
#endif

#include<smstructs.h>
#include <SMObject.h>

class HUD
{
public:
	static void RenderString(const char * str, int x, int y, void * font);
	static void DrawGauge(double x, double y, double r, double min, double max, double val, const char * label, const char * minLabel = NULL, const char * maxLabel = NULL);
	static void Draw();
// my own stuff, get it out if u need
	int setupSharedMemory();
	void setVar();
	static void DrawGPS(double x, double y, double r, double min, double max, double val, const char* label);
	void DrawJorn();
private: 
	SMObject* SensorData;
	SM_GPS* GPSTing2;
	double northing;
	double easting;
	double height;

};

#endif
