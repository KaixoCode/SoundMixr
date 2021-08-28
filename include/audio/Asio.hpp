#pragma once
#include "pch.hpp"
using namespace Audijo;

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
class Asio : public Stream<Api::Asio>
{
public:
	static int SAMPLE_RATE;

	/**
	 * Constructor
	 * Retrieves list of available devices.
	 */
	Asio();

	/**
	 * Destructor
	 * Makes sure the stream is closed.
	 */
	~Asio();

	bool OpenStream(const StreamParameters&);
	bool StartStream();
	bool StopStream();
	void CloseStream();

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

private:
	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;
};