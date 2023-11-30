#include "MyException.h"

MyException::MyException(int line, const char* file) noexcept  {
	this->line = line;
	this->file = file;
}

const char* MyException::what() const noexcept {
	std::ostringstream ss;
	ss << GetType() << std::endl << GetOriginString();
	whatBuffer = ss.str();
	return whatBuffer.c_str();
}

std::string MyException::GetOriginString() const noexcept
{
	std::ostringstream wss;
	wss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return wss.str();
}
int MyException::GetLine() const noexcept
{
	return line;
}
const std::string& MyException::GetFile() const noexcept
{
	return file;
}
const char* MyException::GetType() const noexcept {
	return "Base Exception";
}
