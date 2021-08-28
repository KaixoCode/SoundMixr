#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"
#include "audio/ForwardChannel.hpp"

class SubChannelCollection : public Panel
{
public:
	SubChannelCollection()
		: Panel()
	{}

	void Update(const Vec4<int>& v) override
	{
		int x = 8;
		for (auto i : m_Channels)
		{
			i->Height(Height() - 28);
			i->X(x);
			i->Y(0);
			x += i->Width() + 8;
		}
		if (m_VirtualChannel == -1)
			Width(0);
		else
			Width(x - 8);
		Panel::Update(v);
	}

	void Render(CommandCollection& d) override
	{
		d.Command<Graphics::Fill>(ThemeT::Get().window_panel);
		d.Command<Graphics::Quad>(Vec4<int>{ X(), Y(), Width(), Height() - 20 });
		d.Command<Graphics::PushMatrix>();
		d.Command<Graphics::Translate>(Vec2<int>{ X(), Y() });
		for (auto i : m_Channels)
		{
			i->Render(d);
		}
		d.Command<Graphics::PopMatrix>();
	}

	void UpdateEffects()
	{
		for (auto& i : m_Channels)
		{
			i->EffectChain().UpdateEffects();
		}
	}

	/**
	 * Emplace a new channel.
	 * @tparam Type type of channel
	 * @tparam Args arguments to construct the channel
	 */
	template<typename Type, typename ... Args>
	Type& EmplaceChannel(Args&& ... args)
	{
		auto _c = std::make_unique<Type>(std::forward<Args>(args)...);

		ChannelBase* _channel = nullptr;
		_channel = reinterpret_cast<ChannelBase*>(m_Components.emplace_back(std::move(_c)).get());

		if (_channel)
			m_Channels.emplace_back(_channel);

		return *static_cast<Type*>(_channel);
	}    


	virtual operator nlohmann::json() 
	{
		nlohmann::json _json = nlohmann::json::object();

		_json["devices"] = nlohmann::json::array();
		for (auto& i : m_Channels)
			_json["devices"].push_back(i->operator nlohmann::json());

		return _json;
	};

	virtual void operator=(const nlohmann::json& json)
	{
		bool show = false;
		for (auto& i : json.at("devices"))
		{
			show = true;
			auto& _c = EmplaceChannel<ForwardChannel>(m_Type);
			_c.m_VirtualDevice = m_VirtualChannel;
			_c = i;
			_c.OpenStream();
		}
		Visible(show && m_VirtualChannel != -1);
		if (!(show && m_VirtualChannel != -1))
			Width(0);
	}
	
	// Vector of pointers to all channels
	std::vector<ChannelBase*> m_Channels;
	int m_VirtualChannel = -1;
	int m_Type;
};

/**
 * Endpoint channel, for input and outputs provided by the opened asio device, 
 * a channel contains 1 or more endpoints, the endpoints are always sorted on their 
 * ids.
 */
class EndpointChannel : public ChannelBase
{
public:
	EndpointChannel(ChannelType type);

	/**
	 * Processing for the endpoint channel happens as follows:
	 *  - Get the levels from the endpoints
	 *  - Apply effectchain, panning, volume, mute, and mono.
	 *  - Forward to all connections.
	 */
	virtual void Process() override;

	/**
	 * Add an endpoint to this channel. Increments the line 
	 * count of this Channel as well.
	 * @param e endpoint
	 */
	void AddEndpoint(Endpoint* e);

	/**
	 * Remove an endpoint from this channel. Decrements the line
	 * count of this Channel as well.
	 * @param e endpoint
	 */
	void RemoveEndpoint(Endpoint* e);

	/**
	 * Sorts the endpoints.
	 */
	void SortEndpoints();

	/**
	 * Calculates predetermined width given the amount of endpoints.
	 */
	void CalcWidth();

	/**
	 * Get all endpoints in this EndpointChannel.
	 * @return endpoints
	 */
	auto Endpoints() -> std::vector<Endpoint*>& { return m_Endpoints; }

	void Update(const Vec4<int>& v);

	virtual bool Hovering() const override
	{
		return ChannelBase::Hovering() && !m_SubChannels.Hovering();
	}

	virtual operator nlohmann::json() override;
	virtual void operator=(const nlohmann::json& json) override;

private:
	SubChannelCollection& m_SubChannels;

	std::vector<Endpoint*> m_Endpoints;

	Stream<Api::Wasapi> m_Physical;

	friend class Audio;
};