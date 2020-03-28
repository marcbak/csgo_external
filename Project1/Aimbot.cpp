#define NOMINMAX
#include "Aimbot.h"
#include "Entity.h"
#include "LocalPlayerInfo.h"
#include "MemoryManager.h"
#include "image.h"
#include <iostream>
namespace {
void normalize_angle(D3DXVECTOR3& current_angles) noexcept
{
    if (current_angles.x < -180.0f)
        current_angles.x += 360.0f;
    if (current_angles.x > 180.0f)
        current_angles.x -= 360.0f;
    if (current_angles.y < -180.0f)
        current_angles.y += 360.0f;
    if (current_angles.y > 180.0f)
        current_angles.y -= 360.0f;
    if (current_angles.x > 89.0f && current_angles.x <= 180.0f) {
        current_angles.x = 89.0f;
    }
}

D3DXVECTOR3 humanize_angle(const D3DXVECTOR3& aim_angles, const D3DXVECTOR3& current_angles)
{
    constexpr auto SMOOTH_VALUE = 10.0;
    auto angle_delta = aim_angles - current_angles;
    normalize_angle(angle_delta);
    auto new_smoothed_angle = current_angles + angle_delta / SMOOTH_VALUE;
    return new_smoothed_angle;
}

}
void Aimbot::do_aim() const noexcept
{
    const auto player_id = m_local_playerinfo.get_my_id();
    while (true) {
        const auto player_to_aim_at = get_closest_alive_player();
        aim_at(player_to_aim_at, player_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void Aimbot::aim_at(const DWORD player_to_aim_at, const int player_id) const noexcept
{
    if (player_to_aim_at == 0) {
        return;
    }

    if (!(m_entity.is_visible(player_to_aim_at, player_id) && GetAsyncKeyState(VK_LBUTTON))) {
        return;
    }
    m_local_playerinfo.set_local_view_angles(get_aiming_angle(player_to_aim_at));
}

D3DXVECTOR3 Aimbot::get_aiming_angle(const DWORD player_to_aim_at) const noexcept
{
    constexpr auto RECOIL_COMPENSATION = 2.5;
    const auto local_pos_with_offset = m_local_playerinfo.get_local_pos() + m_local_playerinfo.get_local_view_offset();
    auto aim_angles = Image::calculate_angle(local_pos_with_offset, m_entity.get_entity_bone_pos(player_to_aim_at, 8));
    auto current_angles = m_local_playerinfo.get_local_view_angles();
    aim_angles -= m_local_playerinfo.get_local_aim_punch_angles() * RECOIL_COMPENSATION;
    if (!m_should_humanize) {
        return aim_angles;
    }
    return humanize_angle(aim_angles, current_angles);
}

DWORD Aimbot::get_closest_alive_player() const noexcept
{
    D3DXVECTOR3 head_world_to_screen {};
    DWORD closest_player {};
    auto closest_distance = std::numeric_limits<double>::max();

    for (int entity_id = 1; entity_id < MAX_PLAYERS; entity_id++) {
        const auto base_entity = m_entity.get_entity_base(entity_id);
        // TODO: Check for immunity
        if ((m_entity.get_entity_team(base_entity) != m_local_playerinfo.get_team()) && m_entity.is_valid(base_entity)) {

            Image::world_to_screen(m_local_playerinfo, m_entity.get_entity_bone_pos(base_entity, 8),
                head_world_to_screen, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
            const auto x = GetSystemMetrics(SM_CXSCREEN) / 2 - head_world_to_screen.x;
            const auto y = GetSystemMetrics(SM_CYSCREEN) / 2 - head_world_to_screen.y;
            const double distance = sqrtf(x * x + y * y);

            if (distance < closest_distance) {
                closest_distance = distance;
                closest_player = base_entity;
            }
        }
    }
    return closest_player;
}