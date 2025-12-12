#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "IDebugMenuMgr.h"
#include "IDebugWindow.h"

typedef std::vector<IDebugWindow*> TDebugWindowVector;
typedef TDebugWindowVector::iterator TDebugWindowVectorIterator;
typedef TDebugWindowVector::const_iterator TDebugWindowVectorConstIterator;

typedef std::unordered_map<std::string, IDebugWindow*> TDebugWindowMap;
typedef TDebugWindowMap::iterator TDebugWindowMapIterator;
typedef TDebugWindowMap::const_iterator TDebugWindowMapConstIterator;

class CDebugMenuMgr : public IDebugMenuMgr
{
private:
	TDebugWindowVector mWindows;
	TDebugWindowMap mWindowsLookup;
	bool mEnabled : 1;

	void RegisterAllWindows();

public://TODO: Make private
	template<class T>
	T* RegisterWindow(const std::string& name)
	{
		TDebugWindowMapIterator it = mWindowsLookup.find(name);
		
		if (it == mWindowsLookup.end())
		{
			T* window = new T();

			mWindows.push_back(window);
			mWindowsLookup[name] = window;
			return window;
		}
		else
		{
			return dynamic_cast<T*>(it->second);
		}
	}

	template<class T>
	T* GetWindow(const std::string& name)
	{		
		TDebugWindowMapIterator it = mWindowsLookup.find(name);
		
		if (mWindowsLookup.find(name) != mWindowsLookup.end())
		{
			return dynamic_cast<T*>(it->second);
		}
		else
		{
			return NULL;
		}
	}

public:
	CDebugMenuMgr();

	virtual ~CDebugMenuMgr();
	virtual void Initialize();
	virtual void Shutdown();
	virtual void Draw();
	virtual bool GetEnabled() const;
	virtual void SetEnabled(bool enabled);
	virtual bool WantCaptureMouse() const;
	virtual bool WantCaptureKeyboard() const;
};
