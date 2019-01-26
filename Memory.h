#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

class Memory
{
public:
	Memory()
	{
		hnd = NULL;
	}

	template<class x>
	x read(DWORD addr)
	{
		x y;
		ReadProcessMemory(hnd, (PVOID)addr, &y, sizeof(y), NULL);
		return y;
	}
	template<class x>
	x write(DWORD addr, x y)
	{
		WriteProcessMemory(hnd, (LPVOID)addr, &y, sizeof(y), NULL);
		return 0;
	}

	DWORD getProc(char * proc)
	{
		HANDLE snap = CreateToolhelp32Snapshot(0x00000002, 0);
		DWORD processID;
		PROCESSENTRY32 pEntry;
		pEntry.dwSize = sizeof(PROCESSENTRY32);

		if(snap==INVALID_HANDLE_VALUE)
		{
			std::cout<<"Failed to take snapsjot of process list\n";
			return false;
		}

		while(Process32Next(snap, &pEntry))
		{
			if(!strcmp(pEntry.szExeFile, proc))
			{
				processID = pEntry.th32ProcessID;
				std::cout<<"Process \t->"<<pEntry.szExeFile << "\nProcessId \t->"<<pEntry.th32ProcessID<<std::endl;
				hnd = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
			}
		}
		return processID;
	}

	uintptr_t getModule(const char* modName, DWORD procId)
	{
		HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
		MODULEENTRY32 mEntry;

		if(hModule == INVALID_HANDLE_VALUE)
		{
			std::cout<<"Failed snapshot\n";
			CloseHandle(hModule);
			return 0;
		}

		mEntry.dwSize = sizeof(mEntry);

		while (Module32Next(hModule, &mEntry))
		{
			if (!strcmp(mEntry.szModule, modName))
			{
				std::cout<< "Base addr \t->0x"<<std::hex<<std::uppercase<<(DWORD)mEntry.modBaseAddr<<std::endl;
				CloseHandle(hModule);
				return (DWORD)mEntry.hModule;
			}
		}
		std::cout<<"Couldn't find a module in snapshot of process"<<std::endl;
		CloseHandle(hModule);
		return 0;
	}
private:
	HANDLE hnd;
};