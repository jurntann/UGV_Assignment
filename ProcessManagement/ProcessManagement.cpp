
#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <array>
#include "SMStructs.h"
#include "SMObject.h"
#include <direct.h>
#include <string>

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;
using namespace System::Diagnostics;

#define NUM_UNITS 5

bool IsProcessRunning(const char* processName);
void StartProcesses();

//defining start up sequence
TCHAR Units[10][20] = //
{
	TEXT("GPS.exe"),
	TEXT("Camera.exe"),
	TEXT("Display.exe"),
	TEXT("Laser.exe"),
	TEXT("VehicleControl.exe")
};

int main()
{
	// Declare an SM Object instance for PROCESS MANAGEMENT
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	// SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	//start all 5 modules
	//StartProcesses();
	std::string cwd("\0", FILENAME_MAX + 1);
	std::string currDir = getcwd(&cwd[0], cwd.capacity());
	currDir.append("\\..\\Debug");
	String^ dirString = gcnew String(currDir.c_str());
	array<String^>^ ModuleList = gcnew array<String^>{"GPS_z5261433", "Camera", "Display", "Laser_1", "VehicleControl_1"};
	array<int>^ Critical = gcnew array<int>(ModuleList->Length) { 0, 1, 0, 1, 1 };
	array<Process^>^ ProcessList = gcnew array<Process^>(ModuleList->Length);
	for (int i = 0; i < ModuleList->Length; i++) {
		//if (Process::GetProcessesByName(ModuleList[i])->Length == 0) {
			ProcessList[i] = gcnew Process;
			ProcessList[i]->StartInfo->WorkingDirectory = dirString;
			ProcessList[i]->StartInfo->FileName = ModuleList[i];
			ProcessList[i]->Start();
			Console::WriteLine("The process " + ModuleList[i] + ".exe started");
			//}
	
	}

	// limit before action is taken
	int LIMIT = 10;
	// array of counters, if any of the elements reach three then action is taken 
	std::array<int, 5> counters = { 0,0,0,0,0 }; //Left to right represents processes gps to laser
	while (!_kbhit()) {
		// reset strike everytime to iterate through counters
		int strike = 0;
		int process = 0;
		for (int mask = 0b00000001; mask < 0b00100000 ; mask <<= 1) {	
			
			// PMData->Heartbeat.Status
			std::cout << "mask: " << mask << std::endl;
			if ((PMData->Heartbeat.Status & mask) == mask){ 
				// perform bitwise AND (&) operation on each bit, if result is 1 then process alive
				//set each heartbeat to 0 again if the heartbeat is 1 
				PMData->Heartbeat.Status = PMData->Heartbeat.Status ^ mask; // XOR will set the matching 1 and 1 bits to 0, but leave the rest
				std::cout << "heartbeat changed for process" << std::endl;
			} else {
				// if bit is 0, then AND operation will return 0 
				std::cout << "strike: " << counters[strike] << std::endl;
				counters[strike]++;
				if (counters[strike] > LIMIT) {
					// if critical process(another bitwise mask and operation, shutdown all if not then just restart process [TODO]
					// for now default is to shutdown 
					if ((PMData->Heartbeat.Status & CRITICALMASK) !=  CRITICALMASK){
						std::cout << "CRITICAL PROGRAM: NOW SHUTTING DOWN OPERATIONS" << std::endl;
						PMData->Shutdown.Status = 0xFF;
						exit(0);
					} else {

						std::cout << "NON-CRITICAL PROGRAM: attempting to restart" << std::endl;
						// check if process has exited, if it has, start it again
						// if not, kill it and start again
						if (ProcessList[process]->HasExited) {
							ProcessList[process]->Start();
						} else {
							ProcessList[process]->Kill();
							ProcessList[process]->Start();
						}
					}
				}
				process++;
			}
			strike++;
			
		}
		Thread::Sleep(1000); // allow time for keypress for manual shutdown
	}
	PMData->Shutdown.Status = 0xFF;
	Console::ReadKey();
	return 0;
}


//Is process running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp((const char *)entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}


void StartProcesses()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];
	for (int i = 0; i < NUM_UNITS; i++)
	{
		if (!IsProcessRunning((const char *)Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));
			
			if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
			}
			std::cout << "Started: " << Units[i] << std::endl;
			Sleep(100);
		}
	}
}

