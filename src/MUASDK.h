#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>

#include <UI/CMenu.h>
#include <UI/CMenuFactory.h>

namespace MUASDK
{
	//Stolen from GTAIV.EFLC.FusionFix, all credits to the creators: https://github.com/ThirteenAG/GTAIV.EFLC.FusionFix/blob/master/source/common.ixx
	template<typename... Args>
	class Event : public std::function<void(Args...)>
	{
	public:
		using std::function<void(Args...)>::function;

	private:
		std::list<std::function<void(Args...)>> handlers;

	public:
		auto operator+=(std::function<void(Args...)>&& handler) -> std::function<void()>
		{
			auto it = handlers.insert(handlers.end(), std::move(handler));
			return [this, it]() { handlers.erase(it); };
		}

		void ExecuteAll(Args... args) const
		{
			if (!handlers.empty())
			{
				for (auto& handler : handlers)
				{
					handler(args...);
				}
			}
		}
	};

	Event<>& OnInitEvent();
	Event<>& OnShutdownEvent();
	Event<>& OnRunFrameEvent();
	Event<>& OnEndDrawEvent();
	Event<>& OnHandleEventsEvent();
	Event<>& OnGameInitEvent();
	Event<>& OnGameShutdownEvent();
	Event<>& OnGameRunFrameEvent();
	Event<>& OnUICommandsInitEvent();
	
	typedef std::function<CMenu* ()> TMenuFactory;
	typedef std::map<std::string, TMenuFactory> TMenuFactoryMap;
	typedef TMenuFactoryMap::iterator TMenuFactoryMapIt;
	
	extern TMenuFactoryMap menuFactoryMap;
	
	template<class T>
	void RegisterMenu(const char* name)
	{
		menuFactoryMap[name] = []() -> CMenu*
		{
			return new (menuFactoryMenus.alloc_value_raw()) T();
		};
	}

	void InstallHooks();
}
