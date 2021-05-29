#include "audio/GeneratorFrame.hpp"
#include "Controller.hpp"

GeneratorFrame::GeneratorFrame(SoundMixr::GeneratorBase* generator)
	: m_Generator(Panel().Emplace<::Generator>(generator)), SoundMixrFrame(WindowData{ generator->Name(), 
		{ generator->Width(), generator->Height() }, false, false, true, true, false, true, &Controller::Get().mainWindow })
{
	Panel().Layout<Layout::Grid>(1, 1, 8, 8);
}

void GeneratorFrame::Update(const Vec4<int>& v)
{
	Color(ThemeT::Get().window_border);
	SoundMixrFrame::Update(v);
}
