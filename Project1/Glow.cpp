#include <thread>
#include <chrono>
#include "Glow.h"

#include "Entity.h"
#include "LocalPlayerInfo.h"
#include "MemoryManager.h"

void Glow::do_glow() const noexcept
{
    while (true) {
        for (int entity_id = 0; entity_id <= MAX_PLAYERS; entity_id++) {
            const auto entity_base = m_entity.get_entity_base(entity_id);
            const auto entity_team = m_entity.get_entity_team(entity_base);
            if (entity_team != m_local_playerinfo.get_team()) {
                m_entity.glow_esp(entity_id, entity_base, 255, 255, 255, 255);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}