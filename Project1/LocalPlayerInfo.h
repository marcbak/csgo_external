#pragma once
#include <Windows.h>
#include "offsets.h"
#include "MemoryManager.h"

class LocalPlayerInfo {
public:
	LocalPlayerInfo(const MemoryManager& memory_manager) : client_state{0}, local_player{0}, game_module{0}, team{0},
		m_mem_manager(memory_manager)
	{
		const auto engine_address = reinterpret_cast<DWORD>(m_mem_manager.get_engine_module().modBaseAddr);
		game_module = (DWORD)m_mem_manager.get_game_module().modBaseAddr;
		local_player = m_mem_manager.read_memory<DWORD>(game_module + Offsets::local_player);
		team = m_mem_manager.read_memory<int>(local_player + Offsets::team);
		client_state = m_mem_manager.read_memory<DWORD>(engine_address + Offsets::client_state);
	}

	[[nodiscard]] D3DXMATRIX get_view_matrix() const noexcept
	{
		return m_mem_manager.read_memory<D3DXMATRIX>(game_module + Offsets::view_matrix);
	}

	[[nodiscard]] D3DXVECTOR3 get_local_aim_punch_angles() const noexcept 
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(local_player + Offsets::aim_punch_angles);

	}

	[[nodiscard]] D3DXVECTOR3 get_local_view_angles() const noexcept
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(client_state + Offsets::view_angles);
	
	}

	[[nodiscard]] D3DXVECTOR3 get_local_pos() const noexcept
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(local_player + Offsets::vec_origin);
	}

	[[nodiscard]] D3DXVECTOR3 get_local_view_offset() const noexcept
	{
		return m_mem_manager.read_memory<D3DXVECTOR3>(local_player + Offsets::vec_view_offset);
	}

	void set_local_view_angles(const D3DXVECTOR3& view_angles) const noexcept 
	{
		m_mem_manager.write_memory<decltype(view_angles)>(client_state + Offsets::client_state_view_angles, view_angles);
	}

	int get_my_id() const noexcept
	{
		for (int i = 1; i < 64; i++) {
			int player_id = m_mem_manager.read_memory<int>(game_module + Offsets::entity_list + (i - 1) * 0x10);
			if (player_id == local_player)
				return i;
		}
		return -1;
	}

	DWORD client_state;
	DWORD local_player;
	DWORD game_module;
	int team;

private:

	const MemoryManager& m_mem_manager;
};