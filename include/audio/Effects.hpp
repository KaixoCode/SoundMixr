#pragma once
#include "pch.hpp"
#include "ui/VolumeSlider.hpp"
#include "ui/Graphics.hpp"
#include "ui/Dropdown.hpp"

struct NOTHING
{
	static void Render(ButtonBase&, CommandCollection&) {}
};

// -------------------------------------------------------------------------- \\
// ------------------------------ Effect ------------------------------------ \\
// -------------------------------------------------------------------------- \\

class Effect : public Panel
{
public:
	static inline double sampleRate = 48000;

	Effect(EffectBase*);

	void Render(CommandCollection& d) override;
	void Update(const Vec4<int>& v) override;
	
	float NextSample(float s, int c) { return m_Effect->NextSample(s, c); };
	void Channels(int c) { m_Effect->Channels(c); m_Channels = c; }

	bool Hovering() { return m_Hovering; }
	bool HoveringDrag() { return m_HoveringDrag; }

	operator json() 
	{ 
		json _json = *m_Effect;
		_json["enabled"] = m_Enable->Active();
		_json["small"] = m_Minim->Active();
		return _json;
	}

	void operator=(const json& json) 
	{ 
		m_Enable->Active(json.at("enabled").get<bool>());
		m_Minim->Active(json.at("small").get<bool>());
		m_MinimB->Active(m_Small);
		*m_Effect = json;
	}

	bool Delete() { return m_Delete; }

protected:
	int m_Channels = -1, m_RealHeight = 0, m_Delete = false;
	bool m_Hovering = false, m_Small = false, m_HoveringDrag = false, m_Enabled = true;
	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> m_Menu;
	MenuAccessories::Divider* m_Div, * m_Div2;
	Button<SoundMixrGraphics::Menu, ButtonType::Toggle>* m_Minim;
	Button<ToggleButtonG, ButtonType::Toggle>* m_Enable;
	Button<NOTHING, ButtonType::Toggle>* m_MinimB;
	EffectBase* m_Effect;
	std::vector<Vec4<int>> m_Dividers;

	void Init()
	{
		InitDiv(m_Effect->Div(), { 0, 0, 300, m_Effect->Height()});
	}

	void InitDiv(EffectLayout::Div& div, const Vec4<int>& dim)
	{
		if (div.DivType() == EffectLayout::Div::Type::OBJECT)
			SetObject(div, dim);

		else 
			if (div.CellType() == EffectLayout::Type::COLS)
			{
				// Get all the sizes of the sub-divs
				std::vector<int> sizes;
				int width = dim.width, amt = 0, obamt = 0;
				for (auto& i : div.Divs())
					if (i->CellSize() == EffectLayout::Div::AUTO)
					{
						if (i->DivType() == EffectLayout::Div::Type::OBJECT)
							sizes.push_back(i->Object().Size().width + i->Padding()), width -= i->Object().Size().width + i->Padding(), obamt++;
						else
							sizes.push_back(0), amt++;
					} 
					else
						width -= i->CellSize(), sizes.push_back(-i->CellSize());

				// See what's left to divide
				int x = dim.x;
				int w = width;
				if (amt)
					w /= amt;
				else if (obamt)
					w /= obamt;

				// If there's some space left, either give it to divs or objects
				if (w > 0)
					for (auto& i : sizes)
						if (amt && i == 0) // Give space to div with no given size
							i = w;
						else if (obamt && i > 0) // Give space to object
							i += w;

				// if we're short on space, take some away from objects or divs.
				if (width < 0)
					for (auto& i : sizes)
						if (i > 0 && obamt) // Take away from object
							i -= w;
						else if (i < 0 && !obamt) // Take away from div with a given size
							i -= w;

				// Set sizes and positions
				for (int i = 0; i < div.Divs().size(); i++)
				{
					if (div.Dividers() && i != 0)
						m_Dividers.emplace_back(Vec4<int>{ x, dim.y + 8, 1, dim.height - 16 });

					if (sizes[i] < 0) // If div with given space, size is negative so negate
						InitDiv(*div.Divs()[i], { x + div.Padding(), dim.y + div.Padding(), -sizes[i] - 2 * div.Padding(), dim.height - 2 * div.Padding() }), x += -sizes[i];
					else // Otherwise just recurse
						InitDiv(*div.Divs()[i], { x + div.Padding(), dim.y + div.Padding(), sizes[i] - 2 * div.Padding(), dim.height - 2 * div.Padding() }), x += sizes[i];
				}
			}
			else
			{
				// Get all the sizes of the sub-divs
				std::vector<int> sizes;
				int height = dim.height, amt = 0, obamt = 0;
				for (auto& i : div.Divs())
					if (i->CellSize() == EffectLayout::Div::AUTO)
					{
						if (i->DivType() == EffectLayout::Div::Type::OBJECT)
							sizes.push_back(i->Object().Size().height + i->Padding()), height -= i->Object().Size().height + i->Padding(), obamt++;
						else
							sizes.push_back(0), amt++;
					}
					else
						height -= i->CellSize(), sizes.push_back(-i->CellSize());

				// See what's left to divide
				int y = dim.y;
				int h = height;
				if (amt)
					h /= amt;
				else if (obamt)
					h /= obamt;

				// If there's some space left, either give it to divs or objects
				if (h > 0)
					for (auto& i : sizes)
						if (amt && i == 0) // Give space to div with no given size
							i = h;
						else if (obamt && i > 0) // Give space to object
							i += h;

				// if we're short on space, take some away from objects or divs.
				if (height < 0)
					for (auto& i : sizes)
						if (i > 0 && obamt) // Take away from object
							i += h;
						else if (i < 0 && !obamt) // Take away from div with a given size
							i += h;

				// Set sizes and positions
				for (int i = 0; i < div.Divs().size(); i++)
				{
					if (div.Dividers() && i != 0)
						m_Dividers.emplace_back(Vec4<int>{ dim.x + 8, y, dim.width - 16, 1 });
					
					if (sizes[i] < 0) // If div with given space, size is negative so negate
						InitDiv(*div.Divs()[i], { dim.x + div.Padding(), y + div.Padding(), dim.width - 2 * div.Padding(), -sizes[i] - 2 * div.Padding() }), y += -sizes[i];
					else // Otherwise just recurse
						InitDiv(*div.Divs()[i], { dim.x + div.Padding(), y + div.Padding(), dim.width - 2 * div.Padding(), sizes[i] - 2 * div.Padding() }), y += sizes[i];
				}
			}
	}

	void SetObject(EffectLayout::Div& div, const Vec4<int>& dim)
	{
		// Get the object from the div
		EffectObject* object = &div.Object();

		// Calculate the position using the alignment
		Vec2<int> position = { dim.x, dim.y };
		if (div.Align() == EffectLayout::Align::CENTER)
			position += { dim.width / 2 - object->Size().width / 2, dim.height / 2 - object->Size().height / 2 };
		else if (div.Align() == EffectLayout::Align::RIGHT)
			position += { dim.width - object->Size().width, dim.height / 2 - object->Size().height / 2 };
		else if (div.Align() == EffectLayout::Align::LEFT)
			position += { 0, dim.height / 2 - object->Size().height / 2 };
		else if (div.Align() == EffectLayout::Align::BOTTOM)
			position += { dim.width / 2 - object->Size().width / 2, 0 };
		else if (div.Align() == EffectLayout::Align::TOP)
			position += { dim.width / 2 - object->Size().width / 2, dim.height - object->Size().height };


		// Determine type and add to effect.
		auto xy = dynamic_cast<XYController*>(object);
		if (xy != nullptr)
		{
			Emplace<XYControllerComponent>(*xy), xy->Position({ position.x, position.y });
			return;
		}

		auto rb = dynamic_cast<RadioButton*>(object);
		if (rb != nullptr)
		{
			Emplace<RadioButtonComponent>(*rb), rb->Position({ position.x, position.y });
			return;
		}

		auto dy = dynamic_cast<DynamicsObject*>(object);
		if (dy != nullptr)
		{
			Emplace<DynamicsSlider>(*dy), dy->Position({ position.x, position.y });
			return;
		}

		auto vs = dynamic_cast<VolumeSlider*>(object);
		if (vs != nullptr)
		{
			Emplace<VolumeSliderComponent>(*vs), vs->Position({ position.x, position.y - 5 });
			return;
		}

		auto param = dynamic_cast<Parameter*>(object);
		if (param != nullptr)
		{
			if (param->Type() == ParameterType::Slider)
				Emplace<NormalSlider>(*param), param->Position({ position.x, position.y });
			else if (param->Type() == ParameterType::Knob)
				Emplace<KnobSlider>(*param), param->Position({ position.x, position.y });
			else if (param->Type() == ParameterType::TwoWaySlider)
				Emplace<NormalSlider>(*param), param->Position({ position.x, position.y });
			return;
		}

		auto dd = dynamic_cast<DropDown*>(object);
		if (dd != nullptr)
		{
			Emplace<DropDownComponent<int, DropdownButton>>(*dd), dd->Position({ position.x, position.y });
			return;
		}

		auto toggle = dynamic_cast<ToggleButton*>(object);
		if (toggle != nullptr)
		{
			Emplace<ToggleButtonComponent>(*toggle), toggle->Position({ position.x, position.y });
			return;
		}
	}
};

// -------------------------------------------------------------------------- \\
// --------------------------- Effect Group --------------------------------- \\
// -------------------------------------------------------------------------- \\

class EffectsGroup : public Panel
{
public:
	EffectsGroup();
	~EffectsGroup();

	bool  Lock()	const { if (!m_Dead) m_Mutex.lock(); return !m_Dead; }
	void  Unlock()	const { m_Mutex.unlock(); }
	float NextSample(float a, int ch);
	void  Channels(int channels);
	bool  Hovering();
	bool  Dragging() { return m_Dragging != nullptr; }

	Effect& Add(EffectBase* e)
	{
		int p = m_EffectCount;
		m_EffectCount = 0;
		auto& a = m_Effects.emplace_back(std::make_unique<Effect>(e));
		e->Channels(m_Channels);
		m_EffectCount = p + 1;
		return *a;
	}

	void Update(const Vec4<int>& viewport) override;
	void Render(CommandCollection& d) override;

	int GetIndex(int y)
	{
		int index = 0;
		int _ri = 0;
		for (auto& i : m_Effects)
		{
			if (y < i->Y() + i->Height() / 2)
				_ri = index + 1;

			index++;
		}

		if (m_Dragging && y < m_Dragging->Y() + m_Dragging->Height() / 2)
			_ri -= 1;

		return _ri;
	}

	operator json();
	void operator=(const json& json);

private:
	std::vector<std::unique_ptr<Effect>> m_Effects;
	int m_Channels = 0, m_EffectCount = 0;

	Effect* m_Hovering;
	Effect* m_Focussed;

	bool m_Dead = false;

	mutable std::mutex m_Mutex;

	Effect* m_Dragging = nullptr;
	int m_InsertIndex = -1;

	double m_MouseY = 0;

	void Determine(Event& e);
};
