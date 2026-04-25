#include<iostream>
#include<windows.h>
#include<string>
#include<vector>
#include <tlhelp32.h>
#include <cwchar>
void SetProcessState(DWORD targetPID, bool freeze);
DWORD GetPIDByName(const wchar_t* processName);
void rad(std::vector<std::wstring> list, bool freeze);
int main() {
	std::vector<std::wstring> BlackList;
	std::wstring input;
	while (true) {
		std::wcout << L"Enter name of blacklist file(if you need to stop write\"stop\"): \n";
		std::wcin >> input;
		if (input == L"stop") {
			break;
		}
		else { BlackList.push_back(input); }

	}
	std::wstring game;
	std::wcout << L"Enter name of game:\n";
	std::wcin >> game;
	DWORD gamePID;
	HWND activeWin = nullptr;
	DWORD activeWinPID = 0;
	bool isbgpaused = false;
	while (true) {
		gamePID = GetPIDByName(game.c_str());
		activeWin = GetForegroundWindow();
		GetWindowThreadProcessId(activeWin, &activeWinPID);
		if (activeWinPID == gamePID && isbgpaused == false) {
			isbgpaused = true;
			rad(BlackList, isbgpaused);
		}
		else if (activeWinPID != gamePID && isbgpaused == true) {
			isbgpaused = false;
			rad(BlackList, isbgpaused);
		}
		Sleep(500);
	}
	return 0;
}
void SetProcessState(DWORD targetPID, bool freeze) {
	HANDLE Snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	THREADENTRY32 te32;
	te32.dwSize = sizeof(THREADENTRY32);
	if (Thread32First(Snap, &te32)) {
		do {
			if (te32.th32OwnerProcessID == targetPID) {
				HANDLE hThreat = OpenThread(THREAD_SUSPEND_RESUME, false, te32.th32ThreadID);
				if (hThreat != NULL) {
					if (freeze == true) {
						SuspendThread(hThreat);
					}
					else {
						ResumeThread(hThreat);

					}CloseHandle(hThreat);
				}
			}
		} while (Thread32Next(Snap, &te32));
		{ CloseHandle(Snap); }
	}
}
DWORD GetPIDByName(const wchar_t* processName) {
	HANDLE process1 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pr32;
	pr32.dwSize = sizeof(PROCESSENTRY32);
	DWORD nPID = 0;
	if (Process32First(process1, &pr32)) {
		do {
			if (_wcsicmp(pr32.szExeFile, processName) == 0) {
				nPID = pr32.th32ProcessID;
				break;
			}
		} while (Process32Next(process1, &pr32));
	}
	CloseHandle(process1);
	return nPID;
}
void rad(std::vector<std::wstring> list, bool freeze) {
	HANDLE all = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 al32;
	al32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(all, &al32)) {
		do {
			for (int i = 0; i < list.size(); i++) {
				if (_wcsicmp(al32.szExeFile, list[i].c_str()) == 0) {
					SetProcessState(al32.th32ProcessID, freeze);
				}
			}
		} while (Process32Next(all, &al32));

	}
	CloseHandle(all);
}