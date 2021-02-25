#pragma once
#include "pch.hpp"

template<typename Graphics>
class SliderBase : public ScrollbarBase
{
public:
	SliderBase()
	{
		m_Listener += [this](Event::MousePressed& e)
		{
			if (e.button == Event::MouseButton::LEFT)
			{
				m_PressMouse = Vertical() ? e.y : e.x;
				m_PressValue = m_Value;

				m_Dragging = true;
				m_NeedsRedraw = true;
			}
		};

		m_Listener += [this](Event::MouseReleased& e)
		{
			m_Dragging = false;
		};

		m_Listener += [this](Event::MouseMoved& e)
		{
			m_Mouse = Vertical() ? e.y : e.x;
		};

		m_Listener += [this](Event::MouseDragged& e)
		{
			if (e.button != Event::MouseButton::LEFT || !m_Dragging)
				return;
			
			m_NeedsRedraw = true;
			if (Vertical())
			{
				m_Mouse = constrain(e.y, Y(), Height());
				m_Value = m_PressValue - ((e.y - m_PressMouse) / (float)(Height())) * (m_Range.end - m_Range.start) + m_Range.start;
				m_Value = constrain(m_Value, m_Range.start, m_Range.end - m_VisibleRange);	
			}
			else
			{
				m_Mouse = constrain(e.x, X(), Width());
				m_Value = m_PressValue - ((e.x - m_PressMouse) / (float)(Width())) * (m_Range.end - m_Range.start) + m_Range.start;
				m_Value = constrain(m_Value, m_Range.start, m_Range.end - m_VisibleRange);
			}
		};

		m_Listener += [this](Event::MouseClicked& e)
		{
			if (m_Counter > 0) // Double Click
				SliderValue(m_ResetValue);

			if (e.button == Event::MouseButton::LEFT)
				m_Counter = 20;
		};
	}

	virtual double SliderValue() { return Value(); };
	virtual void   SliderValue(double v) { Value(v); };
	virtual std::string& ValueText() = 0;
	bool   Vertical() { return m_Vertical; }
	void   Vertical(bool v) { m_Vertical = v; }
	void   ResetValue(double v) { m_ResetValue = v; }

	void Render(CommandCollection& d) override
	{
		if (m_Counter > 0)
			m_Counter--;

		Graphics::Render(*this, d);
	};

private:
	bool m_Vertical = true;
	bool m_Dragging = false;
	int m_Counter = 0;
	double m_ResetValue = 0;
};

