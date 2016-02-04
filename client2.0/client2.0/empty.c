#include <windows.h>
#include "wrapper.h"
#include "resource.h"
#include "doublylinkedlist.h"

#define BUFFERSIZE 256
#define MAX_PATH 128

LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI MonitorWndProc(HWND, UINT, WPARAM, LPARAM);
int createPlanet(HWND hWnd, planet_type *);
void addToListBox(HWND hWnd, char *msg, int listBox);
int planetExists(planet_type *);
int exportPlanets(HWND hWnd);
int importPlanets(HWND hWnd);
int addSentPlanetsToSentList(HWND hWnd);
int sendSelectedPlanetsToServer(HWND hWnd);
void removeFromListbox(HWND hWnd, int listbox, int id);
void clearListbox(HWND hwnd, int listbox);


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

	monitorDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MONITOR), mainDialog, MonitorWndProc);
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
	planet_type planet;

	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BUTTON_SEND:
			// MessageBox(NULL, "Klicked send button", "Test", 0);
			sendSelectedPlanetsToServer(hWnd);
			//addSentPlanetsToSentList(hWnd, IDC_LIST_SENT);

			break;
		case ID_BUTTON_CREATE: // Creates planet out of information in textboxes in mainDialog
			if (createPlanet(hWnd, &planet) == 1) { // only insert to LL if createPlanet succeeded (that is, if all user input is valid)
				InsertAtHead(planet); //Add to DB
				addToListBox(hWnd, head->data.name, IDC_LIST_LOCAL);
			}
			break;
		case ID_BUTTON_IMPORT:
			//MessageBox(NULL, "Klicked import button", "Test", 0);
			importPlanets(hWnd);
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

int createPlanet(HWND hWnd, planet_type *planet)
{
	char buffer[BUFFERSIZE] = {'\0'};


	GetDlgItemText(hWnd, IDC_EDIT_NAME, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return 0;
	}
	else
	{
		if (!planetExists(&planet))
		{
			strcpy_s(planet->name, sizeof(planet->name), buffer);
		}
		else
		{
			MessageBox(0, "Planet already exists!", "ERROR", 1);
			return 0;
		}
	}

	GetDlgItemText(hWnd, IDC_EDIT_XPOSITION, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return 0;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet->sx));
	}

	GetDlgItemText(hWnd, IDC_EDIT_YPOSITION, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return 0;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet->sy));
	}

	GetDlgItemText(hWnd, IDC_EDIT_XVELOCITY, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return 0;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet->vx));
	}

	GetDlgItemText(hWnd, IDC_EDIT_YVELOCITY, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return 0;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet->vy));
	}

	GetDlgItemText(hWnd, IDC_EDIT_MASS, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return 0;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet->mass));
	}

	GetDlgItemText(hWnd, IDC_EDIT_LIFE, buffer, BUFFERSIZE);
	if (strlen(buffer) < 1)
	{
		MessageBox(0, "Please provide input!", "ERROR", 1);
		return 0;
	}
	else
	{
		sscanf_s(buffer, "%lf", &(planet->life));
	}

	SetDlgItemText(hWnd, IDC_EDIT_NAME, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_XPOSITION, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_YPOSITION, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_XVELOCITY, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_YVELOCITY, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_MASS, "\0");
	SetDlgItemText(hWnd, IDC_EDIT_LIFE, "\0");

	return 1;
}

void addToListBox(HWND hWnd, char *msg, int listBox)
{
	HWND hwndList = GetDlgItem(hWnd, listBox);

	SendMessage(hwndList, LB_ADDSTRING, NULL, (LPARAM)msg);
}

int exportPlanets(HWND hWnd)
{
	HANDLE localListBox = GetDlgItem(hWnd, IDC_LIST_LOCAL);
	HANDLE exportFile;
	char buffer[BUFFERSIZE];
	LPWORD bytesWritten = 0;
	BOOL result;


	exportFile = OpenFileDialog("", GENERIC_WRITE, OPEN_ALWAYS);

	int selCount = SendMessage(localListBox, LB_GETSELCOUNT, NULL, NULL);
	int listCount = SendMessage(localListBox, LB_GETCOUNT, NULL, NULL);

	if (exportFile)
	{
		struct Node *iterator = head;

		if (selCount > 0)
		{
			for (int i = 0; i < listCount; i++)
			{
				if (SendMessage(localListBox, LB_GETSEL, i, 0) > 0) // LB_GETSELITEMS for list of items
				{
					SendMessage(localListBox, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer);

					// Loop through all planets in local list
					while (iterator != NULL)
					{
						if (strcmp(iterator->data.name, buffer) == 0)
						{
							result = WriteFile(exportFile, (LPCVOID)&(iterator->data), sizeof(planet_type), &bytesWritten, (LPOVERLAPPED)NULL);
							if (!result || bytesWritten <= 0)
							{
								MessageBox(0, "WriteFile failed.", "Error", 1);
								return 0;
							}
						}
						// Continue iteration
						iterator = iterator->next;
					}
					iterator = head;
				}
			}
		}
		else
		{
			for (int i = 0; i < listCount; i++)
			{
				SendMessage(localListBox, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer);

				while (iterator != NULL)
				{
					result = WriteFile(exportFile, (LPCVOID)&(iterator->data), sizeof(planet_type), &bytesWritten, (LPOVERLAPPED)NULL);
					if (bytesWritten <= 0)
					{
						MessageBox(0, "bytesWritten = 0.", "Error", 1);
						return 0;
					}
					if (!result)
					{
						MessageBox(0, "WriteFile failed", "Error", 1);
						return 0;
					}
					iterator = iterator->next;
				}
			}
		}

		MessageBox(0, "Planets saved to file", "Success!", 1);

		CloseHandle(exportFile);
	}
	return 1;
}

int importPlanets(HWND hWnd)
{
	HANDLE localListBox = GetDlgItem(hWnd, IDC_LIST_LOCAL);
	HANDLE importFile;
	char buffer[BUFFERSIZE];
	planet_type readPlanet;
	DWORD bytesRead = 0;
	BOOL result;
	struct _OVERLAPPED lpOvr;
	lpOvr.Offset = sizeof(planet_type);

	importFile = OpenFileDialog("", GENERIC_READ, OPEN_ALWAYS);


	if (importFile)
	{
		do {
			result = ReadFile(importFile, (LPCVOID)buffer, (DWORD)sizeof(planet_type), (LPDWORD)&bytesRead, (LPOVERLAPPED)NULL);
 			if (!result)
			{
				MessageBox(0, "ReadFile failed.", "Error", 1);
				return 0;
			}
			if (bytesRead > 0)
			{
				
				memcpy(&readPlanet, buffer, sizeof(planet_type));
				if (!planetExists(&readPlanet))
				{
					InsertAtHead(readPlanet);
					addToListBox(hWnd, readPlanet.name, IDC_LIST_LOCAL);
				}
			}
		} while (bytesRead > 0);


		MessageBox(0, "Planets imported to ListBox", "Success!", 1);

		CloseHandle(importFile);
	}
	return 1;
}

int addSentPlanetsToSentList(HWND hWnd) {

	HANDLE localListBox = GetDlgItem(hWnd, IDC_LIST_LOCAL);
	char buffer[BUFFERSIZE];
	LPWORD bytesWritten = 0;
	BOOL result;

	int selCount = SendMessage(localListBox, LB_GETSELCOUNT, NULL, NULL);
	int listCount = SendMessage(localListBox, LB_GETCOUNT, NULL, NULL);


	struct Node *iterator = head;

	if (selCount > 0)
	{
		for (int i = 0; i < listCount; i++)
		{
			if (SendMessage(localListBox, LB_GETSEL, i, 0) > 0) // LB_GETSELITEMS for list of items
			{
				SendMessage(localListBox, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer);

				addToListBox(hWnd, buffer, IDC_LIST_SENT);

			}

		}
		

		MessageBox(0, "Added planets to sent to server list...", "Success!", 1);

	}
	return 1;

}

/*Sends the local planets to the server, returns 1 if succesful, 0 if not*/
int sendSelectedPlanetsToServer(HWND hWnd)
{
	HANDLE serverMailslot, localListBox = GetDlgItem(hWnd, IDC_LIST_LOCAL);
	struct Node *iterator, *toDelete;
	int res;
	char buffer[BUFFERSIZE];

	serverMailslot = mailslotConnect("\\\\.\\mailslot\\serverMailslot");

	if (serverMailslot == INVALID_HANDLE_VALUE)
	{
		MessageBox(0, "Failed to get a handle to the server mailslot!!!\nHave you started the server?\n", "ERROR", MB_OK);
		return 0;
	}

	int selCount = SendMessage(localListBox, LB_GETSELCOUNT, NULL, NULL);
	int listCount = SendMessage(localListBox, LB_GETCOUNT, NULL, NULL);

	if (head != NULL)
	{
		if (selCount > 0)
		{
			for (int i = 0; i < listCount; i++)
			{
				iterator = head;

				if (SendMessage(localListBox, LB_GETSEL, i, 0) > 0) // LB_GETSELITEMS for list of items
				{
					SendMessage(localListBox, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer);

					// Loop through all planets in local list
					while (iterator != NULL)
					{
						if (strcmp(iterator->data.name, buffer) == 0)
						{
							/*res = mailslotWrite(serverMailslot, (void*)&(iterator->data), sizeof(planet_type));
							if (res == 0)
							{
								MessageBox(0, "Error writing to serverMailslot!", "ERROR", MB_OK);
								return 0;
							}
							*/
							addToListBox(hWnd, iterator->data.name, IDC_LIST_SENT);
							removeFromListbox(hWnd, IDC_LIST_LOCAL, i);
							toDelete = iterator;
							break;
						}
						// Continue iteration
						iterator = iterator->next;
					}
					removeNode(toDelete);
					toDelete = NULL;
				}
			}
		}
		else
		{
			for (int i = 0; i < listCount; i++)
			{
				iterator = head;

				while (iterator != NULL)
				{
					res = mailslotWrite(serverMailslot, (void*)&(iterator->data), sizeof(planet_type));
					if (res == 0)
					{
						MessageBox(0, "Error writing to serverMailslot!", "ERROR", MB_OK);
						return 0;
					}

					addToListBox(hWnd, iterator->data.name, IDC_LIST_SENT);

					iterator = iterator->next;
				}
			}

			clearListbox(hWnd, IDC_LIST_LOCAL);

			/*If successful deallocate linked list*/
			iterator = head;
			while (iterator != NULL)
			{
				if (iterator->next == NULL)
				{
					free(iterator);
					iterator = NULL;
					break;
				}

				iterator = iterator->next;
				free(iterator->prev);
			}

			if (head != NULL)
				head = NULL;
		}
	}

	return 1;
}

void removeFromListbox(HWND hWnd, int listbox, int id)
{
	HWND listBox = GetDlgItem(hWnd, listbox);
	SendMessage(listBox, LB_DELETESTRING, (WPARAM)id, (LPARAM)NULL);
}

void clearListbox(HWND hWnd, int listbox)
{
	HWND listBox = GetDlgItem(hWnd, listbox);
	SendMessage(listBox, LB_RESETCONTENT, NULL, NULL);
}

// Looks for planet name in linked list, returns 1 if found and 0 if not.
int planetExists(planet_type *planet) 
{
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