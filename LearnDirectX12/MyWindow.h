#pragma once
#include <Windows.h>
#include "MyException.h"
#include <string>
#include "KeyBoard.h"
#include "Mouse.h"
#include "resource.h"

class MyWindow
{
public:
	class Exception :public MyException{
		public:
			Exception(int line, const char* file,HRESULT hr) noexcept;
			const char* what() const noexcept override;
			virtual const char* GetType() const noexcept;
			static std::string TranslateerrorCode(HRESULT hr) noexcept;
			HRESULT GetErrorCode() const noexcept;
			std::string GetErrorString() const noexcept;
		private:
			HRESULT hr;
	};

	MyWindow(HINSTANCE hInstance,int width,int height) noexcept :hInstance(hInstance), Width(width),Height(height){};
	void SetHWND(HWND hWnd) {
		this->hWnd = hWnd;
	}
	MyWindow(const MyWindow& Wnd) noexcept {
		this->hInstance = Wnd.hInstance;
		this->hWnd = Wnd.hWnd;
		this->Height = Wnd.Height;
		this->Width = Wnd.Width;
	}
	MyWindow(const MyWindow&& Wnd) noexcept{
		this->hInstance = Wnd.hInstance;
		this->hWnd = Wnd.hWnd;
		this->Height = Wnd.Height;
		this->Width = Wnd.Width;
	}
	KeyBoard keyBoard;
	Mouse mouse;
	std::wstring WindowTitle;
	bool IsActive = false;
	int Width;
	int Height;
	HINSTANCE hInstance;
	WNDPROC pWinProc{};
	HWND hWnd=nullptr;
	void SetWindowTitle(std::wstring& str) {SetWindowTextW(this->hWnd, str.c_str());}
	void SetWindowTitle(std::wstring&& str) { SetWindowTextW(this->hWnd, str.c_str()); }
	void SetWindowTitle(std::string& str) { SetWindowTextA(this->hWnd, str.c_str()); }
	void SetWindowTitle(std::string&& str) { SetWindowTextA(this->hWnd, str.c_str()); }
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgIntermedia(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static MyWindow* RegisterForm(HINSTANCE hInstance, int width, int height, std::wstring pClassName, std::wstring lpWndName);
	static MyWindow* RegisterForm(HINSTANCE hInstance, int width, int height, std::wstring pClassName, std::wstring lpWndName, LRESULT(CALLBACK* WndProc)(HWND, UINT, WPARAM, LPARAM));
	static void ShowForm(MyWindow* form);
};
#define MY_EXCEPTION( hr ) MyWindow::Exception(__LINE__, __FILE__, hr)
#define THROW_IF_ERROR( hr ) if (FAILED(hr)){ throw MyWindow::Exception(__LINE__, __FILE__, hr); }
#define MY_LAST_EXCEPTION MyWindow::Exception(__LINE__, __FILE__, GetLastError())