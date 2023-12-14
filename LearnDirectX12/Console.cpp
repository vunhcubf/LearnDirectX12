#include "Console.h"

void Console::PrintLine(std::wstring outputString)
{
	DWORD written;
	WriteConsole(hStdOut, outputString.c_str(), wcslen(outputString.c_str()), &written, NULL);
}

void Console::PrintLineA(std::string text)
{
	std::wstring wtext(text.begin(), text.end());
	PrintLine(wtext);
}
