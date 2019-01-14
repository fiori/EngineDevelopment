#include "../Headers/Mouse.h"

Mouse::Mouse()
{
}

Mouse::~Mouse()
{
}

int Mouse::GetPosX() const
{
	return m_x;
}

int Mouse::GetPosY() const
{
	return m_y;
}

bool Mouse::LeftIsPressed() const
{
	return m_leftIsPressed;
}

bool Mouse::RightIsPressed() const
{
	return m_rightIsPressed;
}

bool Mouse::IsInWindow() const
{
	return m_isInWindow;
}

Mouse::Event Mouse::Read()
{
	if (buffer.size() > 0u)
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

void Mouse::OnMouveMove(int x, int y)
{
	m_x = x;
	m_y = y;

	buffer.push(Mouse::Event(Mouse::Event::Move, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer()
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}