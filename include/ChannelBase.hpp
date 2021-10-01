#pragma once
#include "pch.hpp"

struct ChannelBase
{
	struct Type
	{
		enum
		{
			Input = 0x1,
			Output = 0x2,
			Generator = 0x4,
			SoundBoard = 0x8,
			Endpoint = 0x10,
			Forward = 0x20,
		};
	};

	ChannelBase(int type);
	~ChannelBase();

	int counter = 0;
	int id = 0;
	int lines = 0;
	bool mono = false;
	bool mute = false;
	float pan = 0;
	float volume = 1;
	const int type;
	std::vector<Pointer<ChannelBase>> connections;
	mutable std::mutex lock;
	std::vector<float> levels;
	std::vector<float> peaks;
	std::vector<float> smoothed;
	std::vector<float> pans;

	virtual void Level(float s, int c);
	virtual void NextCycle();
	virtual void Process();

	void Connect(const Pointer<ChannelBase>& c);
	void Disconnect(const Pointer<ChannelBase>& c);
	bool Connected(const Pointer<ChannelBase>& c) const;

	void UpdatePans();

	operator nlohmann::json();
	void operator=(const nlohmann::json& json);
};

struct Endpoint
{
	const std::string name;
	int id;
	bool input;
	float sample = 0;
};

struct EndpointChannel : public ChannelBase
{
	std::vector<int> endpoints;

	EndpointChannel(bool input);
	void NextCycle() override;
	void Process() override;

	void Add(const Pointer<Endpoint>& e);
	void Remove(const Pointer<Endpoint>& e);
	bool Contains(const Pointer<Endpoint>& e);
};