#include "audio/EndpointChannel.hpp"

void EndpointChannel::Process()
{
	m_Lock.lock();

	// Input takes sample from endpoint and sends to connections
	if (Type() & ChannelBase::Type::Input)
	{
		// First get all levels from the endpoints
		for (int i = 0; i < Lines(); i++)
			m_Levels[i] = m_Endpoints[i]->sample;

		// Then process the samples
		ChannelBase::Process();
	}

	// Otherwise it's output and takes samples from m_Levels and sends to endpoints
	else
	{
		// First process the incoming samples
		ChannelBase::Process();

		// Then forward them to the endpoints
		for (int i = 0; i < Lines(); i++)
			m_Endpoints[i]->sample = m_Levels[i];
	}

	// Reset levels
	for (auto& i : m_Levels)
		i = 0;

	m_Lock.unlock();
}

void EndpointChannel::AddEndpoint(Endpoint* e)
{
	// If first endpoint, set name of channel.
	if (m_Endpoints.size() == 0 && e)
		name.Content(e->name);

	// Add if not already added.
	if (!std::contains(m_Endpoints, e))
	{
		// Add and sort with new endpoint.
		m_Endpoints.push_back(e), Lines(Lines() + 1), SortEndpoints();
		SortEndpoints();

		// if Endpoint added, set id to first endpoint.
		m_Id = m_Endpoints[0]->id;
	}
};

void EndpointChannel::RemoveEndpoint(Endpoint* e)
{
	auto& it = std::find(m_Endpoints.begin(), m_Endpoints.end(), e);
	if (it != m_Endpoints.end())
	{
		// If not added, add endpoint.
		m_Endpoints.erase(it), Lines(Lines() - 1), SortEndpoints();

		// if Endpoint removed, reset if to first endpoint.
		if (m_Endpoints.size() != 0)
			m_Id = m_Endpoints[0]->id;
	}
};

void EndpointChannel::SortEndpoints()
{
	std::sort(m_Endpoints.begin(), m_Endpoints.end(),
		[](Endpoint* a, Endpoint* b) -> bool
		{
			return a->id < b->id;
		});

	CalcWidth();
}

void EndpointChannel::CalcWidth()
{
	if (Lines() <= 4)
		Width(70);
	else
		Width(Lines() * 7 + 42);
}

EndpointChannel::operator nlohmann::json()
{
	nlohmann::json _json = ChannelBase::operator nlohmann::json();

	std::vector<int> _channels{};
	for (auto& i : m_Endpoints)
		_channels.push_back(i->id);

	_json["channels"] = _channels;

	return _json;
};