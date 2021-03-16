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

		class NoteOff { byte note, velocity, channel; };
		class NoteOn { byte note, velocity, channel; };
		class PolyAfterTouch { byte note, pressure, channel; };
		class ControlChange { byte control, value, channel; };
		class ProgramChange { byte program, _, channel; };
		class ChannelAfterTouch { byte pressure, _, channel; };
		class PitchWheel { uint16_t value, channel; };

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
	
	template<typename T>
	static std::vector<Callback<T>> m_Callbacks;



	Midi& Get()
	{
		static Midi instance;
		return instance;
	}

	Midi()
	{
		LoadPorts();
	}

	void OpenPort()
	{
		if (!m_Device)
			return;

		m_Midi.openPort(m_Device->id);
	}

	void LoadPorts()
	{
		int ports = m_Midi.getPortCount();
		for (int i = 0; i < ports; i++)
			m_Devices.emplace_back(i, m_Midi.getPortName(i));
	}

	void ReadMessages()
	{
		if (!m_Midi.isPortOpen())
			return;

		for (int i = 0; i < 50; i++)
		{
			m_Midi.getMessage(&m_Message);
			if (!m_Message.size())
				break;

			Event event{ m_Message };
			for (auto& i : m_Callbacks<Event>)
				i(event);

			switch (event.type)
			{
			case Event::Type::NoteOff: for (auto& i : m_Callbacks<Event::NoteOff>) i(event.noteoff); break;
			case Event::Type::NoteOn: for (auto& i : m_Callbacks<Event::NoteOn>) i(event.noteon);break;
			case Event::Type::PolyAfterTouch: for (auto& i : m_Callbacks<Event::PolyAfterTouch>) i(event.polyaftertouch);break;
			case Event::Type::ControlChange: for (auto& i : m_Callbacks<Event::ControlChange>) i(event.controlchange);break;
			case Event::Type::ProgramChange: for (auto& i : m_Callbacks<Event::ProgramChange>) i(event.programchange);break;
			case Event::Type::ChannelAfterTouch: for (auto& i : m_Callbacks<Event::ChannelAfterTouch>) i(event.channelaftertouch);break;
			case Event::Type::PitchWheel: for (auto& i : m_Callbacks<Event::PitchWheel>) i(event.pitchwheel);break;
			}
		};
	}

	template<typename T>
	void operator +=(Callback<T> t)
	{
		m_Callbacks<T>.push_back(t);
	}


	auto Devices() -> std::vector<MidiDevice>& { return m_Devices; }
	void Device(unsigned int d) { m_Device = &m_Devices[d]; }
	void Device(MidiDevice& d) { m_Device = &d; }

private:

	RtMidiIn m_Midi;
	MidiDevice* m_Device = nullptr;
	std::vector<MidiDevice> m_Devices;
	std::vector<uint8_t> m_Message;
};