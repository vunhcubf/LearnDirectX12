#include "MyWindow.h"

MyWindow* MyWindow::RegisterForm(HINSTANCE hInstance, int width, int height, std::wstring pClassName, std::wstring lpWndName) {
	return MyWindow::RegisterForm(hInstance, width, height, pClassName, lpWndName, nullptr);
}

MyWindow* MyWindow::RegisterForm(HINSTANCE hInstance, int width, int height, std::wstring pClassName, std::wstring lpWndName, LRESULT(CALLBACK* WndProc)(HWND, UINT, WPARAM, LPARAM)) {
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRectEx(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false, 0) == 0) {
		throw MY_LAST_EXCEPTION;
	}
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hCursor = nullptr;
	wc.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 128, 128, 0));
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName.c_str();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 64, 64, 0));
	RegisterClassEx(&wc);
	MyWindow* window = new MyWindow(hInstance, width, height);
	HWND hWnd = CreateWindowExW(0, pClassName.c_str(), lpWndName.c_str(), WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, hInstance, window);
	window->SetHWND(hWnd);
	window->pWinProc = WndProc;
	window->WindowTitle = lpWndName;
	return window;
}

void MyWindow::ShowForm(MyWindow* window) {
	window->IsActive = true;
	ShowWindow(window->hWnd, SW_SHOWDEFAULT);
}

MyWindow::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
	:MyException(line, file), hr(hr) {}

const char* MyWindow::Exception::what() const noexcept {
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "File:" << file << std::endl
		<< "Line:" << line << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorString() << std::endl;
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* MyWindow::Exception::GetType() const noexcept {
	return "Exception Of Window";
}

HRESULT MyWindow::Exception::GetErrorCode() const noexcept {
	return hr;
}

std::string MyWindow::Exception::TranslateerrorCode(HRESULT hr) noexcept {
	char* pMsgBuf = nullptr;
	// windows will allocate memory for err string and make our pointer point to it
	const DWORD nMsgLen = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// 0 string length returned indicates a failure
	if (nMsgLen == 0)
	{
		return "Undefined Error Code";
	}
	// copy error string from windows-allocated buffer to std::string
	std::string errorString = pMsgBuf;
	// free windows buffer
	LocalFree(pMsgBuf);
	return errorString;
}

std::string MyWindow::Exception::GetErrorString() const noexcept {
	return TranslateerrorCode(hr);
}

LRESULT CALLBACK MyWindow::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_CREATE) {
		MyWindow* pMyWindow;
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pMyWindow = reinterpret_cast<MyWindow*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pMyWindow));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&MyWindow::HandleMsgIntermedia));
		return pMyWindow->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT CALLBACK MyWindow::HandleMsgIntermedia(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	MyWindow* pMyWindow = reinterpret_cast<MyWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (pMyWindow->pWinProc) {
		return pMyWindow->pWinProc(hWnd, msg, wParam, lParam);
	}
	return pMyWindow->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK MyWindow::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(WM_QUIT);
		break;
		return 0;
	case WM_KILLFOCUS:
		keyBoard.ClearState();
	case WM_SYSKEYDOWN:
		//如果是第一次按下或者开启了连按
		if (!(lParam & 0x40000000) || keyBoard.AutorepeatIsEnabled()) {
			keyBoard.OnKeyPressed(unsigned char(wParam));
		}
		break;
	case WM_SYSKEYUP:
		keyBoard.OnKeyReleased(unsigned char(wParam));
		break;
	case WM_CHAR:
		keyBoard.OnChar(unsigned char(wParam));
		break;
	case WM_LBUTTONDOWN:
		mouse.OnLeftPressed();
		break;
	case WM_LBUTTONUP:
		mouse.OnLeftReleased();
		break;
	case WM_RBUTTONDOWN:
		mouse.OnRightPressed();
		break;
	case WM_RBUTTONUP:
		mouse.OnRightReleased();
		break;
	case WM_MOUSEMOVE: {
		const POINTS pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.y >= 0 && pt.x <= Width && pt.y <= Height) {
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.isInWindow) {
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else {
			if (mouse.LeftIsPressed() || mouse.RightIsPressed()) {
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else {
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_MOUSEWHEEL:
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(delta);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}