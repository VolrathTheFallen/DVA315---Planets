#include <windows.h>
#include "wrapper.h"
#include "resource.h"
#include "doublylinkedlist.h"

#define BUFFERSIZE 256
#define MAX_PATH 128

LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI MonitorWndProc(HWND, UINT, WPARAM, LPARAM);
planet_type createPlanet(HWND hWnd);
void AddToListBox(HWND hWnd, char *msg, int listBox);
int planetExists(planet_type *);
int exportPlanets(HWND hWnd);

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
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_SEND:
			MessageBox(NULL, "Klicked send button", "Test", 0);
			break;
		case ID_BUTTON_CREATE: // Creates planet out of information in textboxes in mainDialog
			InsertAtHead(createPlanet(hWnd)); //Add to DB
			AddToListBox(hWnd, head->data.name, IDC_LIST_LOCAL);
			break;
		case ID_BUTTON_IMPORT:
			MessageBox(NULL, "Klicked import button", "Test", 0);
			break;
		case ID_BUTTON_EXPORT:
			exportPlanets(hWnd);
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

planet_type createPlanet(HWND hWnd)
{
	char buffer[BUFFERSIZE] = {'\0'};

	planet_type planet;

	GetDlgItemText(hWnd, IDC_EDIT_NAME, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return;
	}
	else
	{
		if (!planetExists(&planet))
		{
			strcpy_s(planet.name, sizeof(planet.name), buffer);
		}
		else
		{
			MessageBox(0, "Planet already exists!", "ERROR", 1);
			return planet;
		}
	}

	GetDlgItemText(hWnd, IDC_EDIT_XPOSITION, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return planet;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet.sx));
	}

	GetDlgItemText(hWnd, IDC_EDIT_YPOSITION, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return planet;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet.sy));
	}

	GetDlgItemText(hWnd, IDC_EDIT_XVELOCITY, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return planet;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet.vx));
	}

	GetDlgItemText(hWnd, IDC_EDIT_YVELOCITY, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return planet;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet.vy));
	}

	GetDlgItemText(hWnd, IDC_EDIT_MASS, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return planet;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet.mass));
	}

	GetDlgItemText(hWnd, IDC_EDIT_LIFE, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return planet;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet.life));
	}

	SetDlgItemText(hWnd, IDC_EDIT_NAME, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_XPOSITION, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_YPOSITION, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_XVELOCITY, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_YVELOCITY, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_MASS, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_LIFE, "\0");

	return planet;
}

void AddToListBox(HWND hWnd, char *msg, int listBox)
{
	HWND hwndList = GetDlgItem(hWnd, listBox);

	SendMessage(hwndList, LB_ADDSTRING, NULL, (LPARAM)msg);
}

int exportPlanets(HWND hWnd)
{
	OPENFILENAME ofn;
	//char fileName[MAX_PATH] = "";
	HANDLE localListBox = GetDlgItem(hWnd, IDC_LIST_LOCAL);
	char buffer[BUFFERSIZE];

	int selInd = -1;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = "Planet Files (*.dat)\0*.dat\0";
	ofn.lpstrFile = "";// fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "dat";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		FILE *file = fopen_s(ofn.lpstrFile, ofn.lpstrFileTitle, "wb");
		int selCount = SendMessage(localListBox, LB_GETSELCOUNT, NULL, NULL);
		int listCount = SendMessage(localListBox, LB_GETCOUNT, NULL, NULL);

		if (file)
		{
			if (selCount > 0)
			{
				for (int i = 0; i < listCount; i++)
				{
					if (SendMessage(localListBox, LB_GETSEL, i, 0) > 0) // LB_GETSELITEMS for list of items
					{
						SendMessage(localListBox, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer);

						// Loop through all planets in local list
						struct Node *iterator = head;
						while (iterator != NULL)
						{
							if (strcmp(iterator->data.name, buffer) == 0)
							{
								fwrite(&(iterator->data), sizeof(planet_type), 1, file);
							}
							// Continue iteration
							iterator = iterator->next;
						}
					}
				}
				MessageBox(0, "Planets saved to file", "Success!", 1);
			}
			else
			{
				struct Node *iterator = head;
				while (iterator != NULL)
				{
					fwrite(&(iterator->data), sizeof(planet_type), 1, file);
					iterator = iterator->next;
				}
				MessageBox(0, "Planets saved to file", "Success!", 1);
			}
			fclose(file);
		}
	}
}

// Looks for planet name in linked list, returns 1 if found and 0 if not.
int planetExists(planet_type *planet) {

	struct Node *iterator = head;

	if (iterator == NULL) {
		return 0;
	}

	while (iterator != NULL)
	{
		if (strcmp(iterator->data.name, planet->name) == 0)
		{
			// planet with this name exists
			return 1;
		}
		iterator = iterator->next;
	}

	return 0;
}