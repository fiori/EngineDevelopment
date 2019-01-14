#pragma once
#include <queue>

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum Type
		{
			LeftPress,
			LeftRelease,
			RightPress,
			RightRelease,
			Move,
			Invalid
		};
	private:
		Type m_type;
		bool m_leftIsPressed;
		bool m_rightIsPressed;
		int m_x;
		int m_y;
	public:
		Event()
			:m_type(Invalid), m_leftIsPressed(false), m_rightIsPressed(false), m_x(0), m_y(0)
		{}
		Event(Type type, const Mouse& parent)
			:m_type(Invalid), m_leftIsPressed(false), m_rightIsPressed(false), m_x(parent.m_x), m_y(parent.m_y)
		{}
	public:
		bool IsValid() const
		{
			return m_type != Invalid;
		}
		Type GetType() const
		{
			return m_type;
		}

		int GetPosX() const
		{
			return m_x;
		}
		int GetPosY() const
		{
			return m_y;
		}
		bool LeftIsPressed() const
		{
			return m_leftIsPressed;
		}
		bool RightIsPressed() const
		{
			return m_rightIsPressed;
		}
	};
public:
	Mouse();
	~Mouse();
	int GetPosX() const;
	int GetPosY() const;
	bool LeftIsPressed() const;
	bool RightIsPressed() const;
	bool IsInWindow() const;
	Mouse::Event Read();
	bool IsEmpty() const
	{
		return buffer.empty();
	}
private:
	void OnMouveMove(int x, int y);
	void TrimBuffer();
private:
	static constexpr unsigned int bufferSize = 4u;
	int m_x = 0;
	int m_y = 0;
	bool m_leftIsPressed = false;
	bool m_rightIsPressed = false;
	bool m_isInWindow = false;
	std::queue<Event> buffer;
};
