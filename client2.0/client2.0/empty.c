#include <windows.h>
#include "wrapper.h"
#include "resource.h"

#define UPDATE_FREQ     10	

LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI MonitorWndProc(HWND, UINT, WPARAM, LPARAM);

HDC hDC;		/* Handle to Device Context, gets set 1st time in MainWndProc */
/* we need it to access the window for printing and drawin */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {

	MSG msg;
	HWND monitorDialog, mainDialog;

	mainDialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_CREATE), NULL, MainWndProc);
	if (mainDialog != NULL)
		ShowWindow(mainDialog, SW_SHOW);
	else 
		MessageBox(NULL, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);

	windowRefreshTimer(mainDialog, UPDATE_FREQ);

	// create monitor dialog as child to Main dialog

	monitorDialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_MONITOR), mainDialog, MonitorWndProc);
	if (monitorDialog != NULL)
		ShowWindow(monitorDialog, SW_SHOW);
	else
		MessageBox(NULL, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);

	/* NOTE: When this timer expires a message will be sent to  */
	/*       our callback function (MainWndProc).               */
	windowRefreshTimer(monitorDialog, UPDATE_FREQ);

	/* (the message processing loop that all windows applications must have) */
	/* NOTE: just leave it as it is. */
	BOOL ret;

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

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
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

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

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

	return DefWindowProc(hWnd, msg, wParam, lParam);
}