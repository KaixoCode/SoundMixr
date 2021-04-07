#pragma once
#include "pch.hpp"
#include "ui/Components.hpp"
#include "ui/Graphics.hpp"
#include "ui/Dropdown.hpp"
#include "EffectLoader.hpp"

class Effect : public Panel
{
public:
	Effect(Effects::EffectBase* effect)
		: m_Effect(effect)
	{
		Init();
		m_Name = effect->Name();

		m_RealHeight = -1;
		m_MinimB = &Emplace<Button<NOTHING, ButtonType::Toggle>>(&m_Small, "");
		m_Enable = &Emplace<Button<ToggleButtonGraphics, ButtonType::Toggle>>(&m_Enabled, "");
		m_Enable->Size({ 18, 18 });
		m_MinimB->Size({ 24, 24 });

		m_Listener += [this](Event::MouseMoved& e)
		{
			m_HoveringDrag = false;
			if (e.x > 24 && e.y > Height() - 24 && e.x < Width() - 24)
				m_HoveringDrag = true;
		};

		m_Listener += [this](Event& e)
		{
			if (e.x - X() >= 24 && e.x - X() <= Width() - 24 && m_Small && e.type != Event::Type::KeyPressed && e.type != Event::Type::KeyReleased)
				e.y = Y() + 1;
		};
	}

	float NextSample(float s, int c)
	{
		return m_Effect->NextSample(s, c);
	}

	void Bypass(bool b)
	{
		m_Bypass = b;
		if (!m_Bypass)
		{
			m_Enable->Enable();
			if (!m_Enabled)
				return;

			for (auto& i : Components())
				if (auto p = dynamic_cast<ParameterBase*>(i.get()))
					p->Enable();
				else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
					p->Enable();
			m_Effect->Update();
		}
		else
		{
			for (auto& i : Components())
				if (i.get() == m_MinimB)
					continue;
				else if (auto p = dynamic_cast<ParameterBase*>(i.get()))
					p->Disable();
				else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
					p->Disable();
		}
	}

	void Lines(int c)
	{
		m_Effect->Channels(c);
	}

	bool HoveringDrag() { return m_HoveringDrag; }

	void UpdateEffect()
	{
		m_Effect->Update();
	}

	void Update(const Vec4<int>& v)
	{
		if (m_Enabled && !m_PEnabled && !m_Bypass)
		{
			m_PEnabled = true;
			for (auto& i : Components())
				if (auto p = dynamic_cast<ParameterBase*>(i.get()))
					p->Enable();
				else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
					p->Enable();
			m_Effect->Update();
		}
		else if (!m_Enabled && m_PEnabled && !m_Bypass)
		{
			m_PEnabled = false;
			for (auto& i : Components())
				if (i.get() == m_Enable || i.get() == m_MinimB)
					continue;
				else if (auto p = dynamic_cast<ParameterBase*>(i.get()))
					p->Disable();
				else if (auto p = dynamic_cast<ButtonBase*>(i.get()))
					p->Disable();
		}

		if (m_Small && !m_PSmall)
		{
			m_PSmall = true;
			m_RealHeight = Height();
			Height(24);
		}
		else if (!m_Small && m_PSmall)
		{
			m_PSmall = false;
			if (m_RealHeight == -1)
				m_RealHeight = Height();

			Height(m_RealHeight);
		}


		m_Size.height = m_Small ? 24 : m_Effect->Height() + 24;
		m_Enable->Position({ 3, Height() - 21 });
		m_MinimB->Position({ Width() - 24, Height() - 24 });
		Background(ThemeT::Get().effect_background);
		Panel::Update(v);
	}

	void Render(CommandCollection& d)
	{
		using namespace Graphics;
		if (!m_Small)
			Panel::Render(d);

		d.Command<PushMatrix>();
		d.Command<Translate>(Position());
		d.Command<Fill>(ThemeT::Get().effect_title_bar);
		d.Command<Quad>(Vec4<int>{ 0, Height() - 24, Width(), 24 });
		d.Command<Font>(Fonts::Gidole16, 16.0f);

		if (m_Enabled)
			d.Command<Fill>(ThemeT::Get().effect_title_text);
		else
			d.Command<Fill>(ThemeT::Get().effect_title_text_off);

		d.Command<TextAlign>(Align::LEFT, Align::CENTER);
		d.Command<Text>(&m_Name, Vec2<int>{ 30, Height() - 12});

		d.Command<Fill>(ThemeT::Get().effect_minimize_button);
		if (!m_Small)
			d.Command<Quad>(Vec4<int>{Width() - 17, Height() - 13, 10, 2});
		else
			d.Command<Triangle>(Vec4<int>{Width() - 12, Height() - 12, 10, 10}, 270.0f);

		m_Enable->Render(d);
		m_MinimB->Render(d);

		if (!m_Small)
		{
			d.Command<Fill>(ThemeT::Get().effect_divider);
			for (auto& i : m_Dividers)
				d.Command<Quad>(i);
		}

		d.Command<PopMatrix>();
	}

	operator nlohmann::json()
	{
		nlohmann::json _json = *m_Effect;
		_json["enabled"] = m_Enable->Active();
		_json["small"] = m_Small;
		_json["preset"] = m_Preset.string();
		_json["type"] = m_Name;
		return _json;
	}

	void operator=(const nlohmann::json& json)
	{
		m_Enable->Active(json.at("enabled").get<bool>());
		m_Small = json.at("small").get<bool>();
		m_Preset = json.at("preset").get<std::string>();

		if (m_Preset != "")
		{
			m_Name = m_Effect->Name() + " - " + m_Preset.stem().string();

			m_SavePreset->Enable();
		}

		*m_Effect = json;
	}

private:	
	int m_RealHeight = 0,
		m_Delete = false;

	bool m_PSmall = false,
		m_Small = false,
		m_HoveringDrag = false,
		m_PEnabled = true,
		m_Enabled = true,
		m_Bypass = false;

	Effects::EffectBase* m_Effect;
	Component* m_PrevObj = nullptr;
	std::vector<Vec4<int>> m_Dividers;

	Button<SoundMixrGraphics::Menu, ButtonType::Normal>* m_SavePreset;
	Button<ToggleButtonGraphics, ButtonType::Toggle>* m_Enable;
	Button<NOTHING, ButtonType::Toggle>* m_MinimB;

	std::filesystem::path m_Preset{ "" };
	std::string m_Name;

	std::unordered_map<int, int> m_RadioButtonKeys;
	std::unordered_map<int, std::vector<Effects::RadioButton*>> m_RadioButtons;


	void Effect::Init()
	{
		InitDiv(m_Effect->Div(), { 0, 0, 300, m_Effect->Height() });
	}

	void Effect::InitDiv(Effects::Div& div, const Vec4<int>& dim)
	{
		if (div.DivType() == Effects::Div::Type::Object)
			SetObject(div, dim);

		else
			if (div.Align() == Effects::Div::Alignment::Horizontal)
			{
				// Get all the sizes of the sub-divs
				std::vector<int> sizes;
				int width = dim.width, amt = 0, obamt = 0;
				for (auto& i : div.Divs())
					if (i->DivSize() == Effects::Div::AUTO)
					{
						if (i->DivType() == Effects::Div::Type::Object)
							sizes.push_back(i->Object().Size().width + i->Padding()), width -= i->Object().Size().width + i->Padding(), obamt++;
						else
							sizes.push_back(0), amt++;
					}
					else
						width -= i->DivSize(), sizes.push_back(-i->DivSize());

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
					if (i->DivSize() == Effects::Div::AUTO)
					{
						if (i->DivType() == Effects::Div::Type::Object)
							sizes.push_back(i->Object().Size().height + i->Padding()), height -= i->Object().Size().height + i->Padding(), obamt++;
						else
							sizes.push_back(0), amt++;
					}
					else
						height -= i->DivSize(), sizes.push_back(-i->DivSize());

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

	void Effect::SetObject(Effects::Div& div, const Vec4<int>& dim)
	{
		// Get the object from the div
		Effects::Object* object = &div.Object();

		// Calculate the position using the alignment
		Vec2<int> position = { dim.x, dim.y };
		if (div.Align() == Effects::Div::Alignment::Center)
			position += { dim.width / 2 - object->Size().width / 2, dim.height / 2 - object->Size().height / 2 };
		else if (div.Align() == Effects::Div::Alignment::Right)
			position += { dim.width - object->Size().width, dim.height / 2 - object->Size().height / 2 };
		else if (div.Align() == Effects::Div::Alignment::Left)
			position += { 0, dim.height / 2 - object->Size().height / 2 };
		else if (div.Align() == Effects::Div::Alignment::Bottom)
			position += { dim.width / 2 - object->Size().width / 2, 0 };
		else if (div.Align() == Effects::Div::Alignment::Top)
			position += { dim.width / 2 - object->Size().width / 2, dim.height - object->Size().height };


		// Determine type and add to effect.
		Component* ob = nullptr;
		auto xy = dynamic_cast<Effects::XYController*>(object);
		if (xy != nullptr)
		{
			ob = &Emplace<XYController>(*xy), xy->Position({ position.x, position.y });
			goto theback;
		}

		auto fc = dynamic_cast<Effects::FilterCurve*>(object);
		if (fc != nullptr)
		{
			ob = &Emplace<FilterCurve>(*fc), fc->Position({ position.x, position.y });
			goto theback;
		}

		auto sc = dynamic_cast<Effects::SimpleFilterCurve*>(object);
		if (sc != nullptr)
		{
			ob = &Emplace<SimpleFilterCurve>(*sc), sc->Position({ position.x, position.y });
			goto theback;
		}

		auto rb = dynamic_cast<Effects::RadioButton*>(object);
		if (rb != nullptr)
		{
			ob = &Emplace<RadioButton>(*rb, m_RadioButtonKeys, m_RadioButtons), rb->Position({ position.x, position.y });
			goto theback;
		}

		auto dy = dynamic_cast<Effects::DynamicsSlider*>(object);
		if (dy != nullptr)
		{
			ob = &Emplace<DynamicsSlider>(*dy), dy->Position({ position.x, position.y });
			goto theback;
		}

		auto vs = dynamic_cast<Effects::VolumeSlider*>(object);
		if (vs != nullptr)
		{
			ob = &Emplace<VolumeSlider>(*vs), vs->Position({ position.x, position.y - 5 });
			goto theback;
		}

		auto param = dynamic_cast<Effects::Parameter*>(object);
		if (param != nullptr)
		{
			if (param->Type() == Effects::ParameterType::Slider)
				ob = &Emplace<Slider>(*param), param->Position({ position.x, position.y });
			else if (param->Type() == Effects::ParameterType::Knob)
				ob = &Emplace<Knob>(*param), param->Position({ position.x, position.y });
			goto theback;
		}

		auto dd = dynamic_cast<Effects::DropDown*>(object);
		if (dd != nullptr)
		{
			ob = &Emplace<DropDown<int, DropdownButton>>(*dd), dd->Position({ position.x, position.y });
			goto theback;
		}

		auto toggle = dynamic_cast<Effects::ToggleButton*>(object);
		if (toggle != nullptr)
		{
			ob = &Emplace<ToggleButton>(*toggle), toggle->Position({ position.x, position.y });
			goto theback;
		}

	theback:

		if (ob == nullptr)
			return;

		if (m_PrevObj)
		{
			m_PrevObj->TabComponent(ob);
			ob->BackTabComponent(m_PrevObj);
		}

		m_PrevObj = ob;
	}
};