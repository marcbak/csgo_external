#pragma once
#include <Windows.h>
#include "MemoryManager.h"
#include "offsets.h"

class Entity
{
public:
	Entity(const Offsets& offsets, MemoryManager& mem_manager, const DWORD game_module) : m_offsets(offsets), m_mem_manager(mem_manager), 
		m_game_module(game_module), m_base_entity_list(game_module + m_offsets.entity_list)
	{
	}

	DWORD get_entity_base(const int entity_index) const
	{
		return m_mem_manager.read_memory<DWORD>(m_base_entity_list + (entity_index * 0x10));
	}

	DWORD get_entity_team(const int entity_base) const
	{
		return m_mem_manager.read_memory<DWORD>(entity_base + m_offsets.team);
	}

	DWORD get_glow_object() const 
	{
		return m_mem_manager.read_memory<DWORD>(m_game_module + m_offsets.glow_object_manager);
	}

	int get_glow_index(const int entity_base) const
	{
		return m_mem_manager.read_memory<DWORD>(entity_base + m_offsets.glow_index);
	}

	void glow_esp(const int entity_index, const int entity_base, const float r, const float g, const float b, const float alpha) const
	{
		const auto glow_object = get_glow_object();
		const auto glow_index = get_glow_index(entity_base);
		m_mem_manager.write_memory<float>((glow_object + ((glow_index * 0x38) + 0x4)), r / 255);
		m_mem_manager.write_memory<float>((glow_object + ((glow_index * 0x38) + 0x8)), g / 255);
		m_mem_manager.write_memory<float>((glow_object + ((glow_index * 0x38) + 0xC)), b / 255);
		m_mem_manager.write_memory<float>((glow_object + ((glow_index * 0x38) + 0x10)), alpha / 255);
		m_mem_manager.write_memory<bool>((glow_object + ((glow_index * 0x38) + 0x24)), true);
		m_mem_manager.write_memory<bool>((glow_object + ((glow_index * 0x38) + 0x25)), false);
		
	}

private:
	const Offsets& m_offsets;
	MemoryManager& m_mem_manager;
	DWORD m_game_module = 0;
	DWORD m_base_entity_list = 0;

};

