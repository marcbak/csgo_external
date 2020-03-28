#include <Windows.h>
#pragma once

struct Offsets {
	static constexpr DWORD aim_punch_angles = 0x302C;
	static constexpr DWORD bone_matrix = 0x26A8;
	static constexpr DWORD client_state = 0x00589D9C;
	static constexpr DWORD client_state_view_angles = 0x4D88;
	static constexpr DWORD crosshair = 0xB3D4;
	static constexpr DWORD dormant = 0xED;
	static constexpr DWORD entity_list = 0x4D44A24;
	static constexpr DWORD force_attack = 0x3175F98;
	static constexpr DWORD glow_index = 0xA428;
	static constexpr DWORD glow_object_manager = 0x528C8D8;
	static constexpr DWORD health = 0x100;
	static constexpr DWORD local_player = 0xD30B94;
	static constexpr DWORD spotted = 0x93D;
	static constexpr DWORD spotted_by_mask = 0x980;
	static constexpr DWORD team = 0xF4;;
	static constexpr DWORD vec_origin= 0x138;
	static constexpr DWORD vec_view_offset = 0x108;
	static constexpr DWORD view_angles = 0x00004D88;
	static constexpr DWORD view_matrix = 0x04D36364;
};