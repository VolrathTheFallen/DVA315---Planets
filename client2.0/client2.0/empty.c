#include <windows.h>
#include "wrapper.h"
#include "resource.h"

BOOL CALLBACK mainWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow ) 
{

	HWND mainWnd = NULL;

	
	mainWnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_CREATE), NULL, mainWndProc);
	if (mainWnd != NULL)
	{
			ShowWindow(mainWnd, SW_SHOW);
	}
	else
	{
		MessageBox(NULL, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);
	}

	return 1;
}

BOOL CALLBACK mainWndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_SEND:
			MessageBox(hwnd, "Hi!", "This is a message",
				MB_OK | MB_ICONEXCLAMATION);
			break;
		case ID_BUTTON_CREATE:
			MessageBox(hwnd, "Bye!", "This is also a message",
				MB_OK | MB_ICONEXCLAMATION);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}