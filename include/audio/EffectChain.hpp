#pragma once
#include "pch.hpp"
#include "audio/Effect.hpp"
#include "ui/Graphics.hpp"

/**
 * Moves object at 'oldIndex' in vector 'v' to the 'newIndex' while maintaining order
 * of the other elements.
 * @param v vector
 * @param oldIndex old index
 * @param newIndex new index
 */
template <typename t> void move(std::vector<t>& v, size_t oldIndex, size_t newIndex)
{
	if (oldIndex > newIndex)
		std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
	else
		std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

/**
 * Panel that is stored inside of each channel, will be given to EffectPanel to
 * display it in the east panel of the AudioController.
 */
class EffectChain : public SMXRScrollPanel
{
public:

	/**
	 * Constructor.
	 * All layout and event handling.
	 */
	EffectChain();

	/**
	 * Setup the rightclick menu.
	 */
	void GenerateMenu();

	/**
	 * Override because we need to lock before clearing.
	 */
	void Clear() override;

	/**
	 * Add an effect to this chain.
	 * @param effect effectbase
	 */
	auto AddEffect(SoundMixr::EffectBase* effect) -> Effect&;

	/**
	 * Set the amount of lines in this chain. Resize all buffers etc.
	 * @param c count
	 */
	void Lines(int c);

	/**
	 * Set if this effect chain should be bypassed.
	 * @param v bypass
	 */
	void Bypass(bool v);

	/**
	 * Returns true if this effect chain is bypassed.
	 * @return true if bypassed
	 */
	bool Bypass() const { return m_Bypassed; }

	/**
	 * Generate the next sample given the input sample and the line number.
	 * @param s input sample
	 * @param c line number
	 */
	double Process(double s, int c);

	/**
	 * Update the effects, happens separately because this needs to always happens, not
	 * only when the effect is visible and within the viewport.
	 */
	void UpdateEffects();

	/**
	 * Override the cursor, effectpanel will determine cursor in most cases. If over HoveringDrag arae
	 * the cursor will be the resize all cursor, otherwise it will be the cursor chosen by the hovering
	 * component. If no hovering it will be the normal cursor.
	 */
	int Cursor() const override;

	void Update(const Vec4<int>& v) override;
	void Render(CommandCollection& d) override;

	virtual operator nlohmann::json();
	virtual void operator=(const nlohmann::json& json);

private:
	int m_InsertIndex = -1,
		m_Lines = 0;

	double m_MouseY = 0;

	bool m_Bypassed = false;

	Menu<SoundMixrGraphics::Vertical, MenuType::Normal> 
		m_Menu,
		m_EffectMenu;

	::Panel* m_EffectPanel;
	
	Component* m_DraggingComponent = nullptr;
	
	// Lock is used to make sure any mutations to the effect chain 
	// are done concurrently safe.
	mutable std::mutex m_Lock;
	
	/**
	 * Converts y-coord to index in the effect panel.
	 * @param y y-coord
	 */
	int GetIndex(int y) const;
};