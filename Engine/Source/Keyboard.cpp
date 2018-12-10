#include "../Headers/Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keycode) const
{
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey()
{
	if (keybuffer.size() > 0u)
	{
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const
{
	return keybuffer.empty();
}

void Keyboard::EnableAutorepeat()
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat()
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const
{
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode)
{
	keystates[keycode] = true;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Press, keycode));
	TrimBuffer(keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char keycode)
{
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Release, keycode));
	TrimBuffer(keybuffer);
}

//Trims the buffer so it does not get bigger than 4bytes
template <typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer)
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}