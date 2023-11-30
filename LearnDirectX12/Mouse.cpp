#include "Mouse.h"
#include <Windows.h>

std::pair<int, int> Mouse::GetPos() const noexcept {
	return{ x,y };
}
int Mouse::GetPosX()const noexcept {
	return x;
}
int Mouse::GetPosY()const noexcept {
	return y;
}
bool Mouse::IsInWindow() const noexcept{
	return isInWindow;
}
bool Mouse::LeftIsPressed()const noexcept{
	return leftIsPressed;
}
bool Mouse::RightIsPressed()const noexcept {
	return rightIsPressed;
}
void Mouse::Flush() noexcept {
	while (!IsEmpty()) {
		buffer.pop();
	}
}
void Mouse::OnMouseMove(int x, int y)noexcept {
	this->x = x;
	this->y = y;
	buffer.push(Event(Event::Type::Move,*this));
	TrimBuffer();
}
void Mouse::OnMouseLeave() noexcept {
	this->isInWindow = false;
	buffer.push(Event(Event::Type::Leave, *this));
	TrimBuffer();
}
void Mouse::OnMouseEnter()noexcept {
	this->isInWindow = true;
	buffer.push(Event(Event::Type::Enter, *this));
	TrimBuffer();
}
void Mouse::OnLeftPressed()noexcept {
	this->leftIsPressed = true;
	buffer.push(Event(Event::Type::LPress, *this));
	TrimBuffer();
}
void Mouse::OnRightPressed()noexcept {
	this->rightIsPressed = true;
	buffer.push(Event(Event::Type::RPress, *this));
	TrimBuffer();
}
void Mouse::OnLeftReleased() noexcept {
	this->leftIsPressed = false;
	buffer.push(Event(Event::Type::LRelease, *this));
	TrimBuffer();
}
void Mouse::OnRightReleased()noexcept {
	this->rightIsPressed = false;
	buffer.push(Event(Event::Type::RRelease, *this));
	TrimBuffer();
}
std::optional<Mouse::Event> Mouse::Read() noexcept {
	if (!IsEmpty()) {
		Event e = buffer.front();
		buffer.pop();
		return e;
	}
	return {};
}
void Mouse::OnWheelUp()noexcept {
	buffer.push(Event(Event::Type::WheelUp,*this));
	TrimBuffer();
}
void Mouse::OnWheelDown() noexcept {
	buffer.push(Event(Event::Type::WheelDown, *this));
	TrimBuffer();
}
void Mouse::TrimBuffer()noexcept {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}
void Mouse::OnWheelDelta(int delta) noexcept {
	wheelDeltaCarry += delta;
	while (wheelDeltaCarry >= WHEEL_DELTA) {
		OnWheelUp();
		wheelDeltaCarry -= WHEEL_DELTA;
	}
	while (wheelDeltaCarry <= WHEEL_DELTA) {
		OnWheelDown();
		wheelDeltaCarry += WHEEL_DELTA;
	}
}
