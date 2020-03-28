#include "TriggerBot.h"
#include <chrono>
#include <thread>

#include "Entity.h"
#include "LocalPlayerInfo.h"
#include "MemoryManager.h"

void TriggerBot::do_trigger() const noexcept
{
    while (true) {
        if (!GetAsyncKeyState(VK_LMENU)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }
        int crosshair_id = m_mem_manager.read_memory<int>(m_local_playerinfo.get_local_player() + Offsets::crosshair);
        if (crosshair_id > 0) {
            DWORD entity = m_mem_manager.read_memory<DWORD>(m_local_playerinfo.get_game_module() + Offsets::entity_list + ((crosshair_id - 1) * 0x10));
            int enemy_team = m_mem_manager.read_memory<int>(entity + Offsets::team);
            int enemy_health = m_mem_manager.read_memory<int>(entity + Offsets::health);

            if (enemy_team != m_local_playerinfo.get_team() && enemy_health > 0) {
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
