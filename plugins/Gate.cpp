#define EFFECT_PLUGIN
#include "Base.hpp"

namespace SoundMixr
{
	/**
	 * Simple Volume effect example.
	 */
	class Gate : public EffectBase
	{
	public:
		Gate()
			:
			// Initialize the volume knob
			m_MinVolume(Parameter("Min", ParameterType::Knob)),
			m_MaxVolume(Parameter("Max", ParameterType::Knob)),
			EffectBase("Gate")
		{
			// Set the height of the effect to 80.
			Height(80);

			// Some settings for the volume knob
			m_MinVolume.Size({ 30, 30 });
			m_MinVolume.Range({ 0, 100 });
			m_MinVolume.Unit("%");
			m_MinVolume.Decimals(1);
			m_MinVolume.ResetValue(100);
			m_MinVolume.ResetValue();
			m_MinVolume.Multiplier(0.4);

			m_MaxVolume.Size({ 30, 30 });
			m_MaxVolume.Range({ 0, 100 });
			m_MaxVolume.Unit("%");
			m_MaxVolume.Decimals(1);
			m_MaxVolume.ResetValue(100);
			m_MaxVolume.ResetValue();
			m_MaxVolume.Multiplier(0.4);

			// Put the volume knob in the center of the entire Div.
			Div() = { 2, Div::Alignment::Horizontal, 0, false, Div::AUTO };
			Div()[0] = m_MinVolume;
			Div()[1] = m_MaxVolume;
		}

		void Update() override
		{
			max_volume = m_MaxVolume.Value() * 0.01;
			min_volume = m_MinVolume.Value() * 0.01;
		}

		float Process(float in, int c) override
		{
			static int counter = 0;
			static float current_change = state ? max_volume : min_volume;

			float sample = in * current_change;

			if (++counter > SampleRate() / changes) {
				state = !state;
				current_change = state ? max_volume : min_volume;
				counter = 0;
			}

			return sample;
		}

	private:

		// The volume parameter
		SoundMixr::Parameter& m_MinVolume;
		SoundMixr::Parameter& m_MaxVolume;
		float max_volume;
		float min_volume;
		int changes = 4;
		bool state = true;
	};
}

/**
 * MAKE SURE TO ADD THIS METHOD!!
 * This allows the Gui to create an instance of the effect.
 */
extern "C" DLLDIR void* NewInstance()
{
	return new SoundMixr::Gate();
}