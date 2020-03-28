#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <thread>

#include "Aimbot.h"
#include "Entity.h"
#include "Glow.h"
#include "LocalPlayerInfo.h"
#include "MemoryManager.h"
#include "TriggerBot.h"


int main()
{
    MemoryManager mem_manager;
    LocalPlayerInfo local_playerinfo { mem_manager };
    Entity entity { mem_manager, local_playerinfo.get_game_module() };
    mem_manager.attach("csgo.exe");
    Aimbot aimbot { entity, local_playerinfo, mem_manager };
    TriggerBot trigger_bot{ entity, local_playerinfo, mem_manager };
    Glow glow{ entity, local_playerinfo, mem_manager };
    std::thread trigger_thread { &TriggerBot::do_trigger, trigger_bot };
    std::thread glow_thread { &Glow::do_glow, glow };
    std::thread aimbot_thread { &Aimbot::do_aim, aimbot };
    trigger_thread.join();
    glow_thread.join();
    aimbot_thread.join();

    return 0;
    mem_manager.get_module("client_panorama.dll");
}