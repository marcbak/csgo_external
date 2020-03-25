#include <iostream>
#include "MemoryManager.h"
#include "offsets.h"

struct vars {
	DWORD local_player;
	DWORD game_module;
	int team;
};


int main()
{
	vars variables;
	MemoryManager mem_manager;
	mem_manager.attach("csgo.exe");
	const auto process_id = mem_manager.get_process_id();
	mem_manager.get_module("client_panorama.dll");
	variables.game_module = (DWORD)mem_manager.get_module().hModule;
	variables.local_player = mem_manager.read_memory<DWORD>(variables.game_module + offset.local_player);
	while (true) {
		variables.team = mem_manager.read_memory<int>(variables.local_player + offset.team);
		int crosshair = mem_manager.read_memory<int>(variables.local_player + offset.crosshair);
		DWORD entity = mem_manager.read_memory<DWORD>(variables.game_module + offset.entity_list + ((crosshair - 1) * 0x10));
		int enemy_team = mem_manager.read_memory<int>(entity + offset.team);
		int enemy_health = mem_manager.read_memory<int>(entity + offset.health);
		std::cout << enemy_team << "\n";
	}
}