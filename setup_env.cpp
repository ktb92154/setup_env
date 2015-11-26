// setup_env.cpp : Defines the entry point for the console application.
//
/*
Goal: Create a .exe that starts up an array of executables.
Example: Click setup_env.exe -> which opens minesweeper.exe, RemoteDLL.exe, and others
Status: I'm able to click on the exe which spawns processes for minesweeper.exe and remoteDLL.exe.
        But it requires duplicating the code for every executable path.

Issues: Code is more clunky then I'd like, unable to get createProcess to read an array of exe paths.
        Ideally, input an array -> into method -> which spawns # of processes depending on array size.
*/

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include <iostream>       // std::cout
//#include <thread>         // std::thread
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

#include "Shlwapi.h"
#include "../tools_ktb/ConsoleColor.h"
#include "../tools_ktb/pubsub.h"
#include "cfg_data.h"
//#include "../tools_ktb/cfg_tool.h"

#include <boost/interprocess/windows_shared_memory.hpp>
#include <utility>
#include <boost/interprocess/mapped_region.hpp>
#include <cstring>
#include <cstdlib>
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstdio>    //std::remove
#include <vector>
using namespace std;
using namespace boost::interprocess;

size_t ExecuteProcess(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait)
{
	/* CreateProcess API initialization */
	STARTUPINFOW siStartupInfo = { 0 };
	PROCESS_INFORMATION piProcessInfo = { 0 };
	size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
	DWORD dwExitCode = 0;
	std::wstring sTempStr = L"";

	/* - NOTE - You should check here to see if the exe even exists */

	/* Add a space to the beginning of the Parameters */
	if (Parameters.size() != 0)
	{
		if (Parameters[0] != L' ')
		{
			Parameters.insert(0, L" ");
		}
	}

	/* The first parameter needs to be the exe itself */
	sTempStr = FullPathToExe;
	iPos = sTempStr.find_last_of(L"\\");
	sTempStr.erase(0, iPos + 1);
	Parameters = sTempStr.append(Parameters);

	/* CreateProcessW can modify Parameters thus we allocate needed memory */
	wchar_t * pwszParam = new wchar_t[Parameters.size() + 1];
	if (pwszParam == 0)
	{
		return 1;
	}
	const wchar_t* pchrTemp = Parameters.c_str();
	wcscpy_s(pwszParam, Parameters.size() + 1, pchrTemp);

	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);

	std::cout << green << "Main process: " << piProcessInfo.dwProcessId << white << std::endl;

	BOOL result = CreateProcessW(const_cast<LPCWSTR>(FullPathToExe.c_str()),
		pwszParam,      // Command line
		0,              // Process handle not inheritable
		0,              // Thread handle not inheritable
		false,          // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE, // Create separate terminal, otherwise use CREATE_DEFAULT_ERROR_MODE
							//CREATE_DEFAULT_ERROR_MODE, // No creation flags and no terminal
		0,              // Use parent's environment block
		0,              // Use parent's starting directory 
		&siStartupInfo, // Pointer to STARTUPINFO structure
		&piProcessInfo  // Pointer to PROCESS_INFORMATION structure
		);

	if(result)
	{
		/* Watch the process. */
		DWORD pid = GetProcessId(piProcessInfo.hProcess);
		//std::cout << green << "Created PID: " << pid << " for " << wstrTostr(FullPathToExe.c_str()) << white << std::endl;
		std::cout << green << "Created PID: " << pid << white << std::endl;

		//dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, INFINITE);
		dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000));


	}
	else
	{
		/* CreateProcess failed */
		iReturnVal = GetLastError();
		std::cout << red << "Create new PID failed: " << iReturnVal << white << std::endl;
	}

	/* Free memory */
	delete[]pwszParam;
	pwszParam = 0;

	/* Release handles */
	CloseHandle(piProcessInfo.hProcess);
	CloseHandle(piProcessInfo.hThread);

	return iReturnVal;
}

void print_quotient(float x, float y)
{
	std::cout << "The quotient is " << x / y << std::endl;
}

void print_string(std::string x)
{
	std::cout << "The string is " << x << std::endl;
}

int main()
{
	// Signal with no arguments and a void return value
	boost::signals2::signal<void(float, float)> sig2;
	sig2.connect(&print_quotient);

	sig2(5., 3.);
	// Initialize and read configuration file for dir paths
	cfg_data cfg("config.ini");
		
	std::wstring program = cfg.wget("Paths.Minesweeper"); // WORKS!

	ExecuteProcess(program, L"", 1); // WORKS
		
	std::wstring dll = cfg.wget("Paths.Dll"); // WORKS!
	ExecuteProcess(dll, L"", 1); // WORKS

		using namespace boost::interprocess;
		typedef std::pair<double, int> MyType;

		try {
			//An special shared memory where we can
			//construct objects associated with a name.
			//First remove any old shared memory of the same name, create 
			//the shared memory segment and initialize needed resources
			shared_memory_object::remove("MySharedMemory");
			managed_shared_memory segment
				//create       segment name    segment size
				(create_only, "MySharedMemory", 65536);

			//Create an object of MyType initialized to {0.0, 0}
			MyType *instance = segment.construct<MyType>
				("MyType instance")  //name of the object
				(0.0, 0);            //ctor first argument

									 //Create an array of 10 elements of MyType initialized to {0.0, 0}
			MyType *array = segment.construct<MyType>
				("MyType array")     //name of the object
				[10]                 //number of elements
			(0.0, 0);            //Same two ctor arguments for all objects

								 //Create an array of 3 elements of MyType initializing each one
								 //to a different value {0.0, 0}, {1.0, 1}, {2.0, 2}...
			float float_initializer[3] = { 0.0, 1.0, 2.0 };
			int   int_initializer[3] = { 0, 1, 2 };

			MyType *array_it = segment.construct_it<MyType>
				("MyType array from it")   //name of the object
				[3]                        //number of elements
			(&float_initializer[0]    //Iterator for the 1st ctor argument
				, &int_initializer[0]);    //Iterator for the 2nd ctor argument
		}
		catch (...) {
			shared_memory_object::remove("MySharedMemory");
			throw;
		}

	std::cout << red << "Press any key to exit terminal" << white << std::endl;
	std::cin.get();
	shared_memory_object::remove("MySharedMemory");
	return 0;
}

