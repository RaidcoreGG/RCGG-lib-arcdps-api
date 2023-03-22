#ifndef ARCDPS_H
#define ARCDPS_H

#include <stdint.h>
#include <Windows.h>

namespace ArcDPS
{
	/* arcdps export table */
	struct PluginDefinition
	{
		uintptr_t       Size;					/* size of exports table */
		uint32_t        Signature;				/* pick a number between 0 and uint32_t max that isn't used by other modules */
		uint32_t        ImGuiVersion;			/* set this to IMGUI_VERSION_NUM. if you don't use imgui, 18000 (as of 2021-02-02) */
		const char*     Name;					/* name string */
		const char*     Build;					/* build string */
		void*           WndProc;				/* wndproc callback, fn(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam), return assigned to umsg */
		void*           CombatCallback;			/* combat event callback, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
		void*           ImGuiRenderCallback;	/* ::present callback, before imgui::render, fn(uint32_t not_charsel_or_loading) */
		void*           UIOptions;				/* ::present callback, appending to the end of options window in arcdps, fn() */
		void*           CombatLocalCallback;	/* combat event callback like area but from chat log, fn(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) */
		void*           WndProcFiltered;		/* wndproc callback like wnd_nofilter above, input filered using modifiers */
		void*           UIWindows;				/* called once per 'window' option checkbox, with null at the end, non-zero return disables arcdps drawing that checkbox, fn(char* category) */
	};

	/* combat event - see evtc docs for details, revision param in combat cb is equivalent of revision byte header */
	struct CombatEvent
	{
		uint64_t        Time;
		uint64_t        SourceAgent;
		uint64_t        DestinationAgent;
		int32_t         Value;
		int32_t         BuffDamage;
		uint32_t        OverstackValue;
		uint32_t        SkillID;
		uint16_t        SourceInstanceID;
		uint16_t        DestinationInstanceID;
		uint16_t        SrcMasterInstanceID;
		uint16_t        DestinationMasterInstanceID;
		uint8_t         IFF;
		uint8_t         Buff;
		uint8_t         Result;
		uint8_t         IsActivation;
		uint8_t         IsBuffRemove;
		uint8_t         IsNinety;
		uint8_t         IsFifty;
		uint8_t         IsMoving;
		uint8_t         IsStatechange;
		uint8_t         IsFlanking;
		uint8_t         IsShields;
		uint8_t         IsOffcycle;
		uint8_t         PAD61;
		uint8_t         PAD62;
		uint8_t         PAD63;
		uint8_t         PAD64;
	};

	/* agent short */
	struct AgentShort
	{
		char*           Name;					/* agent name. may be null. valid only at time of event. utf8 */
		uintptr_t       ID;						/* agent unique identifier */
		uint32_t        Profession;				/* profession at time of event. refer to evtc notes for identification */
		uint32_t        Specialization;			/* elite spec at time of event. refer to evtc notes for identification */
		uint32_t        IsSelf;					/* 1 if self, 0 if not */
		uint16_t        Team;					/* sep21+ */
	};

	/* combat result (direct) */
	enum class CombatResult
	{
		Normal,									// strike was neither crit or glance
		Critical,								// strike was crit
		Glance,									// strike was glance
		Block,									// strike was blocked eg. mesmer shield 4
		Evade,									// strike was evaded, eg. dodge or mesmer sword 2
		Interrupt,								// strike interrupted something
		Absorbed,								// strike was "invulned" or absorbed eg. guardian elite
		Blind,									// strike missed
		KillingBlow,							// not a damage strike, target was killed by skill by
		Downed,									// not a damage strike, target was downed by skill by
		Breakbar,								// not a damage strike, target had value of breakbar damage dealt
		Activation,								// not a damage strike, on-activation event (src hit dst if damaging buff)
		UNKNOWN
	};

	/* combat state change */
	enum class CombatStateChange
	{
		None,									// not used - not this kind of event
		EnterCombat,							// src_agent entered combat, dst_agent is subgroup
		ExitCombat,								// src_agent left combat
		ChangeUp,								// src_agent is now alive
		ChangeDead,								// src_agent is now dead
		ChangeDown,								// src_agent is now downed
		Spawn,									// src_agent is now in game tracking range (not in realtime api)
		Despawn,								// src_agent is no longer being tracked (not in realtime api)
		HealthUpdate,							// src_agent is at health percent. dst_agent = percent * 10000 (eg. 99.5% will be 9950) (not in realtime api)
		LogStart,								// log start. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp. src_agent = 0x637261 (arcdps id) if evtc, species id if realtime
		LogEnd,									// log end. value = server unix timestamp **uint32**. buff_dmg = local unix timestamp. src_agent = 0x637261 (arcdps id) if evtc, species id if realtime
		WeaponSwap,								// src_agent swapped weapon set. dst_agent = current set id (0/1 water, 4/5 land)
		MaxHealthUpdate,						// src_agent has had it's maximum health changed. dst_agent = new max health (not in realtime api)
		PointOfView,							// src_agent is agent of "recording" player  (not in realtime api)
		Language,								// src_agent is text language  (not in realtime api)
		GameBuild,								// src_agent is game build  (not in realtime api)
		ShardID,								// src_agent is sever shard id  (not in realtime api)
		Reward,									// src_agent is self, dst_agent is reward id, value is reward type. these are the wiggly boxes that you get
		BuffInitial,							// combat event that will appear once per buff per agent on logging start (statechange==18, buff==18, normal cbtevent otherwise)
		Position,								// src_agent changed, cast float* p = (float*)&dst_agent, access as x/y/z (float[3]) (not in realtime api)
		Velocity,								// src_agent changed, cast float* v = (float*)&dst_agent, access as x/y/z (float[3]) (not in realtime api)
		Facing,									// src_agent changed, cast float* f = (float*)&dst_agent, access as x/y (float[2]) (not in realtime api)
		TeamChange,								// src_agent change, dst_agent new team id
		AttackTarget,							// src_agent is an attacktarget, dst_agent is the parent agent (gadget type), value is the current targetable state (not in realtime api)
		Targetable,								// dst_agent is new target-able state (0 = no, 1 = yes. default yes) (not in realtime api)
		MapID,									// src_agent is map id  (not in realtime api)
		REPLINFO,								// internal use, won't see anywhere
		StackActive,							// src_agent is agent with buff, dst_agent is the stackid marked active
		StackReset,								// src_agent is agent with buff, value is the duration to reset to (also marks inactive), pad61-pad64 buff instance id
		Guild,									// src_agent is agent, dst_agent through buff_dmg is 16 byte guid (client form, needs minor rearrange for api form)
		BuffInfo,								// is_flanking = probably invuln, is_shields = probably invert, is_offcycle = category, pad61 = stacking type, pad62 = probably resistance, src_master_instid = max stacks, overstack_value = duration cap (not in realtime)
		BuffFormula,							// (float*)&time[8]: type attr1 attr2 param1 param2 param3 trait_src trait_self, (float*)&src_instid[2] = buff_src buff_self, is_flanking = !npc, is_shields = !player, is_offcycle = break, overstack = value of type determined by pad61 (none/number/skill) (not in realtime, one per formula)
		SkillInfo,								// (float*)&time[4]: recharge range0 range1 tooltiptime (not in realtime)
		SkillTiming,							// src_agent = action, dst_agent = at millisecond (not in realtime, one per timing)
		BreakbarState,							// src_agent is agent, value is u16 game enum (active, recover, immune, none) (not in realtime api)
		BreakbarPercent,						// src_agent is agent, value is float with percent (not in realtime api)
		Error,									// (char*)&time[32]: error string (not in realtime api)
		Tag,									// src_agent is agent, value is the id (volatile, game build dependent) of the tag, buff will be non-zero if commander
		BarrierUpdate,							// src_agent is at barrier percent. dst_agent = percent * 10000 (eg. 99.5% will be 9950) (not in realtime api)
		StatReset,								// with arc ui stats reset (not in log), src_agent = npc id of active log
		Extension,								// cbtevent with statechange byte set to this
		APIDelayed,								// cbtevent with statechange byte set to this
		InstanceStart,							// src_agent is ms time at which the instance likely was started
		TickRate,								// every 500ms, src_agent = 25 - tickrate (when tickrate < 21)
		Last90BeforeDown,						// src_agent is enemy agent that went down, dst_agent is time in ms since last 90% (for downs contribution)
		Effect,									// src_agent is owner. dst_agent if at agent, else &value = float[3] xyz, &iff = float[2] xy orient, &pad61 = float[1] z orient, skillid = effectid. if is_flanking: duration = trackingid. &is_shields = uint16 duration. if effectid = 0, end &is_shields = trackingid (not in realtime api)
		IDToGUID,								// &src_agent = 16byte persistent content guid, overstack_value is of contentlocal enum, skillid is content id  (not in realtime api)
		UNKNOWN
	};

	struct UISettings
	{
		uint64_t IsHidden				: 1;
		uint64_t IsAlwaysDrawn			: 1;
		uint64_t IsModMoveLocked		: 1;
		uint64_t IsModClickLocked		: 1;
		uint64_t IsClosingWithEscape	: 1;
	};

	struct Modifiers
	{
		WORD Mod1;
		WORD Mod2;
		WORD ModMulti;
	};

	/* exports */
	typedef uint64_t(*Export_GetU64)();

	static void* LogFile;
	static void* LogArc;

	static PluginDefinitions PluginInfo;

	static void LogToFile(char* str) /* log to arcdps.log, thread/async safe */
	{
		size_t(*log)(char*) = (size_t(*)(char*))LogFile;
		if (log) (*log)(str);
		return;
	}
	static void LogToArc(char* str) /* log to extensions tab in arcdps log window, thread/async safe */
	{
		size_t(*log)(char*) = (size_t(*)(char*))LogArc;
		if (log) (*log)(str);
		return;
	}
	static void Log(char* str) /* log to arcdps.log and log window*/
	{
		LogToFile(str);
		LogToArc(str);
		return;
	}
}

#endif