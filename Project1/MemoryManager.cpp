#include "MemoryManager.h"
#include <algorithm>
#include <iostream>

void MemoryManager::attach(const std::string_view& process_name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snapshot == INVALID_HANDLE_VALUE) {
		std::string exception("invalids snapshot for process" + std::string(process_name));
		throw exception;
	}
	PROCESSENTRY32 process_entry{};

	// THIS IS NEEDED BECAUSE WINDOWS API IS FROM FUCKING 1985
	process_entry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshot, &process_entry)) {
		std::string exception("No running process: " + std::string(process_name));
		CloseHandle(snapshot);
		throw exception;
	}

	if (std::string(process_entry.szExeFile) == process_name) {
		CloseHandle(snapshot);
		// (Notice the usage of PROCESS_ALL_ACCESS flag in order to grant read/write privileges)
		m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
		m_process_id = process_entry.th32ProcessID;
		return;
	}

	while(Process32Next(snapshot, &process_entry))
	{
		if (std::string(process_entry.szExeFile) != process_name) {
			m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
			m_process_id = process_entry.th32ProcessID;
		}
	}
	CloseHandle(snapshot);
}

void MemoryManager::get_module(const std::string_view& module_name)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_process_id);
	if (snapshot == INVALID_HANDLE_VALUE) {
		std::string exception("invalid snapshot for module" + std::string(module_name));
		throw exception;
	}

	MODULEENTRY32 module_entry{};
	module_entry.dwSize = sizeof(decltype(module_entry));
	if (!Module32First(snapshot, &module_entry)) {
		std::string exception("No process running for module: " + std::string(module_name));
		CloseHandle(snapshot);
		throw exception;
	}
	if (std::string(module_entry.szModule) == module_name) {
		CloseHandle(snapshot);
		m_module = module_entry;
		return;
	}
	while (Module32Next(snapshot, &module_entry))
	{
		if (std::string(module_entry.szModule) ==  module_name)
		{
			CloseHandle(snapshot);
			m_module = module_entry;
			return;
		}
	}

	CloseHandle(snapshot);
}
