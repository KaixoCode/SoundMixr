#include "audio/EndpointChannel.hpp"

EndpointChannel::EndpointChannel(ChannelType type)
	: ChannelBase(type), 
	m_SubChannels(Emplace<SubChannelCollection>())
{	
	m_SubChannels.Background(Color{ 0, 0, 0, 0 });
	m_SubChannels.Width(0);

	Div().Clear();

	// Layout is divs, for easy complex layout.
	Layout<Layout::Divs>();
	Div() = { 2, Div::Alignment::Horizontal, 0, false, Div::AUTO };
	Div()[0] = { 7, Div::Alignment::Vertical, 0, false, Div::AUTO };
	Div()[0][6] = { name, Div::Alignment::Center, 28, true };
	Div()[0][5].DivSize(14);
	Div()[0][4] = { 2, Div::Alignment::Horizontal, 0, false, Div::AUTO };
	Div()[0][4][0] = { volume, Div::Alignment::Left, Div::AUTO, true };
	Div()[0][4][1].DivSize(24);
	Div()[0][3] = { 2, Div::Alignment::Horizontal, 0, false, 24 };
	Div()[0][3][0].DivSize(6);
	Div()[0][3][1] = { volumeVal, Div::Alignment::Center, Div::AUTO, true };
	Div()[0][2] = { 4, Div::Alignment::Horizontal, 3, false, 32 };
	Div()[0][2][0].DivSize(2);
	Div()[0][2][1] = { mono, Div::Alignment::Center, Div::AUTO, true };
	Div()[0][2][2] = { mute, Div::Alignment::Center, Div::AUTO, true };
	Div()[0][2][3].DivSize(2);
	Div()[0][1] = { 3, Div::Alignment::Horizontal, 2, false, 22 };
	Div()[0][1][0].DivSize(2);
	Div()[0][1][1] = { pan, Div::Alignment::Center, Div::AUTO, true };
	Div()[0][1][2].DivSize(2);
	Div()[0][0] = { route, Div::Alignment::Center, 25, true };
	Div()[1] = { m_SubChannels, Div::Alignment::Left, Div::AUTO, false };
	CalcWidth();
}

void EndpointChannel::Process()
{
	std::lock_guard<std::mutex> _{ m_Lock };

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
	int prev = Width();
	if (Lines() <= 4)
		Width(70 + m_SubChannels.Width());
	else
		Width(Lines() * 7 + 42 + m_SubChannels.Width());
	if (prev != Width())
		LayoutManager().Refresh();
}

void EndpointChannel::Update(const Vec4<int>& v)
{
	CalcWidth();
	m_SubChannels.Height(Height());
	ChannelBase::Update(v);
}

EndpointChannel::operator nlohmann::json()
{
	nlohmann::json _json = ChannelBase::operator nlohmann::json();

	std::vector<int> _channels{};
	for (auto& i : m_Endpoints)
		_channels.push_back(i->id);

	_json["channels"] = _channels;
	_json["virtual"] = m_SubChannels.m_VirtualChannel;

	return _json;
};

void EndpointChannel::operator=(const nlohmann::json& json) 
{
	ChannelBase::operator=(json);
	m_SubChannels.m_VirtualChannel = json.at("virtual").get<int>();
};
