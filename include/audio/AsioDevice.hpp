
class Endpoint
{
public:
	float sample;
};


class AsioDevice
{
public:
	std::vector<Endpoint>& Inputs() { return m_Inputs; }
	std::vector<Endpoint>& Outputs() { return m_Outputs; }

private:
	std::vector<Endpoint> m_Inputs;
	std::vector<Endpoint> m_Outputs;
};