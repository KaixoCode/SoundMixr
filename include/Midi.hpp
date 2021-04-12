#pragma once
#include "pch.hpp"

class MidiDevice
{
public:
	unsigned int id;
	std::string name;
};

class Midi
{
public:

	class Event
	{
	public:
		enum class Type
		{
			NoteOn = 0b1001,
			NoteOff = 0b1000,
			PolyAfterTouch = 0b1010,
			ControlChange = 0b1011,
			ProgramChange = 0b1100,
			ChannelAfterTouch = 0b1101,
			PitchWheel = 0b1110
		};

		Type type;

		Event(std::vector<uint8_t>& message)
		{
			int bytes = message.size();
			if (bytes > 0)
				type = (Type)((message[0] & 0b11110000) >> 4),
				channel = (message[0] & 0b00001111);

			if (bytes > 1)
				data1 = (message[1] & 0b01111111);

			if (bytes > 2)
				data2 = (message[2] & 0b01111111);
		}

		struct NoteOff { byte note, velocity, channel, device; };
		struct NoteOn { byte note, velocity, channel, device; };
		struct PolyAfterTouch { byte note, pressure, channel, device; };
		struct ControlChange { byte control, value, channel, device; };
		struct ProgramChange { byte program, _, channel, device; };
		struct ChannelAfterTouch { byte pressure, _, channel, device; };
		struct PitchWheel { uint16_t value; byte channel, device; };

		union
		{
			struct { byte data1, data2, channel, device; };

			NoteOff noteoff;
			NoteOn noteon;
			PolyAfterTouch polyaftertouch;
			ControlChange controlchange;
			ProgramChange programchange;
			ChannelAfterTouch channelaftertouch;
			PitchWheel pitchwheel;
		};
	};

	template<typename T>
	using Callback = std::function<void(T&)>;

	static inline Midi& Get()
	{
		static Midi instance;
		return instance;
	}

	Midi() { LoadPorts(); }

	void OpenPort(MidiDevice& m) { OpenPort(m.id); }

	bool OpenPort(int id)
	{
		auto& o = m_Opened.try_emplace(id);
		if (o.second)
			try
			{
				o.first->second.openPort(id);
			}
			catch (...)
			{
				m_Opened.erase(m_Opened.find(id));
				return false;
			}

		return true;
	}

	void ClosePort(MidiDevice& m) { ClosePort(m.id); }

	void ClosePort(int id)
	{
		auto& i = m_Opened.find(id);
		if (i != m_Opened.end())
			i->second.closePort(), m_Opened.erase(i);
	}

	void LoadPorts()
	{
		m_Devices.clear();
		int ports = m_Midi.getPortCount();
		for (unsigned int i = 0; i < ports; i++)
			m_Devices.emplace_back(MidiDevice{ i, m_Midi.getPortName(i) });
	}

	void ReadMessages()
	{
		if (m_Opened.size() == 0)
			return;

		// Go through all opened midi devices.
		for (auto& midi : m_Opened)
		{
			// Read 50 messages.
			for (int i = 0; i < 50; i++)
			{
				// Get the message, if no message it's empty so break.
				midi.second.getMessage(&m_Message);
				if (!m_Message.size())
					break;

				// Send the event to all callbacks.
				Event event{ m_Message };
				event.device = midi.first;
				//LOG((int)event.type << ", " << (int)event.data1 << ", " << (int)event.data2);
				for (auto& [j, i] : m_EventCallbacks)
					i(event);

				switch (event.type)
				{
				case Event::Type::NoteOff: for (auto& [j, i] : m_NoteOffCallbacks) i(event.noteoff); break;
				case Event::Type::NoteOn: for (auto& [j, i] : m_NoteOnCallbacks) i(event.noteon);break;
				case Event::Type::PolyAfterTouch: for (auto& [j, i] : m_PolyAfterTouchCallbacks) i(event.polyaftertouch);break;
				case Event::Type::ControlChange: for (auto& [j, i] : m_ControlChangeCallbacks) i(event.controlchange);break;
				case Event::Type::ProgramChange: for (auto& [j, i] : m_ProgramChangeCallbacks) i(event.programchange);break;
				case Event::Type::ChannelAfterTouch: for (auto& [j, i] : m_ChannelAfterTouchCallbacks) i(event.channelaftertouch);break;
				case Event::Type::PitchWheel: for (auto& [j, i] : m_PitchWheelCallbacks) i(event.pitchwheel);break;
				}
			}
		};
	}

	std::unordered_map<int, RtMidiIn>& Opened() { return m_Opened; }


	class EventStorage
	{
	public:
		using Callback = std::function<void(void)>;

		EventStorage(Callback c)
			: m_Callback(c)
		{}

		~EventStorage()
		{
			m_Callback();
		}

	private:
		Callback m_Callback;
	};

	template<typename T>
	inline auto operator+=(T t) { return AddCallback(t); }

	auto AddCallback(Callback<Event> a)                    { m_EventCallbacks.emplace(m_Counter++, a);             int b = m_Counter; return [&, b] { Remove<Event>(b - 1); }; }
	auto AddCallback(Callback<Event::NoteOff> a)           { m_NoteOffCallbacks.emplace(m_Counter++, a);           int b = m_Counter; return [&, b] { Remove<Event::NoteOff>(b - 1); }; }
	auto AddCallback(Callback<Event::NoteOn> a)            { m_NoteOnCallbacks.emplace(m_Counter++, a);            int b = m_Counter; return [&, b] { Remove<Event::NoteOn>(b - 1); }; }
	auto AddCallback(Callback<Event::PolyAfterTouch> a)    { m_PolyAfterTouchCallbacks.emplace(m_Counter++, a);    int b = m_Counter; return [&, b] { Remove<Event::PolyAfterTouch>(b - 1); }; }
	auto AddCallback(Callback<Event::ControlChange> a)     { m_ControlChangeCallbacks.emplace(m_Counter++, a);     int b = m_Counter; return [&, b] { Remove<Event::ControlChange>(b - 1); }; }
	auto AddCallback(Callback<Event::ProgramChange> a)     { m_ProgramChangeCallbacks.emplace(m_Counter++, a);     int b = m_Counter; return [&, b] { Remove<Event::ProgramChange>(b - 1); }; }
	auto AddCallback(Callback<Event::ChannelAfterTouch> a) { m_ChannelAfterTouchCallbacks.emplace(m_Counter++, a); int b = m_Counter; return [&, b] { Remove<Event::ChannelAfterTouch>(b - 1); }; }
	auto AddCallback(Callback<Event::PitchWheel> a)        { m_PitchWheelCallbacks.emplace(m_Counter++, a);        int b = m_Counter; return [&, b] { Remove<Event::PitchWheel>(b - 1); }; }

	template<typename T>
	void Remove(int id) { RemoveCallback(id); }
	template<> void Remove<Event>(int id) { m_EventCallbacks.erase(m_EventCallbacks.find(id)); }
	template<> void Remove<Event::NoteOff>(int id) { m_NoteOffCallbacks.erase(m_NoteOffCallbacks.find(id)); }
	template<> void Remove<Event::NoteOn>(int id) { m_NoteOnCallbacks.erase(m_NoteOnCallbacks.find(id)); }
	template<> void Remove<Event::PolyAfterTouch>(int id) { m_PolyAfterTouchCallbacks.erase(m_PolyAfterTouchCallbacks.find(id)); }
	template<> void Remove<Event::ControlChange>(int id) { m_ControlChangeCallbacks.erase(m_ControlChangeCallbacks.find(id)); }
	template<> void Remove<Event::ProgramChange>(int id) { m_ProgramChangeCallbacks.erase(m_ProgramChangeCallbacks.find(id)); }
	template<> void Remove<Event::ChannelAfterTouch>(int id) { m_ChannelAfterTouchCallbacks.erase(m_ChannelAfterTouchCallbacks.find(id)); }
	template<> void Remove<Event::PitchWheel>(int id) { m_PitchWheelCallbacks.erase(m_PitchWheelCallbacks.find(id)); }

	auto Devices() -> std::vector<MidiDevice>& { return m_Devices; }

	int DeviceCount() { return m_Midi.getPortCount(); }

private:
	std::unordered_map<int, Callback<Event>>                    m_EventCallbacks;
	std::unordered_map<int, Callback<Event::NoteOff>>           m_NoteOffCallbacks;
	std::unordered_map<int, Callback<Event::NoteOn>>            m_NoteOnCallbacks;
	std::unordered_map<int, Callback<Event::PolyAfterTouch>>    m_PolyAfterTouchCallbacks;
	std::unordered_map<int, Callback<Event::ControlChange>>     m_ControlChangeCallbacks;
	std::unordered_map<int, Callback<Event::ProgramChange>>     m_ProgramChangeCallbacks;
	std::unordered_map<int, Callback<Event::ChannelAfterTouch>> m_ChannelAfterTouchCallbacks;
	std::unordered_map<int, Callback<Event::PitchWheel>>        m_PitchWheelCallbacks;

	RtMidiIn m_Midi;
	std::vector<MidiDevice> m_Devices;
	std::unordered_map<int, RtMidiIn> m_Opened;
	std::vector<uint8_t> m_Message;
	int m_Counter = 0;
};