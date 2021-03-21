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
			NoteOn = 0b1000,
			NoteOff = 0b1001,
			PolyAfterTouch = 0b1010,
			ControlChange = 0b1011,
			ProgramChange = 0b1100,
			ChannelAfterTouch = 0b1101,
			PitchWheel = 0b1110
		};

		Type type;
		unsigned int device = -1;

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

		struct NoteOff { byte note, velocity, channel; };
		struct NoteOn { byte note, velocity, channel; };
		struct PolyAfterTouch { byte note, pressure, channel; };
		struct ControlChange { byte control, value, channel; };
		struct ProgramChange { byte program, _, channel; };
		struct ChannelAfterTouch { byte pressure, _, channel; };
		struct PitchWheel { uint16_t value, channel; };

		union
		{
			struct { byte data1, data2, channel; };

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

	Midi()
	{
		LoadPorts();
	}

	void OpenPort(MidiDevice& m)
	{
		OpenPort(m.id);
	}

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

	void ClosePort(MidiDevice& m)
	{
		ClosePort(m.id);
	}

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
				for (auto& i : m_EventCallbacks)
					i(event);

				switch (event.type)
				{
				case Event::Type::NoteOff: for (auto& i : m_NoteOffCallbacks) i(event.noteoff); break;
				case Event::Type::NoteOn: for (auto& i : m_NoteOnCallbacks) i(event.noteon);break;
				case Event::Type::PolyAfterTouch: for (auto& i : m_PolyAfterTouchCallbacks) i(event.polyaftertouch);break;
				case Event::Type::ControlChange: for (auto& i : m_ControlChangeCallbacks) i(event.controlchange);break;
				case Event::Type::ProgramChange: for (auto& i : m_ProgramChangeCallbacks) i(event.programchange);break;
				case Event::Type::ChannelAfterTouch: for (auto& i : m_ChannelAfterTouchCallbacks) i(event.channelaftertouch);break;
				case Event::Type::PitchWheel: for (auto& i : m_PitchWheelCallbacks) i(event.pitchwheel);break;
				}
			}
		};
	}

	template<typename T>
	void operator+=(T t) { AddCallback(t); }

	void AddCallback(Callback<Event> a) { m_EventCallbacks.emplace_back(a); }
	void AddCallback(Callback<Event::NoteOff> a) { m_NoteOffCallbacks.emplace_back(a); }
	void AddCallback(Callback<Event::NoteOn> a) { m_NoteOnCallbacks.emplace_back(a); }
	void AddCallback(Callback<Event::PolyAfterTouch> a) { m_PolyAfterTouchCallbacks.emplace_back(a); }
	void AddCallback(Callback<Event::ControlChange> a) { m_ControlChangeCallbacks.emplace_back(a); }
	void AddCallback(Callback<Event::ProgramChange> a) { m_ProgramChangeCallbacks.emplace_back(a); }
	void AddCallback(Callback<Event::ChannelAfterTouch> a) { m_ChannelAfterTouchCallbacks.emplace_back(a); }
	void AddCallback(Callback<Event::PitchWheel> a) { m_PitchWheelCallbacks.emplace_back(a); }

	auto Devices() -> std::vector<MidiDevice>& { return m_Devices; }

private:
	std::vector<Callback<Event>>                    m_EventCallbacks;
	std::vector<Callback<Event::NoteOff>>           m_NoteOffCallbacks;
	std::vector<Callback<Event::NoteOn>>            m_NoteOnCallbacks;
	std::vector<Callback<Event::PolyAfterTouch>>    m_PolyAfterTouchCallbacks;
	std::vector<Callback<Event::ControlChange>>     m_ControlChangeCallbacks;
	std::vector<Callback<Event::ProgramChange>>     m_ProgramChangeCallbacks;
	std::vector<Callback<Event::ChannelAfterTouch>> m_ChannelAfterTouchCallbacks;
	std::vector<Callback<Event::PitchWheel>>        m_PitchWheelCallbacks;

	RtMidiIn m_Midi;
	std::vector<MidiDevice> m_Devices;
	std::unordered_map<int, RtMidiIn> m_Opened;
	std::vector<uint8_t> m_Message;
};