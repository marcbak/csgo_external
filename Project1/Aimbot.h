#pragma once
#include "fwd_decls.h"
#include <d3dx9math.h>
#include <Windows.h>
#include <thread>

class Aimbot {
public:
    Aimbot(const Entity& entity, const LocalPlayerInfo& local_playerinfo, const MemoryManager& mem_manager)
        : m_should_humanize(true)
        , m_entity(entity)
        , m_local_playerinfo(local_playerinfo)
        , m_mem_manager(mem_manager)
    {
    }

    void do_aim() const noexcept;

private:
    void aim_at(const DWORD player_to_aim_at, const int player_id) const noexcept;
    D3DXVECTOR3 get_aiming_angle(const DWORD player_to_aim_at) const noexcept;
    DWORD get_closest_alive_player() const noexcept;

private: 
    bool m_should_humanize = true;
    const Entity& m_entity;
    const LocalPlayerInfo& m_local_playerinfo;
    const MemoryManager& m_mem_manager;
};