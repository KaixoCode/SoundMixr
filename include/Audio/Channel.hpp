#pragma once
#include "pch.hpp"
#include "Audio/Parameter.hpp"

namespace Audio
{
	struct Channel
	{
		static inline int id_counter = 0;
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

		Channel(int type);
		~Channel();

		std::string name;
		int counter = 0;
		int id = id_counter++;
		int lines = 0;
		bool mono = false;
		bool mute = false;
		Parameter pan{ 0 };
		Parameter volume{ 1 };
		const int type;
		std::vector<Pointer<Channel>> connections;
		mutable std::mutex lock;
		std::vector<float> levels;
		std::vector<float> peaks;
		std::vector<float> smoothed;
		std::vector<float> pans;

		virtual void Level(float s, int c);
		virtual void NextCycle();
		virtual void Process();

		void Connect(const Pointer<Channel>& c);
		void Disconnect(const Pointer<Channel>& c);
		bool Connected(const Pointer<Channel>& c) const;

		void UpdatePans();

		virtual operator nlohmann::json();
		virtual void operator=(const nlohmann::json& json);
	};

	struct Endpoint
	{
		const std::string name;
		int id;
		bool input;
		float sample = 0;
	};

	struct EndpointChannel : public Channel
	{
		std::vector<int> endpoints;

		EndpointChannel(bool input);
		void NextCycle() override;
		void Process() override;

		void Add(int id);
		void Remove(int id);
		bool Contains(int id);

		virtual operator nlohmann::json() override;
		virtual void operator=(const nlohmann::json& json) override;
	};
}