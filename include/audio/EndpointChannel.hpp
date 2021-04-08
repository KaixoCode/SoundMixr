#pragma once
#include "pch.hpp"
#include "audio/ChannelBase.hpp"
#include "audio/Asio.hpp"

/**
 * Endpoint channel, for input and outputs provided by the opened asio device, 
 * a channel contains 1 or more endpoints, the endpoints are always sorted on their 
 * ids.
 */
class EndpointChannel : public ChannelBase
{
public:
	using ChannelBase::ChannelBase;

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

	virtual operator nlohmann::json() override;
	virtual void operator=(const nlohmann::json& json) override;

private:
	std::vector<Endpoint*> m_Endpoints;
};