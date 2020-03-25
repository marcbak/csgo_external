#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>
#include <vector>

class MemoryManager
{
public:
	MemoryManager() : m_module(), m_process_id(), m_process_handle(INVALID_HANDLE_VALUE)
	{
	}
	~MemoryManager() = default;

	void attach(const std::string_view& process_name);

	void get_module(const std::string_view& module_name);


	template<typename T>
	T read_memory(DWORD addr) const noexcept {
		T value;
		ReadProcessMemory(m_process_handle, reinterpret_cast<LPVOID>(addr), value, sizeof(T), NULL);
		return value;
	}

	template<typename T>
	bool write_memory(DWORD addr, const T& value) const noexcept
	{
		return WriteProcessMemory(m_process_handle, addr, value, sizeof(T), NULL) == true;
	}

	[[nodiscard]] const auto get_process_id() const noexcept
	{
		return m_process_id; 
	}

	[[nodiscard]]  const auto get_process_handle() const noexcept
	{
		return m_process_handle;
	} 

	MODULEENTRY32 get_module() const noexcept 
	{
		return m_module;
	}

private:
	MODULEENTRY32 m_module;
	DWORD m_process_id;
	HANDLE m_process_handle;
};

