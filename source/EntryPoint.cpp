#include "Controller.hpp"

#ifdef _DEBUG
int main()
{
	Controller::Get().Run();
	return 0;
}
#else
int WINAPI WinMain(
	HINSTANCE hInstance,     /* [input] handle to current instance */
	HINSTANCE hPrevInstance, /* [input] handle to previous instance */
	LPSTR lpCmdLine,         /* [input] pointer to command line */
	int nCmdShow             /* [input] show state of window */
) 
{
	Controller::Get().Run();
	return 0;
}
#endif