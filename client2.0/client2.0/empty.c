#include <windows.h>
#include "wrapper.h"
#include "resource.h"
#include "doublylinkedlist.h"

#define UPDATE_FREQ     10	

LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI MonitorWndProc(HWND, UINT, WPARAM, LPARAM);
void createPlanet();

HDC hDC;		/* Handle to Device Context, gets set 1st time in MainWndProc */
/* we need it to access the window for printing and drawin */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {

	MSG msg;
	BOOL ret;
	HWND monitorDialog, mainDialog;

	mainDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CREATE), NULL, MainWndProc);
	if (mainDialog != NULL)
		ShowWindow(mainDialog, SW_SHOW);
	else 
		MessageBox(NULL, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);

	// create monitor dialog as child to Main dialog

	monitorDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MONITOR), NULL, MonitorWndProc);
	if (monitorDialog != NULL)
		ShowWindow(monitorDialog, SW_SHOW);
	else
		MessageBox(NULL, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);
	

	while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (ret != -1)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// An error occurred! Handle it and bail out.
			MessageBox(NULL, L"Unexpected Error", NULL, MB_OK | MB_ICONERROR);
			return 1;
		}
	}

	return msg.wParam;
}

/********************************************************************\
* Function: LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM) *
*                                                                    *
* Purpose: Processes Application Messages (received by the window)   *
* Comments: The following messages are processed                     *
*                                                                    *
*           WM_PAINT                                                 *
*           WM_COMMAND                                               *
*           WM_DESTROY                                               *
*           WM_TIMER                                                 *
*                                                                    *
\********************************************************************/
/* NOTE: This function is called by Windows when something happens to our window */

LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return FALSE;// DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	char buffer[256];
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_SEND:
			GetDlgItemText(hWnd, IDC_EDIT_NAME, buffer, 256);
			MessageBox(NULL, buffer, "Test", 0);
			break;
		case ID_BUTTON_CREATE: // Creates planet out of information in textboxes in mainDialog
			createPlanet();
			MessageBox(NULL, "Klicked create button", "Test", 0);
			break;
		case ID_BUTTON_IMPORT:
			MessageBox(NULL, "Klicked import button", "Test", 0);
			break;
		case ID_BUTTON_EXPORT:
			MessageBox(NULL, "Klicked export button", "Test", 0);
			break;
		}
		return 0;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return 0;// DefWindowProc(hWnd, msg, wParam, lParam);
}

void createPlanet()
{

}