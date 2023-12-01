#include "EventManager.h"

void WindowsEventManager::RegisterEvent(EventType Type, void* InstPtr, void(*Invoke)(void*, HWND, UINT, WPARAM, LPARAM)) {
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

void WindowsEventManager::TriggerEvents_KEYDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	for (auto it = List_KEYDOWN.begin(); it != List_KEYDOWN.end(); it++) {
		(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
	}
}
void WindowsEventManager::TriggerEvents_KEYUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	for (auto it = List_KEYUP.begin(); it != List_KEYUP.end(); it++) {
		(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
	}
}
void WindowsEventManager::TriggerEvents_RBUTTONDOWN(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	for (auto it = List_RBUTTONDOWN.begin(); it != List_RBUTTONDOWN.end(); it++) {
		(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
	}
}
void WindowsEventManager::TriggerEvents_RBUTTONUP(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	for (auto it = List_RBUTTONUP.begin(); it != List_RBUTTONUP.end(); it++) {
		(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
	}
}
void WindowsEventManager::TriggerEvents_MOUSEMOVE(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	for (auto it = List_MOUSEMOVE.begin(); it != List_MOUSEMOVE.end(); it++) {
		(*(it->Invoke))(it->InstPtr, hWnd, msg, wParam, lParam);
	}
}