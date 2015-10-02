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
#include <thread>         // std::thread
#include "Shlwapi.h"
//#include "../Win32Project1/ConsoleColor.h"
#include "ConsoleColor/ConsoleColor.h"
#include "cfg_data.h"

using namespace std;

size_t ExecuteProcess(std::wstring FullPathToExe, std::wstring Parameters, size_t SecondsToWait)
{
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

	/* CreateProcess API initialization */
	STARTUPINFOW siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	/*
	PROCESS_INFORMATION has these members
	HANDLE hProcess;   // process handle
	HANDLE hThread;    // primary thread handle
	DWORD dwProcessId; // process PID
	DWORD dwThreadId;  // thread ID
	*/
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);
	std::cout << green << "Main process: " << piProcessInfo.dwProcessId << white << std::endl;
	if (CreateProcessW(const_cast<LPCWSTR>(FullPathToExe.c_str()),
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
		) != false)
	{
		/* Watch the process. */
		std::cout << green << "Created process: " << piProcessInfo.dwProcessId << ", succeeded." << white << std::endl;
		dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000));
		std::cout << green << "Ended process: " << piProcessInfo.hProcess << ", succeeded." << white << std::endl;
	}
	else
	{
		/* CreateProcess failed */
		iReturnVal = GetLastError();
		std::cout << red << "Create process failed: " << iReturnVal << white << std::endl;
	}

	/* Free memory */
	delete[]pwszParam;
	pwszParam = 0;

	/* Release handles */
	CloseHandle(piProcessInfo.hProcess);
	CloseHandle(piProcessInfo.hThread);

	return iReturnVal;
}


int main()
{

	// Initialize and read configuration file for dir paths
	cfg_data cfg("config.ini");

	LPWSTR paths[2] = { L"W:\\!Amazing_Tools\\RemoteDLLInjector\\RemoteDll32.exe", L"W:\\!Amazing_Tools\\RemoteDLLInjector\\Winmine__XP.exe"};
	LPWSTR * p;
	p = paths;
	//wchar_t exe_path[] = L"W:\\!Amazing_Tools\\RemoteDLLInjector\\Winmine__XP.exe";

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	std::wstring program = cfg.wget("Paths.Minesweeper"); // WORKS!

	ExecuteProcess(program, L"", 10); // WORKS
    //ExecuteProcess(L"C:\\Python27\\python.exe", L"", 100); // WORKS
		
	 // use L"" prefix for wide chars
	//swprintf_s(injector_path, 10, L"%d", pid); // use L"" prefix for wide chars
	//MessageBox(NULL, injector_path, L"TEST", MB_OK);
	
	// **** Works to combine two wchar_t ****
	//wchar_t* c;
	//c = wcscat(injector_path, dll_path);
	// **** end works ****

	std::wstring dll = cfg.wget("Paths.Dll2"); // WORKS!
	ExecuteProcess(dll, L"", 10); // WORKS
	
	std::cin.get();

	return 0;
}

