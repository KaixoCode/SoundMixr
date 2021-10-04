#pragma once
#include "pch.hpp"


namespace Audio
{
	struct Parameter
	{
		float value = 0;
		struct MidiLink
		{
			int device = -1;
			int channel = -1;
			int cc = -1;
		} midi;
	
		Parameter& operator=(float v) { value = v; return *this; }
		operator float() const { return value; }

		operator nlohmann::json()
		{
			nlohmann::json _json = nlohmann::json::object();
			_json["value"] = value;
			nlohmann::json _midi = nlohmann::json::object();
			_midi["device"] = midi.device;
			_midi["channel"] = midi.channel;
			_midi["cc"] = midi.cc;
			_json["midi"] = _midi;
			return _json;
		}

		Parameter& operator=(const nlohmann::json& _json)
		{
			value = _json.at("value").get<float>();
			midi.device = _json.at("midi").at("device").get<int>();
			midi.channel = _json.at("midi").at("channel").get<int>();
			midi.cc = _json.at("midi").at("cc").get<int>();
			return *this;
		}
	};
}