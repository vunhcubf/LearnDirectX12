#pragma once
#include <Windows.h>
#include <string>

class Console {
public:
	HANDLE hStdOut;
	Console() {
		AllocConsole();
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	}
    void PrintLine(std::wstring text);
	void PrintLineA(std::string text);
	~Console() {
		FreeConsole();
	}
};