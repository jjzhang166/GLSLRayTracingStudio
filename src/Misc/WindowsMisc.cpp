#include "Misc/WindowsMisc.h"

#include <Windows.h>
#include <tchar.h>

void WindowsMisc::GetDesktopResolution(int32& outWidth, int32& outHeight)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	outWidth  = desktop.right;
	outHeight = desktop.bottom;
}

void WindowsMisc::RequestMinimize()
{
	::ShowWindow(::GetActiveWindow(), SW_MINIMIZE);
}

bool WindowsMisc::IsThisApplicationForeground()
{
	uint32 foregroundProcess;
	::GetWindowThreadProcessId(GetForegroundWindow(), (::DWORD*)&foregroundProcess);
	return (foregroundProcess == GetCurrentProcessId());
}

void WindowsMisc::PreventScreenSaver()
{
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dx = 0;
	input.mi.dy = 0;	
	input.mi.mouseData = 0;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.time = 0;
	input.mi.dwExtraInfo = 0;

	SendInput(1, &input, sizeof(INPUT));
}
