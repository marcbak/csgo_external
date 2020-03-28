#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>
#include <vector>

class MemoryManager
{
public:
	MemoryManager() : m_engine_module(), m_game_module(), m_process_id(), m_process_handle(INVALID_HANDLE_VALUE)
	{
		attach("csgo.exe");
		m_game_module   = get_module("client_panorama.dll");
		m_engine_module = get_module("engine.dll");
	}
	~MemoryManager() = default;

	void attach(const std::string_view& process_name);

	MODULEENTRY32 get_module(const std::string_view& module_name);


	template<typename T>
	T read_memory(DWORD addr) const noexcept {
		T value;
		ReadProcessMemory(m_process_handle, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), NULL);
		return value;
	}

	template<typename T>
	bool write_memory(DWORD addr, const T& value) const noexcept
	{
		const auto result = WriteProcessMemory(m_process_handle, reinterpret_cast<LPVOID>(addr), &value, sizeof(T), NULL);
		return result;
	}

	[[nodiscard]] const auto get_process_id() const noexcept
	{
		return m_process_id; 
	}

	[[nodiscard]]  const auto get_process_handle() const noexcept
	{
		return m_process_handle;
	} 

	MODULEENTRY32 get_engine_module() const noexcept
	{
		return m_engine_module;
	}

	MODULEENTRY32 get_game_module() const noexcept 
	{
		return m_game_module;
	}

private:
	MODULEENTRY32 m_game_module;
	MODULEENTRY32 m_engine_module;
	DWORD m_process_id;
	HANDLE m_process_handle;
};

