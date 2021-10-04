#include "Midi.hpp"



Midi::Event::Event(std::vector<uint8_t>& message)
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

std::string Midi::Event::ToString()
{
	std::stringstream ss;
	ss << "(" << (int)type << ", " << (int)channel << ", " << (int)data1 << ", " << (int)data2 << ")";
	return ss.str();
}

Midi::Midi() 
{ 
	LoadPorts(); 
}

bool Midi::OpenInputPort(int id)
{
	CrashLog("Opening input midi port with id " << id);
	auto o = m_InOpened.try_emplace(id);
	if (o.second)
		try
	{
		o.first->second.openPort(id);
	}
	catch (...)
	{
		CrashLog("Failed to open port");
		m_InOpened.erase(m_InOpened.find(id));
		return false;
	}

	return true;
}

bool Midi::OpenOutputPort(int id)
{
	CrashLog("Opening output midi port with id " << id);
	auto o = m_OutOpened.try_emplace(id);
	if (o.second)
		try
	{
		o.first->second.openPort(id);
	}
	catch (...)
	{
		CrashLog("Failed to open port");
		m_OutOpened.erase(m_OutOpened.find(id));
		return false;
	}

	return true;
}

void Midi::CloseInputPort(int id)
{
	CrashLog("Closing input midi port with id " << id);
	auto i = m_InOpened.find(id);
	if (i != m_InOpened.end())
		i->second.closePort(), m_InOpened.erase(i);
}

void Midi::CloseOutputPort(int id)
{
	CrashLog("Closing output midi port with id " << id);
	auto i = m_OutOpened.find(id);
	if (i != m_OutOpened.end())
		i->second.closePort(), m_OutOpened.erase(i);
}

void Midi::LoadPorts()
{
	CrashLog("Loading midi ports...");
	m_InputDevices.clear();
	int ports = m_MidiIn.getPortCount();
	CrashLog("Found " << ports << " input ports");
	for (unsigned int i = 0; i < ports; i++)
	{
		CrashLog(i << " : " << m_MidiIn.getPortName(i));
		m_InputDevices.emplace_back(Midi::Device{ i, m_MidiIn.getPortName(i) });
	}
	m_OutputDevices.clear();
	ports = m_MidiOut.getPortCount();
	CrashLog("Found " << ports << " output ports");
	for (unsigned int i = 0; i < ports; i++)
	{
		CrashLog(i << " : " << m_MidiOut.getPortName(i));
		m_OutputDevices.emplace_back(Midi::Device{ i, m_MidiOut.getPortName(i) });
	}
}

void Midi::ReadMessages()
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

			Event event{ m_Message };
			event.device = midi.first;


			// Forward midi to all connected output devices
			std::stringstream ss;
			bool first = true;
			for (auto& out : m_OutOpened)
			{
				if (first)
					ss << ", ", first = false;
				ss << out.first;
				out.second.sendMessage(&m_Message);
			}

			CrashLog("Midi from channel " << midi.first << ": " << event.ToString() << "; forward to outputs: " << ss.str());

			// Send the event to all callbacks.
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