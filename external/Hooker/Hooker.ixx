module;
#include <stdexcept>
#include <deque>
#include <unordered_set>
#include <Windows.h>
#include <detours/detours.h>

export module Hooker;



export
{
	class Hooker
	{
	public:
		virtual void setHook() = 0;
		virtual void unHook() = 0;

		virtual ~Hooker() = default;
	};

	class DetoursHooker //: public Hooker
	{
		std::deque<std::pair<PVOID, PVOID>> hook_deque_;
		std::deque<std::pair<PVOID, PVOID>> unhook_deque_;

	public:
		//e. DetourAttach((LPVOID)&main_ori, main_mod);
		DetoursHooker() = default;

		void endeque(const std::deque<std::pair<PVOID, PVOID>>& raw_to_mod)
		{
			for (auto& e : raw_to_mod)
			{
				hook_deque_.emplace_back(e);
			}
		}

		void endeque(const std::pair<PVOID, PVOID>& raw_to_mod)
		{
			hook_deque_.emplace_back(raw_to_mod);
		}

		void setHook()
		{
			//检查是否有重复的
//			unordered_set<pair<PVOID, PVOID>> tmp;
//			for (auto& e : hook_deque_)
//			{
//				if (tmp.emplace(e).second == false)
//				{
//					throw invalid_argument("hook the same func for two times?");
//				}
//			}
			//
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			while (!hook_deque_.empty())
			{
				auto& raw_to_mod = hook_deque_.front();
				DetourAttach((PVOID*)raw_to_mod.first, raw_to_mod.second);
				unhook_deque_.push_back(raw_to_mod);
				hook_deque_.pop_front();
			}
			PVOID* error = nullptr;
			DetourTransactionCommitEx(&error);
		}

		void unHook()
		{
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			while (!unhook_deque_.empty())
			{
				auto& raw_to_mod = unhook_deque_.front();
				DetourDetach((PVOID*)raw_to_mod.first, raw_to_mod.second);
				unhook_deque_.pop_front();
			}
			PVOID* error = nullptr;
			DetourTransactionCommitEx(&error);
		}
	};

}
