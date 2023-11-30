#pragma once
#include <queue>
#include <optional>
#include <utility>

class Mouse {
	friend class MyWindow;
public:
	class Event {
	public:
		enum class Type {
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
		};
	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x{};
		int y{};
	public:
		Event(Type type, Mouse& parent)noexcept :type(type), leftIsPressed(parent.leftIsPressed), rightIsPressed(parent.rightIsPressed), x(parent.x), y(parent.y) {};
		Type GetType() const noexcept { return type; }
		std::pair<int, int> GetPos() const noexcept { return{ x,y }; }
		int GetPosX() const noexcept { return x; }
		int GetPosY() const noexcept { return y; }
		bool LeftIsPressed() const noexcept { return leftIsPressed; }
		bool RightIsPressed() const noexcept { return rightIsPressed; }
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetPosX() const noexcept;
	int GetPosY() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	std::optional<Mouse::Event> Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Flush() noexcept;
private:
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed() noexcept;
	void OnLeftReleased() noexcept;
	void OnRightPressed() noexcept;
	void OnRightReleased() noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void TrimBuffer() noexcept;
	void OnWheelDelta(int delta) noexcept;
private:
	static constexpr unsigned int bufferSize = 32u;
	int x;
	int y;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	int wheelDeltaCarry = 0;
	std::queue<Event> buffer;
};