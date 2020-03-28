#define NOMINMAX
#include <iostream>
#include <limits>
#include <d3dx9math.h>
#include <Windows.h>
#include <thread>
#include "Entity.h"
#include "image.h"
#include "LocalPlayerInfo.h"
#include "MemoryManager.h"
#include "offsets.h"

namespace {
	constexpr int MAX_PLAYERS = 32;

	void trigger_bot(const MemoryManager& mem_manager, const LocalPlayerInfo& variables)
	{
		while (true) {
			int crosshair_id = mem_manager.read_memory<int>(variables.local_player + Offsets::crosshair);
			if (crosshair_id > 0) {
				DWORD entity = mem_manager.read_memory<DWORD>(variables.game_module + Offsets::entity_list + ((crosshair_id - 1) * 0x10));
				int enemy_team = mem_manager.read_memory<int>(entity + Offsets::team);
				int enemy_health = mem_manager.read_memory<int>(entity + Offsets::health);

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

	void glowesp(const Entity& entity, const LocalPlayerInfo& variables)
	{
		while (true) {
			for (int entity_id = 0; entity_id <= MAX_PLAYERS; entity_id++) {
				const auto entity_base = entity.get_entity_base(entity_id);
				const auto entity_team = entity.get_entity_team(entity_base);
				if (entity_team != variables.team) {
					entity.glow_esp(entity_id, entity_base, 255, 123, 140, 255);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	}

	DWORD get_closest_alive_player(const Entity& entity, const LocalPlayerInfo& local_playerinfo)
	{
		D3DXVECTOR3 head_world_to_screen{};
		DWORD closest_player{};
		auto closest_distance = std::numeric_limits<double>::max();

		for (int entity_id = 1; entity_id < MAX_PLAYERS; entity_id++)
		{
			const auto base_entity = entity.get_entity_base(entity_id);
			// TODO: Check for immunity
			if ((entity.get_entity_team(base_entity) != local_playerinfo.team) && entity.is_valid(base_entity)) {
				Image::world_to_screen(local_playerinfo, entity.get_entity_bone_pos(base_entity, 8),
					head_world_to_screen, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
				const auto x = GetSystemMetrics(SM_CXSCREEN) / 2 - head_world_to_screen.x;
				const auto y = GetSystemMetrics(SM_CYSCREEN) / 2 - head_world_to_screen.y;
				const double distance = sqrt(x * x + y * y);

				if (distance < closest_distance) {
					closest_distance = distance;
					closest_player = base_entity;
				}
			}
		}
		return closest_player;
	}
	
	void aimbot(const Entity& entity, const LocalPlayerInfo& local_playerinfo, const DWORD player_to_aim_at, const int player_id) {
		if (player_to_aim_at == 0) {
			return;
		}

		if (entity.is_visible(player_to_aim_at, player_id) && GetAsyncKeyState(VK_LBUTTON))
		{
			const auto local_pos_with_offset = local_playerinfo.get_local_pos() + local_playerinfo.get_local_view_offset();
			auto aim_angles = Image::calculate_angle(local_pos_with_offset, entity.get_entity_bone_pos(player_to_aim_at, 8));
			constexpr auto RECOIL_COMPENSATION = 2.0;
			aim_angles -= local_playerinfo.get_local_aim_punch_angles();
			local_playerinfo.set_local_view_angles(aim_angles);
		}
	}
	
	void do_aim(const Entity& entity, const LocalPlayerInfo& local_playerinfo)
	{
		const auto player_id = local_playerinfo.get_my_id();
		while (true)
		{
			const auto player_to_aim_at = get_closest_alive_player(entity, local_playerinfo);
			aimbot(entity, local_playerinfo, player_to_aim_at, player_id);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}
}

int main()
{
	MemoryManager mem_manager;
	LocalPlayerInfo variables{ mem_manager };
	Entity entity{ mem_manager, variables.game_module };

	mem_manager.attach("csgo.exe");
	std::thread trigger_thread{ &trigger_bot, std::cref(mem_manager), std::cref(variables) };
	std::thread glow_thread{ &glowesp, std::cref(entity), std::cref(variables) };
	std::thread aimbot_thread{ &do_aim, std::cref(entity), std::cref(variables) };
	trigger_thread.join();
	glow_thread.join();
	aimbot_thread.join();

	return 0;
	mem_manager.get_module("client_panorama.dll");
}