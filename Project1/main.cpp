#include <iostream>
#include <Windows.h>
#include "offsets.h"
#include "Entity.h"
#include "MemoryManager.h"

struct vars {
	DWORD local_player;
	DWORD game_module;
	int team;
};

namespace {
void trigger_bot(MemoryManager& mem_manager, const vars& variables, const Offsets& offsets) 
{
	int crosshair_id = mem_manager.read_memory<int>(variables.local_player + offsets.crosshair);
	if (crosshair_id > 0) {
		DWORD entity = mem_manager.read_memory<DWORD>(variables.game_module + offsets.entity_list + ((crosshair_id - 1) * 0x10));
		int enemy_team = mem_manager.read_memory<int>(entity + offsets.team);
		int enemy_health = mem_manager.read_memory<int>(entity + offsets.health);
		
		if (enemy_team != variables.team && enemy_health > 0)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			Sleep(25);
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
	}	
}

void glowesp(Entity& entity, vars& variables) 
{
	for (int entity_id = 0; entity_id <= 32; entity_id++) {
		const auto entity_base = entity.get_entity_base(entity_id);
		const auto entity_team = entity.get_entity_team(entity_base);
		if (entity_team != variables.team) {
			entity.glow_esp(entity_id, entity_base, 255, 255, 255, 255);
		}
	}
}
}

void initialize_variables(vars& variables, MemoryManager& mem_manager, const Offsets& offsets)
{
	variables.game_module = (DWORD)mem_manager.get_module().modBaseAddr;
	variables.local_player = mem_manager.read_memory<DWORD>(variables.game_module + offsets.local_player);
	variables.team = mem_manager.read_memory<int>(variables.local_player + offsets.team);
}

int main()
{
	vars variables;
	MemoryManager mem_manager;
	Offsets offsets;
	mem_manager.attach("csgo.exe");
	const auto process_id = mem_manager.get_process_id();
	mem_manager.get_module("client_panorama.dll");
	initialize_variables(variables, mem_manager, offsets);
	Entity entity{offsets, mem_manager, variables.game_module};
	while (true) {
		trigger_bot(mem_manager, variables, offsets);
		glowesp(entity, variables);
	}
}