#include <iostream>
#include <Windows.h>
#include <thread>
#include "LocalPlayerInfo.h"
#include "offsets.h"
#include "Entity.h"
#include "MemoryManager.h"

namespace {
void initialize_variables(LocalPlayerInfo& variables, MemoryManager& mem_manager, const Offsets& offsets)
{
	variables.game_module = (DWORD)mem_manager.get_module().modBaseAddr;
	variables.local_player = mem_manager.read_memory<DWORD>(variables.game_module + offsets.local_player);
	variables.team = mem_manager.read_memory<int>(variables.local_player + offsets.team);
}

void trigger_bot(MemoryManager& mem_manager, const LocalPlayerInfo& variables, const Offsets& offsets) 
{
	while (true) {
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
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void glowesp(Entity& entity, LocalPlayerInfo& variables) 
{
	while (true) {
		for (int entity_id = 0; entity_id <= 32; entity_id++) {
			const auto entity_base = entity.get_entity_base(entity_id);
			const auto entity_team = entity.get_entity_team(entity_base);
			if (entity_team != variables.team) {
				entity.glow_esp(entity_id, entity_base, 255, 123, 140, 255);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}
}

int main()
{
	LocalPlayerInfo variables;
	MemoryManager mem_manager;
	Offsets offsets;
	mem_manager.attach("csgo.exe");
	const auto process_id = mem_manager.get_process_id();
	mem_manager.get_module("client_panorama.dll");
	initialize_variables(variables, mem_manager, offsets);
	Entity entity{offsets, mem_manager, variables.game_module};
	std::thread trigger_thread{ &trigger_bot, std::ref(mem_manager), std::ref(variables), std::ref(offsets)};
	std::thread glow_thread{ &glowesp, std::ref(entity), std::ref(variables)};
	trigger_thread.join();
	glow_thread.join();
	return 0;
}