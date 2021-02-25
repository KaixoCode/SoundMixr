#include "ui/ListPanel.hpp"

// -------------------------------------------------------------------------- \\
// ---------------------------- List Panel ---------------------------------- \\
// -------------------------------------------------------------------------- \\

ListPanel::ListPanel(AsioDevice& sarasio)
	: asio(sarasio), 
	m_Inputs(Panel<::Panel>().Emplace<::Panel>()),
	m_Divider(&Panel().Emplace<MenuAccessories::VerticalDivider>(1, 2, 4, 0)),
	m_Outputs(Panel().Emplace<::Panel>())
{
	m_Inputs.AutoResize(true, false);
	m_Inputs.Layout<Layout::SidewaysStack>(8);
	
	m_Outputs.AutoResize(true, false);
	m_Outputs.Layout<Layout::SidewaysStack>(8);

	Background(Color{ 38, 38, 38, 355 });
	EnableScrollbars(true, false);

	m_Listener += [this](Event::MousePressed& e)
	{
		Vec2<int> translated = Vec2<int>{ 
			e.x + (m_ScrollbarX->NotNecessary() ? 0 : m_ScrollbarX->Value()),
			e.y - (m_ScrollbarX->NotNecessary() ? 0 : m_ScrollbarX->Height()) - (m_ScrollbarY->NotNecessary() ? 0 : Component().Y()) };

		if (e.button == Event::MouseButton::LEFT)
		{
			bool s = false;
			for (auto& _panel : m_InputChannels)
			{
				InputChannelPanel& _current = *_panel;
				if (_current.WithinBounds(translated - m_Inputs.Position()))
				{
					if ((!_current.routed.Hovering() || _current.routed.Disabled()) && !_current.muted.Hovering() && !_current.mono.Hovering() && !_current.pan.Hovering())
					{
						for (auto& _p : m_OutputChannels)
						{
							OutputChannelPanel& _c = *_p;
							_c.Select(&_current.Channels());
							_c.Selected(false);
						}
						for (auto& _p : m_InputChannels)
						{
							InputChannelPanel& _c = *_p;
							_c.Select(&_current.Channels());
							_c.Selected(false);
						}
						_current.Selected(true);
					}
					s = true;
				}
			}
			for (auto& _panel : m_OutputChannels)
			{
				ChannelPanel<::OutputChannels>& _current = *_panel;
				if (_current.WithinBounds(translated - m_Outputs.Position()))
				{
					if ((!_current.routed.Hovering() || _current.routed.Disabled()) && !_current.muted.Hovering() && !_current.mono.Hovering() && !_current.pan.Hovering())
					{
						for (auto& _p : m_InputChannels)
						{
							ChannelPanel<::InputChannels>& _c = *_p;
							_c.Select(&_current.Channels());
							_c.Selected(false);
						}
						for (auto& _p : m_OutputChannels)
						{
							OutputChannelPanel& _c = *_p;
							_c.Select(&_current.Channels());
							_c.Selected(false);
						}
						_current.Selected(true);
					}
					s = true;
				}
			}
			if (!s && !m_ScrollbarX->Hovering() && !m_ScrollbarY->Hovering())
			{
				for (auto& _p : m_InputChannels)
				{
					_p->Selected(false);
					_p->Unselect();
				}
				for (auto& _p : m_OutputChannels)
				{
					_p->Selected(false);
					_p->Unselect();
				}
			}
		}
	};

	m_Listener += [this](Event::Unfocused& e)
	{
		if (!m_ScrollbarX->Hovering() && !m_ScrollbarY->Hovering())
		{
			for (auto& _p : m_InputChannels)
			{
				_p->Selected(false);
				_p->Unselect();
			}
			for (auto& _p : m_OutputChannels)
			{
				_p->Selected(false);
				_p->Unselect();
			}
		}
	};
}

void ListPanel::Vertical()
{
	EnableScrollbars(false, true);
	for (auto& _p : m_InputChannels)
		_p->Vertical();

	for (auto& _p : m_OutputChannels)
		_p->Vertical();

	if (m_Divider)
		m_Divider->Height(1);
}

void ListPanel::Horizontal()
{
	EnableScrollbars(true, false);
	for (auto& _p : m_InputChannels)
		_p->Horizontal();

	for (auto& _p : m_OutputChannels)
		_p->Horizontal();

	if (m_Divider)
		m_Divider->Width(10);
}

void ListPanel::ResetGrouping()
{
	m_Inputs.Clear();
	m_Outputs.Clear();
	m_InputChannels.clear();
	m_OutputChannels.clear();

	for (auto& i : asio.Inputs())
	{
		auto& panl = m_InputChannels.emplace_back(&m_Inputs.Emplace<InputChannelPanel>(this));
		panl->SmartPanel(true);
		panl->AddChannel(&i);
	}

	for (auto& i : asio.Outputs())
	{
		auto& panl = m_OutputChannels.emplace_back(&m_Outputs.Emplace<OutputChannelPanel>(this));
		panl->SmartPanel(true);
		panl->AddChannel(&i);
	}
}