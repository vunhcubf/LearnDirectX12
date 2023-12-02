#pragma once
#include <vector>
#include <windows.h>

class WindowsEventManager {
public:
	
	//注册的方法名称为On_KEYDOWN(例)，不需要静态，参数列表 HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
	//需要c(静态)，调用成员函数On_KEYDOWN
public:
	struct EventHandle {
		EventHandle(EventType Type, void* InstPtr, void(*Invoke)(void*,HWND,UINT,WPARAM,LPARAM)) :Type(Type), InstPtr(InstPtr), Invoke(Invoke) {};
	public:
		void* InstPtr;
		EventType Type;
		void(*Invoke)(void*,HWND,UINT,WPARAM,LPARAM);
	};
public:
	void RegisterEvent(EventType Type, void* InstPtr, void(*Invoke)(void*,HWND,UINT,WPARAM,LPARAM)) {
		switch (Type)
		{
		case EventType::KEYDOWN:
			List_KEYDOWN.push_back(EventHandle(Type, InstPtr, Invoke));
			break;
		case EventType::KEYUP:
			List_KEYUP.push_back(EventHandle(Type, InstPtr, Invoke));
			break;
		case EventType::RBUTTONDOWN:
			List_RBUTTONDOWN.push_back(EventHandle(Type, InstPtr, Invoke));
			break;
		case EventType::RBUTTONUP:
			List_RBUTTONUP.push_back(EventHandle(Type, InstPtr, Invoke));
			break;
		case EventType::MOUSEMOVE:
			List_MOUSEMOVE.push_back(EventHandle(Type, InstPtr, Invoke));
			break;
		default:
			break;
		}
	}
public:
	void TriggerEvents_KEYDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		for (auto it = List_KEYDOWN.begin(); it != List_KEYDOWN.end(); it++) {
			(*(it->Invoke))(it->InstPtr,hWnd,msg,wParam,lParam);
		}
	}
	void TriggerEvents_KEYUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		for (auto it = List_KEYUP.begin(); it != List_KEYUP.end(); it++) {
			(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
		}
	}
	void TriggerEvents_RBUTTONDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		for (auto it = List_RBUTTONDOWN.begin(); it != List_RBUTTONDOWN.end(); it++) {
			(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
		}
	}
	void TriggerEvents_RBUTTONUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		for (auto it = List_RBUTTONUP.begin(); it != List_RBUTTONUP.end(); it++) {
			(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
		}
	}
	void TriggerEvents_MOUSEMOVE(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		for (auto it = List_MOUSEMOVE.begin(); it != List_MOUSEMOVE.end(); it++) {
			(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
		}
	}
private:
	std::vector<EventHandle> List_KEYDOWN = {};
	std::vector<EventHandle> List_KEYUP = {};
	std::vector<EventHandle> List_RBUTTONDOWN = {};
	std::vector<EventHandle> List_RBUTTONUP = {};
	std::vector<EventHandle> List_MOUSEMOVE = {};
public:
	~WindowsEventManager() {
		List_KEYDOWN.clear();
		List_KEYDOWN.shrink_to_fit();
		List_KEYUP.clear();
		List_KEYUP.shrink_to_fit();
		List_RBUTTONDOWN.clear();
		List_RBUTTONDOWN.shrink_to_fit();
		List_RBUTTONUP.clear();
		List_RBUTTONUP.shrink_to_fit();
		List_MOUSEMOVE.clear();
		List_MOUSEMOVE.shrink_to_fit();
	}
};
enum class EventType {
	KEYDOWN,
	KEYUP,
	RBUTTONDOWN,
	RBUTTONUP,
	MOUSEMOVE
};