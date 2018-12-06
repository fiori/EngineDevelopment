#pragma once
#include <queue>
#include <bitset>

class Keyboard 
{
	friend class Window;
public:
	class Event
	{
	public:
		enum Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event()
			:type(Invalid), code(0u) {}
		Event(Type type, unsigned char code)
			:type(type), code(code) {}
		bool IsPress() const
		{
			return type == Press;
		}
		bool IsRelease() const
		{
			return type == Release;
		}
		bool IsValid() const
		{
			return type != Invalid;
		}
		unsigned char GetCode() const
		{
			return code;
		}
	};
public:
	Keyboard() = default;
	bool KeyIsPressed(unsigned char keycode) const;
	Event ReadKey();
	bool KeyIsEmpty() const;
	void EnableAutorepeat();
	void DisableAutorepeat();
	bool AutorepeatIsEnabled() const;
private:
	void OnKeyPressed(unsigned char keycode);
	void OnKeyReleased(unsigned char keycode);
	template<typename T>
	void TrimBuffer(std::queue<T>&buffer);
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 4u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
};





