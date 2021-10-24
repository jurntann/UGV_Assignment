
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <map>

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
	#include <unistd.h>
	#include <sys/time.h>
#elif defined(WIN32)
	#include <Windows.h>
	#include <tchar.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include <unistd.h>
	#include <sys/time.h>
#endif


#include "Camera.hpp"
#include "Ground.hpp"
#include "KeyManager.hpp"

#include "Shape.hpp"
#include "Vehicle.hpp"
#include "MyVehicle.hpp"

#include "Messages.hpp"
#include "HUD.hpp"

#include "SMStructs.h"
#include "SMObject.h"

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Diagnostics;

void display();
void reshape(int width, int height);
void idle();

void keydown(unsigned char key, int x, int y);
void keyup(unsigned char key, int x, int y);
void special_keydown(int keycode, int x, int y);
void special_keyup(int keycode, int x, int y);

void mouse(int button, int state, int x, int y);
void dragged(int x, int y);
void motion(int x, int y);

using namespace std;
using namespace scos;

// Used to store the previous mouse location so we
//   can calculate relative mouse movement.
int prev_mouse_x = -1;
int prev_mouse_y = -1;

// vehicle control related variables
MyVehicle * vehicle = NULL; // change to myVehicle
double speed = 0;
double steering = 0;

// Global variables
ProcessManagement* hb; 
SM_VehicleControl* car;
SM_Laser* leser;
int counter = 0;
int LIMIT = 5000; // ideally four times more than the other modules since the thread sleep is 4 times shorter than other modules

//int _tmain(int argc, _TCHAR* argv[]) {
int main(int argc, char ** argv) {
	// Declare an SM Object instance
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	hb = PMData;

	// Declare an SM Object instance
	SMObject VehicleObj(TEXT("VehicleControl"), sizeof(SM_VehicleControl));
	// SM Creation and seeking access
	VehicleObj.SMCreate();
	VehicleObj.SMAccess();
	SM_VehicleControl* VehicleData = (SM_VehicleControl*)VehicleObj.pData;
	car = VehicleData;

	// Declare an SM Object instance
	SMObject LaserObj(TEXT("Laser"), sizeof(SM_Laser));
	// SM Creation and seeking access
	LaserObj.SMCreate();
	LaserObj.SMAccess();
	SM_Laser* LaserData = (SM_Laser*)LaserObj.pData;
	leser = LaserData;

	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;

	glutInit(&argc, (char**)(argv));
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("MTRN3500 - GL");

	Camera::get()->setWindowDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glutKeyboardFunc(keydown);
	glutKeyboardUpFunc(keyup);
	glutSpecialFunc(special_keydown);
	glutSpecialUpFunc(special_keyup);

	glutMouseFunc(mouse);
	glutMotionFunc(dragged);
	glutPassiveMotionFunc(motion);

	// -------------------------------------------------------------------------
	// Please uncomment the following line of code and replace 'MyVehicle'
	//   with the name of the class you want to show as the current 
	//   custom vehicle.
	// -------------------------------------------------------------------------
	vehicle = new MyVehicle();

	glutMainLoop();

	if (vehicle != NULL) {
		delete vehicle;
	}
	return 0;
}


void display() {
	// -------------------------------------------------------------------------
	//  This method is the main draw routine. 
	// -------------------------------------------------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(Camera::get()->isPursuitMode() && vehicle != NULL) {
		double x = vehicle->getX(), y = vehicle->getY(), z = vehicle->getZ();
		double dx = cos(vehicle->getRotation() * 3.141592765 / 180.0);
		double dy = sin(vehicle->getRotation() * 3.141592765 / 180.0);
		Camera::get()->setDestPos(x + (-3 * dx), y + 7, z + (-3 * dy));
		Camera::get()->setDestDir(dx, -1, dy);
	}
	Camera::get()->updateLocation();
	Camera::get()->setLookAt();

	Ground::draw();
	
	// draw my vehicle
	if (vehicle != NULL) {
		vehicle->draw();
		for (int i = 0; i < 361; i++) {
			vehicle->drawLASER(leser->x[i], leser->y[i], 1.0);
			std::cout << "x: " << leser->x[i] << std::endl;
		}

	}


	// draw HUD
	HUD Headsup;
	Headsup.setupSharedMemory();
	Headsup.setVar();
	Headsup.DrawJorn();
	HUD::Draw();

	glutSwapBuffers();
};

void reshape(int width, int height) {

	Camera::get()->setWindowDimensions(width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
};

double getTime()
{
#if defined(WIN32)
	LARGE_INTEGER freqli;
	LARGE_INTEGER li;
	if(QueryPerformanceCounter(&li) && QueryPerformanceFrequency(&freqli)) {
		return double(li.QuadPart) / double(freqli.QuadPart);
	}
	else {
		static ULONGLONG start = GetTickCount64();
		return (GetTickCount64() - start) / 1000.0;
	}
#else
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec + (t.tv_usec / 1000000.0);
#endif
}

void idle() {
	if (KeyManager::get()->isAsciiKeyPressed('a')) {
		Camera::get()->strafeLeft();
	}

	if (KeyManager::get()->isAsciiKeyPressed('c')) {
		Camera::get()->strafeDown();
	}

	if (KeyManager::get()->isAsciiKeyPressed('d')) {
		Camera::get()->strafeRight();
	}

	if (KeyManager::get()->isAsciiKeyPressed('s')) {
		Camera::get()->moveBackward();
	}

	if (KeyManager::get()->isAsciiKeyPressed('w')) {
		Camera::get()->moveForward();
	}

	if (KeyManager::get()->isAsciiKeyPressed(' ')) {
		Camera::get()->strafeUp();
	}
	speed = 0;
	steering = 0;
	car->Speed = 0;
	car->Steering = 0;
	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_LEFT)) {
		steering = Vehicle::MAX_LEFT_STEERING_DEGS * -1;   
		// send shared memory over to SM_VehicleControl
		car->Speed = speed;
		car->Steering = steering;
		Console::WriteLine("sent1");
	}

	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
		steering = Vehicle::MAX_RIGHT_STEERING_DEGS * -1;
		// send shared memory over to SM_VehicleControl
		car->Speed = speed;
		car->Steering = steering;
		Console::WriteLine("sent2");
	}

	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_UP)) {
		speed = Vehicle::MAX_FORWARD_SPEED_MPS;
		// send shared memory over to SM_VehicleControl
		car->Speed = speed;
		car->Steering = steering;
		Console::WriteLine("sent3");
	}

	if (KeyManager::get()->isSpecialKeyPressed(GLUT_KEY_DOWN)) {
		speed = Vehicle::MAX_BACKWARD_SPEED_MPS;
		// send shared memory over to SM_VehicleControl
		car->Speed = speed;
		car->Steering = steering;
		Console::WriteLine("sent4");
	}
	// Display Laser Data
	//for (int i = 0; i < 361; i++) {
	//	vehicle->drawLASER(leser->x[i],leser->y[i],100.0);
	//}
	// Heartbeat stuff
	if (hb->Heartbeat.Flags.Display == 0) {
		// check that heartbeat has been set to 0 by processmanagement
		// if it has, then set it back to 1 
		hb->Heartbeat.Flags.Display = 1;
		Thread::Sleep(25);
	}
	else {
		// if the heartbeat is still 1 
		// this means processmanagement has dieded and so everything should stop
		//std::cout << "process management is dieded" << std::endl;
		counter++;
		if (counter > LIMIT) {
			exit(0);
		}
		Thread::Sleep(25);
		
	}

	if (hb->Shutdown.Status) {
		std::cout << "terminating program" << std::endl;
		exit(0);
	}


	const float sleep_time_between_frames_in_seconds = 0.025;

	static double previousTime = getTime();
	const double currTime = getTime();
	const double elapsedTime = currTime - previousTime;
	previousTime = currTime;

	// do a simulation step
	if (vehicle != NULL) {
		vehicle->update(speed, steering, elapsedTime);
	}

	display();

#ifdef _WIN32 
	Sleep(sleep_time_between_frames_in_seconds * 1000);
#else
	usleep(sleep_time_between_frames_in_seconds * 1e6);
#endif
};

void keydown(unsigned char key, int x, int y) {

	// keys that will be held down for extended periods of time will be handled
	//   in the idle function
	KeyManager::get()->asciiKeyPressed(key);

	// keys that react ocne when pressed rather than need to be held down
	//   can be handles normally, like this...
	switch (key) {
	case 27: // ESC key
		exit(0);
		break;      
	case '0':
		Camera::get()->jumpToOrigin();
		break;
	case 'p':
		Camera::get()->togglePursuitMode();
		break;
	}

};

void keyup(unsigned char key, int x, int y) {
	KeyManager::get()->asciiKeyReleased(key);
};

void special_keydown(int keycode, int x, int y) {

	KeyManager::get()->specialKeyPressed(keycode);

};

void special_keyup(int keycode, int x, int y) {  
	KeyManager::get()->specialKeyReleased(keycode);  
};

void mouse(int button, int state, int x, int y) {

};

void dragged(int x, int y) {

	if (prev_mouse_x >= 0) {

		int dx = x - prev_mouse_x;
		int dy = y - prev_mouse_y;

		Camera::get()->mouseRotateCamera(dx, dy);
	}

	prev_mouse_x = x;
	prev_mouse_y = y;
};

void motion(int x, int y) {

	prev_mouse_x = x;
	prev_mouse_y = y;
};


