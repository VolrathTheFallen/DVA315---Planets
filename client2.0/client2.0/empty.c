#include <windows.h>
#include "wrapper.h"
#include "resource.h"

#define UPDATE_FREQ     10	

LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI MonitorWndProc(HWND, UINT, WPARAM, LPARAM);
void __stdcall DialogCreate(HWND *);

HDC hDC;		/* Handle to Device Context, gets set 1st time in MainWndProc */
/* we need it to access the window for printing and drawin */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {

	HWND hWnd;
	DWORD threadID;
	MSG msg;

	HWND dialogs[10];

	dialogs[0] = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_CREATE), NULL, MainWndProc);
	if (dialogs[0] != NULL)
		ShowWindow(dialogs[0], SW_SHOW);
	else 
		MessageBox(NULL, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);

	// create monitor dialog as child to Main dialog

	threadID = threadCreate(DialogCreate, dialogs);

	


	/* start the timer for the periodic update of the window    */
	/* (this is a one-shot timer, which means that it has to be */
	/* re-set after each time-out) */
	/* NOTE: When this timer expires a message will be sent to  */
	/*       our callback function (MainWndProc).               */

	//windowRefreshTimer(hWnd, UPDATE_FREQ);


	/* create a thread that can handle incoming client requests */
	/* (the thread starts executing in the function mailThread) */
	/* NOTE: See online help for details, you need to know how  */
	/*       this function does and what its parameters mean.   */
	/* We have no parameters to pass, hence NULL				*/


	


	/* (the message processing loop that all windows applications must have) */
	/* NOTE: just leave it as it is. */
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void __stdcall DialogCreate(HWND *dialogs) {

	dialogs[0] = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_MONITOR), NULL, MainWndProc);
	if (dialogs[0] != NULL)
		ShowWindow(dialogs[0], SW_SHOW);
	else
		MessageBox(NULL, "CreateDialog returned NULL", "Warning!", MB_OK | MB_ICONINFORMATION);




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

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	HANDLE context;
	static DWORD color = 0;
	struct Node *iterator;

	switch (msg) {
		/**************************************************************/
		/*    WM_CREATE:        (received on window creation)
		/**************************************************************/
	case WM_CREATE:
		hDC = GetDC(hWnd);





		break;
		/**************************************************************/
		/*    WM_TIMER:         (received when our timer expires)
		/**************************************************************/
	case WM_TIMER:

		/* NOTE: replace code below for periodic update of the window */
		/*       e.g. draw a planet system)                           */
		/* NOTE: this is referred to as the 'graphics' thread in the lab spec. */
		
		windowRefreshTimer(hWnd, UPDATE_FREQ);
		break;
		/****************************************************************\
		*     WM_PAINT: (received when the window needs to be repainted, *
		*               e.g. when maximizing the window)                 *
		\****************************************************************/

	case WM_PAINT:
		/* NOTE: The code for this message can be removed. It's just */
		/*       for showing something in the window.                */
		context = BeginPaint(hWnd, &ps); /* (you can safely remove the following line of code) */
		//TextOut( context, 10, 10, "Hello, World!", 13 ); /* 13 is the string length */
		EndPaint(hWnd, &ps);
		break;
		/**************************************************************\
		*     WM_DESTROY: PostQuitMessage() is called                  *
		*     (received when the user presses the "quit" button in the *
		*      window)                                                 *
		\**************************************************************/
	case WM_DESTROY:

		DestroyWindow(hWnd);
		PostQuitMessage(0);
		/* NOTE: Windows will automatically release most resources this */
		/*       process is using, e.g. memory and mailslots.           */
		/*       (So even though we don't free the memory which has been*/
		/*       allocated by us, there will not be memory leaks.)      */

		ReleaseDC(hWnd, hDC); /* Some housekeeping */
		break;

		/**************************************************************\
		*     Let the default window proc handle all other messages    *
		\**************************************************************/
	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return 0;
}

LRESULT CALLBACK MonitorWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	HANDLE context;

	switch (msg) {
		/**************************************************************/
		/*    WM_CREATE:        (received on window creation)
		/**************************************************************/
	case WM_CREATE:
		hDC = GetDC(hWnd);


		break;
		/**************************************************************/
		/*    WM_TIMER:         (received when our timer expires)
		/**************************************************************/
	case WM_TIMER:

		/* NOTE: replace code below for periodic update of the window */
		/*       e.g. draw a planet system)                           */
		/* NOTE: this is referred to as the 'graphics' thread in the lab spec. */

		windowRefreshTimer(hWnd, UPDATE_FREQ);
		break;
		/****************************************************************\
		*     WM_PAINT: (received when the window needs to be repainted, *
		*               e.g. when maximizing the window)                 *
		\****************************************************************/

	case WM_PAINT:
		/* NOTE: The code for this message can be removed. It's just */
		/*       for showing something in the window.                */
		context = BeginPaint(hWnd, &ps); /* (you can safely remove the following line of code) */
		//TextOut( context, 10, 10, "Hello, World!", 13 ); /* 13 is the string length */
		EndPaint(hWnd, &ps);
		break;
		/**************************************************************\
		*     WM_DESTROY: PostQuitMessage() is called                  *
		*     (received when the user presses the "quit" button in the *
		*      window)                                                 *
		\**************************************************************/
	case WM_DESTROY:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		/* NOTE: Windows will automatically release most resources this */
		/*       process is using, e.g. memory and mailslots.           */
		/*       (So even though we don't free the memory which has been*/
		/*       allocated by us, there will not be memory leaks.)      */

		ReleaseDC(hWnd, hDC); /* Some housekeeping */
		break;

		/**************************************************************\
		*     Let the default window proc handle all other messages    *
		\**************************************************************/
	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return 0;
}