#pragma once
#include "pch.hpp"

/**
 * Device class contains information regarting an Asio device.
 */
class Device
{
public:
	Device() : info(*static_cast<const PaDeviceInfo*>(nullptr)) { }
	Device(PaDeviceIndex id, const PaDeviceInfo& a)
		: id(id), info(a)
	{}

	PaDeviceIndex id;
	const PaDeviceInfo& info;
};

/**
 * The Endpoint class is used as a single endpoint of a device.
 * <code>sample</code> is used as an input/output.
 */
class Endpoint
{
public:
	Endpoint(int id, const std::string& name, bool input)
		: id(id), name(name), input(input)
	{}

	float sample = 0;

	const std::string name;
	int id;
	bool input;
};

/**
 * Asio class opens/closes Asio devices and their streams.
 */
class Asio
{
public:
	static int SAMPLE_RATE;

	/**
	 * Constructor
	 * Retrieves list of available devices.
	 * @param bool only asio
	 */
	Asio(bool = true);

	/**
	 * Destructor
	 * Makes sure the stream is closed.
	 */
	~Asio();

	/**
	 * Loads all available devices into a map.
	 */
	void ReloadDevices();

	/**
	 * Set the sample rate.
	 * @param sr sample rate
	 */
	void SampleRate(int sr) { m_Samplerate = sr; }

	/**
	 * Get the sample rate.
	 * @return sample rate
	 */
	int SampleRate() const { return m_Samplerate; }

	/**
	 * Get all available devices.
	 * @return vector of devices
	 */
	auto Devices() -> std::unordered_map<int, ::Device>& { return m_Devices; }

	/**
	 * Set the device that should be opened.
	 * @param d device
	 */
	void Device(::Device& d) { m_Device = d.id; }

	/**
	 * Set the device that should be opened using its id.
	 * @param d id
	 */
	void Device(int d) { m_Device = d; }
	
	/**
	 * Clear the device, sets it to nullptr.
	 */
	void NoDevice() { m_Device = -1; }

	/**
	 * Get the current device.
	 * @return device
	 */
	auto Device() -> ::Device* { return m_Device != -1 ? &m_Devices.at(m_Device) : nullptr; }

	/**
	 * Returns true if the stream is currently running.
	 * @return true if stream running
	 */
	bool StreamRunning() { return Pa_IsStreamActive(stream); }

	/**
	 * Returns true if a stream is opened, also returns true if it's also running.
	 * @return true if stream open
	 */
	bool StreamOpened() const { return m_Opened; }

	/**
	 * Get the device id of the device that is currently open. -1 if no device.
	 * @return device id
	 */
	int DeviceId() const { return m_Device != -1 ? m_Devices.at(m_Device).id : -1; }
	
	/**
	 * Opens a stream with the set device and the given callback and userdata.
	 * @param c callback
	 * @param userdata userdata
	 */
	bool OpenStream(PaStreamCallback c, void* userdata);

	/**
	 * Closes the stream if it is opened.
	 */
	void CloseStream();
	
	/**
	 * Starts the stream if it's opened and not already running.
	 * @return true when started, false when not open, already running, or error.
	 */
	bool StartStream();

	/**
	 * Stops a stream if it's running.
	 * @return true when stopped, false if not running or error.
	 */
	bool StopStream();

	/**
	 * Get all the input Endpoints.
	 * @return inputs
	 */
	std::vector<Endpoint>& Inputs() { return m_Inputs; }

	/**
	 * Get all the output Endpoints.
	 * @return outputs
	 */
	std::vector<Endpoint>& Outputs() { return m_Outputs; }

	PaStream* Stream() { return stream; }

private:
	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;

	double m_Samplerate;
	int m_BufferSize;
	bool m_Opened = false;
	bool m_IsAsioOnly = true;

	PaStream* stream = nullptr;
	int m_Device = -1;

	std::unordered_map<int, ::Device> m_Devices;

	static bool m_PortAudioInit;
};