#pragma once
#include <algorithm>
#include <Windows.h>
#include "offsets.h"
#include "MemoryManager.h"

class LocalPlayerInfo {
public:
	LocalPlayerInfo(const MemoryManager& memory_manager) : m_client_state{ 0 }, m_local_player{ 0 }, m_game_module{ 0 }, m_team{ 0 },
		m_mem_manager(memory_manager)
	{
		const auto engine_address = reinterpret_cast<DWORD>(m_mem_manager.get_engine_module().modBaseAddr);
		m_game_module = (DWORD)m_mem_manager.get_game_module().modBaseAddr;
		m_local_player = m_mem_manager.read_memory<DWORD>(m_game_module + Offsets::local_player);
		m_team = m_mem_manager.read_memory<int>(m_local_player + Offsets::team);
		m_client_state = m_mem_manager.read_memory<DWORD>(engine_address + Offsets::client_state);
	}

	[[nodiscard]] D3DXMATRIX get_view_matrix() const noexcept
	{
		return m_mem_manager.read_memory<D3DXMATRIX>(m_game_module + Offsets::view_matrix);
	}

	[[nodiscard]] D3DXVECTOR3 get_local_aim_punch_angles() const noexcept
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(m_local_player + Offsets::aim_punch_angles);

	}

	[[nodiscard]] D3DXVECTOR3 get_local_view_angles() const noexcept
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(m_client_state + Offsets::view_angles);

	}

	[[nodiscard]] D3DXVECTOR3 get_local_pos() const noexcept
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(m_local_player + Offsets::vec_origin);
	}

	[[nodiscard]] D3DXVECTOR3 get_local_view_offset() const noexcept
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(m_local_player + Offsets::vec_view_offset);
	}

	void set_local_view_angles(const D3DXVECTOR3& view_angles) const noexcept
	{

		m_mem_manager.write_memory<decltype(view_angles)>(m_client_state + Offsets::client_state_view_angles, view_angles);
	}

	int get_my_id() const noexcept
	{
		for (int i = 1; i < 64; i++) {
			int player_id = m_mem_manager.read_memory<int>(m_game_module + Offsets::entity_list + (i - 1) * 0x10);
			if (player_id == m_local_player)
				return i;
		}
		return -1;
	}

	DWORD get_client_state() const noexcept
	{
		return m_client_state;
	}
	 
	DWORD get_local_player() const noexcept 
	{
		return m_local_player;
	}

	DWORD get_game_module() const noexcept
	{
		return m_game_module;
	}
	
	int get_team() const noexcept
	{
		return m_team;
	}

private:
	DWORD m_client_state;
	DWORD m_local_player;
	DWORD m_game_module;
	int m_team;
	const MemoryManager& m_mem_manager;
};