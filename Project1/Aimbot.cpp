#define NOMINMAX
#include "Aimbot.h"
#include "Entity.h"
#include "LocalPlayerInfo.h"
#include "MemoryManager.h"
#include "image.h"
#include <iostream>
namespace {
void clamp_angles(D3DXVECTOR3& current_angles) noexcept
{
    while (current_angles.y < -180.0f) {
        current_angles.y += 360.0f;
    }
    while (current_angles.y  > 180.0f) {
        current_angles.y -= 360.0f;
    }

    if (current_angles.x > 89.0f) {
        current_angles.x = 89.0f;
    }
    if (current_angles.x < -89.0f) {
        current_angles.x = -89.0f;
    }

    while (current_angles.y < -180.f) {
        current_angles.y += 360.0f;
    }
    while (current_angles.y > 180.0f) {
        current_angles.y -= 360.0f;
    }
    current_angles.z = 0.0f;
}

D3DXVECTOR3 humanize_angle(const D3DXVECTOR3& aim_angles, const D3DXVECTOR3& current_angles)
{
    constexpr float SMOOTHING_VALUE = 15.0f;
    const auto delta = aim_angles - current_angles;
    auto smoothed_angles = current_angles + delta / SMOOTHING_VALUE;
    clamp_angles(smoothed_angles);
    return smoothed_angles;
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
    constexpr auto RECOIL_COMPENSATION = 2.0;
    const auto local_eye_pos = m_local_playerinfo.get_local_pos() + m_local_playerinfo.get_local_view_offset();
    const auto enemy_position = m_entity.get_entity_bone_pos(player_to_aim_at, 8);
    auto aim_angles = enemy_position - local_eye_pos;
    Image::vector_angles(aim_angles, aim_angles);
    aim_angles -= m_local_playerinfo.get_local_aim_punch_angles() * RECOIL_COMPENSATION;
    if (!m_should_humanize) {
        return aim_angles;
    }
    return humanize_angle(aim_angles, m_local_playerinfo.get_local_view_angles());
}

DWORD Aimbot::get_closest_alive_player() const noexcept
{
    constexpr float INITIAL_MINIMAL_FOV = 30.0;
    float closest_distance = INITIAL_MINIMAL_FOV;
    DWORD closest_player {};
    const auto local_pos_with_offset = m_local_playerinfo.get_local_pos() + m_local_playerinfo.get_local_view_offset();
    auto current_angles = m_local_playerinfo.get_local_view_angles();

    for (int entity_id = 1; entity_id <= MAX_PLAYERS; entity_id++) {
        const auto base_entity = m_entity.get_entity_base(entity_id);
        // TODO: Check for immunity
        if ((m_entity.get_entity_team(base_entity) != m_local_playerinfo.get_team()) && m_entity.is_valid(base_entity)) {
            auto angle_to_enemy = Image::calculate_angle(local_pos_with_offset, m_entity.get_entity_bone_pos(base_entity, 8));
            auto aim_angles = angle_to_enemy - current_angles;
            clamp_angles(aim_angles);
            const auto distance_to_entity = sqrtf(aim_angles.x * aim_angles.x + aim_angles.y * aim_angles.y);
            if (distance_to_entity < closest_distance) {
                closest_distance = distance_to_entity;
                closest_player = base_entity;
            }
        }
    }
    return closest_player;
}