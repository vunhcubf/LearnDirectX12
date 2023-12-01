#pragma once
#include <vector>
#include <windows.h>

enum class EventType {
	KEYDOWN,
	KEYUP,
	RBUTTONDOWN,
	RBUTTONUP,
	MOUSEMOVE
};
class WindowsEventManager {
public:

	//ע��ķ�������ΪOn_KEYDOWN(��)������Ҫ��̬�������б� HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam
	//��Ҫc(��̬)�����ó�Ա����On_KEYDOWN
public:
	struct EventHandle {
		EventHandle(EventType Type, void* InstPtr, void(*Invoke)(void*, HWND, UINT, WPARAM, LPARAM)) :Type(Type), InstPtr(InstPtr), Invoke(Invoke) {};
	public:
		void* InstPtr;
		EventType Type;
		void(*Invoke)(void*, HWND, UINT, WPARAM, LPARAM);
	};
public:
	void RegisterEvent(EventType Type, void* InstPtr, void(*Invoke)(void*, HWND, UINT, WPARAM, LPARAM));
public:
	void TriggerEvents_KEYDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void TriggerEvents_KEYUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void TriggerEvents_RBUTTONDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void TriggerEvents_RBUTTONUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void TriggerEvents_MOUSEMOVE(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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


