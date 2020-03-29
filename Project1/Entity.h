#pragma once
#include "MemoryManager.h"
#include "offsets.h"
#include <Windows.h>
#include <d3dx9math.h>
namespace {
struct glow {
    float r = 1.f;
    float g = 0.f;
    float b = 0.f;
    float a = 1.f;

    char m_unk[4];
    float m_flUnk;
    float m_flBloomAmount = 1.f;
    float m_flUnk1;
    bool m_bRenderWhenOccluded = true;
    bool m_bRenderWhenUnoccluded = false;
    bool m_bFullBloomRender = false;
    char m_unk1;
    int m_nFullBloomStencilTestValue;
    int m_nGlowStyle = 0;
    int m_nSplitScreenSlot;
    int m_nNextFreeSlot;
};
}

class Entity {
public:
    Entity(MemoryManager& mem_manager, const DWORD game_module)
        : m_mem_manager(mem_manager)
        , m_game_module(game_module)
        , m_base_entity_list(game_module + Offsets::entity_list)
    {
    }

    DWORD get_entity_base(const int entity_index) const
    {
        return m_mem_manager.read_memory<DWORD>(m_base_entity_list + (entity_index * 0x10));
    }

    DWORD get_entity_team(const int entity_base) const
    {
        return m_mem_manager.read_memory<DWORD>(entity_base + Offsets::team);
    }

    int get_entity_hp(const DWORD entity_base) const
    {
        return m_mem_manager.read_memory<int>(entity_base + Offsets::health);
    }

    DWORD get_entity_bone_matrix(const DWORD entity_base) const
    {
        return m_mem_manager.read_memory<DWORD>(entity_base + Offsets::bone_matrix);
    }

    D3DXVECTOR3 get_entity_bone_pos(const DWORD entity_base, const int bone_id) const
    {
        struct Matrix3x4 {
            float Matrix[3][4];
        };

        Matrix3x4 bone_matrix = m_mem_manager.read_memory<Matrix3x4>(get_entity_bone_matrix(entity_base) + bone_id * 0x30);

        return {
            bone_matrix.Matrix[0][3],
            bone_matrix.Matrix[1][3],
            bone_matrix.Matrix[2][3]
        };
    }

    D3DXVECTOR3 get_entity_pos(const DWORD entity_base) const
    {
        return m_mem_manager.read_memory<D3DXVECTOR3>(entity_base + Offsets::vec_origin);
    }

    DWORD get_glow_object() const
    {
        return m_mem_manager.read_memory<DWORD>(m_game_module + Offsets::glow_object_manager);
    }

    int get_glow_index(const int entity_base) const
    {
        return m_mem_manager.read_memory<DWORD>(entity_base + Offsets::glow_index);
    }

    bool is_alive(const DWORD entity_base) const
    {
        const auto hp = get_entity_hp(entity_base);
        return hp <= 100 && hp > 0;
    }

    bool is_valid(const DWORD entity_base) const
    {
        const auto dormant = m_mem_manager.read_memory<bool>(entity_base + Offsets::dormant);
        return entity_base != NULL && is_alive(entity_base) && !dormant;
    }

    bool is_visible(const DWORD entity_base, const int player_id) const
    {
        return (m_mem_manager.read_memory<long>(entity_base + Offsets::spotted_by_mask) & (0x1 << (player_id - 1))) != 0;
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
    MemoryManager& m_mem_manager;
    DWORD m_game_module = 0;
    DWORD m_base_entity_list = 0;
};
