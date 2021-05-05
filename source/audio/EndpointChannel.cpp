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
		name.Content(e->name), m_Id = e->id;

	// Add if not already added.
	if (!std::contains(m_Endpoints, e))
		m_Endpoints.push_back(e), Lines(Lines() + 1), SortEndpoints();
};

void EndpointChannel::RemoveEndpoint(Endpoint* e)
{
	auto& it = std::find(m_Endpoints.begin(), m_Endpoints.end(), e);
	if (it != m_Endpoints.end())
		m_Endpoints.erase(it), Lines(Lines() - 1), SortEndpoints();
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
	nlohmann::json _json = m_EffectChain;
	_json["id"] = Id();
	_json["volume"] = volume;
	_json["muted"] = mute.Active();
	_json["mono"] = mono.Active();
	_json["pan"] = pan;
	_json["name"] = name.Content();
	_json["visible"] = m_Visible;

	if (Type() & Type::Input)
	{
		std::vector<int> _connections{};
		for (auto& i : m_Connections)
			_connections.push_back(i->Id());

		_json["connections"] = _connections;
	}

	std::vector<int> _channels{};
	for (auto& i : m_Endpoints)
		_channels.push_back(i->id);

	_json["channels"] = _channels;

	return _json;
};

void EndpointChannel::operator=(const nlohmann::json& json) 
{
	mono.Active(json.at("mono").get<bool>());
	mute.Active(json.at("muted").get<bool>());
	pan = json.at("pan");
	volume = json.at("volume");
	name.Content(json.at("name").get<std::string>());
	m_EffectChain = json;
	m_Visible = json.at("visible");
};