#pragma once

#include <Windows.h>

struct offsets
{
	DWORD local_player = 0xD30B84;
	DWORD force_attack = 0x3175F98;
	DWORD entity_list = 0x4D449F4;
	DWORD crosshair = 0xB3D4;
	DWORD team = 0xF4;
	DWORD health = 0x100;
} offset;