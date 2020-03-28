#pragma once
#include "fwd_decls.h"

class TriggerBot
{
public:
	TriggerBot(const Entity& entity, const LocalPlayerInfo& local_playerinfo, const MemoryManager& mem_manager) :
		m_entity(entity), m_local_playerinfo(local_playerinfo), m_mem_manager(mem_manager)
	{}

	void do_trigger() const noexcept;
private:
	const Entity& m_entity;
	const LocalPlayerInfo& m_local_playerinfo;
	const MemoryManager& m_mem_manager;
};

