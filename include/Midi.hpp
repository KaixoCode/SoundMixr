#pragma once
#include "pch.hpp"

class Midi
{
public:

	enum class Type
	{
		Input, Output
	};

	class Device
	{
	public:
		Device(unsigned int id, const std::string& name)
			: id(id), name(name.substr(0, name.size() - 2))
		{}
		unsigned int id;
		std::string name;
	};

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
			uint32_t data;
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

	void OpenInputPort(Midi::Device& m) { OpenInputPort(m.id); }

	bool OpenInputPort(int id)
	{
		auto& o = m_InOpened.try_emplace(id);
		if (o.second)
			try
			{
				o.first->second.openPort(id);
			}
			catch (...)
			{
				m_InOpened.erase(m_InOpened.find(id));
				return false;
			}

		return true;
	}

	void OpenOutputPort(Midi::Device& m) { OpenOutputPort(m.id); }

	bool OpenOutputPort(int id)
	{
		auto& o = m_OutOpened.try_emplace(id);
		if (o.second)
			try
		{
			o.first->second.openPort(id);
		}
		catch (...)
		{
			m_OutOpened.erase(m_OutOpened.find(id));
			return false;
		}

		return true;
	}

	void CloseInputPort(Midi::Device& m) { CloseInputPort(m.id); }

	void CloseInputPort(int id)
	{
		auto& i = m_InOpened.find(id);
		if (i != m_InOpened.end())
			i->second.closePort(), m_InOpened.erase(i);
	}

	void CloseOutputPort(Midi::Device& m) { CloseOutputPort(m.id); }

	void CloseOutputPort(int id)
	{
		auto& i = m_OutOpened.find(id);
		if (i != m_OutOpened.end())
			i->second.closePort(), m_OutOpened.erase(i);
	}

	void LoadPorts()
	{
		m_InputDevices.clear();
		int ports = m_MidiIn.getPortCount();
		for (unsigned int i = 0; i < ports; i++)
			m_InputDevices.emplace_back(Midi::Device{ i, m_MidiIn.getPortName(i) });

		m_OutputDevices.clear();
		ports = m_MidiOut.getPortCount();
		for (unsigned int i = 0; i < ports; i++)
			m_OutputDevices.emplace_back(Midi::Device{ i, m_MidiOut.getPortName(i) });
	}

	void ReadMessages()
	{
		if (m_InOpened.size() == 0)
			return;

		// Go through all opened midi devices.
		for (auto& midi : m_InOpened)
		{
			// Read 50 messages.
			for (int i = 0; i < 50; i++)
			{
				// Get the message, if no message it's empty so break.
				midi.second.getMessage(&m_Message);
				if (!m_Message.size())
					break;

				// Forward midi to all connected output devices
				for (auto& out : m_OutOpened)
					out.second.sendMessage(&m_Message);

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

	std::unordered_map<int, RtMidiIn>& InputsOpened() { return m_InOpened; }
	std::unordered_map<int, RtMidiOut>& OutputsOpened() { return m_OutOpened; }


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
	template<> void Remove<Event>(int id) { if (m_EventCallbacks.find(id) != m_EventCallbacks.end()) m_EventCallbacks.erase(m_EventCallbacks.find(id)); }
	template<> void Remove<Event::NoteOff>(int id) { if (m_NoteOffCallbacks.find(id) != m_NoteOffCallbacks.end()) m_NoteOffCallbacks.erase(m_NoteOffCallbacks.find(id)); }
	template<> void Remove<Event::NoteOn>(int id) { if (m_NoteOnCallbacks.find(id) != m_NoteOnCallbacks.end()) m_NoteOnCallbacks.erase(m_NoteOnCallbacks.find(id)); }
	template<> void Remove<Event::PolyAfterTouch>(int id) { if (m_PolyAfterTouchCallbacks.find(id) != m_PolyAfterTouchCallbacks.end()) m_PolyAfterTouchCallbacks.erase(m_PolyAfterTouchCallbacks.find(id)); }
	template<> void Remove<Event::ControlChange>(int id) { if (m_ControlChangeCallbacks.find(id) != m_ControlChangeCallbacks.end()) m_ControlChangeCallbacks.erase(m_ControlChangeCallbacks.find(id)); }
	template<> void Remove<Event::ProgramChange>(int id) { if (m_ProgramChangeCallbacks.find(id) != m_ProgramChangeCallbacks.end()) m_ProgramChangeCallbacks.erase(m_ProgramChangeCallbacks.find(id)); }
	template<> void Remove<Event::ChannelAfterTouch>(int id) { if (m_ChannelAfterTouchCallbacks.find(id) != m_ChannelAfterTouchCallbacks.end()) m_ChannelAfterTouchCallbacks.erase(m_ChannelAfterTouchCallbacks.find(id)); }
	template<> void Remove<Event::PitchWheel>(int id) { if (m_PitchWheelCallbacks.find(id) != m_PitchWheelCallbacks.end()) m_PitchWheelCallbacks.erase(m_PitchWheelCallbacks.find(id)); }

	auto InputDevices() -> std::vector<Midi::Device>& { return m_InputDevices; }
	auto OutputDevices() -> std::vector<Midi::Device>& { return m_OutputDevices; }

	int InputDeviceCount() { return m_MidiIn.getPortCount(); }
	int OutputDeviceCount() { return m_MidiOut.getPortCount(); }

private:
	std::unordered_map<int, Callback<Event>>                    m_EventCallbacks;
	std::unordered_map<int, Callback<Event::NoteOff>>           m_NoteOffCallbacks;
	std::unordered_map<int, Callback<Event::NoteOn>>            m_NoteOnCallbacks;
	std::unordered_map<int, Callback<Event::PolyAfterTouch>>    m_PolyAfterTouchCallbacks;
	std::unordered_map<int, Callback<Event::ControlChange>>     m_ControlChangeCallbacks;
	std::unordered_map<int, Callback<Event::ProgramChange>>     m_ProgramChangeCallbacks;
	std::unordered_map<int, Callback<Event::ChannelAfterTouch>> m_ChannelAfterTouchCallbacks;
	std::unordered_map<int, Callback<Event::PitchWheel>>        m_PitchWheelCallbacks;

	RtMidiIn m_MidiIn;
	RtMidiOut m_MidiOut;
	std::vector<Midi::Device> m_InputDevices;
	std::vector<Midi::Device> m_OutputDevices;
	std::unordered_map<int, RtMidiIn> m_InOpened;
	std::unordered_map<int, RtMidiOut> m_OutOpened;
	std::vector<uint8_t> m_Message;
	int m_Counter = 0;
};