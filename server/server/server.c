/********************************************************************\
* server.c                                                           *
*                                                                    *
* Desc: example of the server-side of an application                 *
* Revised: Dag Nystrom & Jukka Maki-Turja                     *
*                                                                    *
* Based on generic.c from Microsoft.                                 *
*                                                                    *
*  Functions:                                                        *
*     WinMain      - Application entry point                         *
*     MainWndProc  - main window procedure                           *
*                                                                    *
* NOTE: this program uses some graphic primitives provided by Win32, *
* therefore there are probably a lot of things that are unfamiliar   *
* to you. There are comments in this file that indicates where it is *
* appropriate to place your code.                                    *
* *******************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "wrapper.h"
#include "doublylinkedlist.h"
#include <time.h>

							/* the server uses a timer to periodically update the presentation window */
							/* here is the timer id and timer period defined                          */

#define UPDATE_FREQ     10	/* update frequency (in ms) for the timer */

							/* (the server uses a mailslot for incoming client requests) */

#define G 6.67259e-11



/*********************  Prototypes  ***************************/
/* NOTE: Windows has defined its own set of types. When the   */
/*       types are of importance to you we will write comments*/ 
/*       to indicate that. (Ignore them for now.)             */
/**************************************************************/

LRESULT WINAPI MainWndProc( HWND, UINT, WPARAM, LPARAM );
DWORD WINAPI mailThread(LPVOID);

// Global variables
CRITICAL_SECTION dbAccess;

// Function prototypes
void __stdcall calculatePosition(planet_type *);
int killPlanet(planet_type *, int);
void sendErrorToCreator(planet_type *, int );

HDC hDC;		/* Handle to Device Context, gets set 1st time in MainWndProc */
				/* we need it to access the window for printing and drawin */

/********************************************************************\
*  Function: int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)    *
*                                                                    *
*   Purpose: Initializes Application                                 *
*                                                                    *
*  Comments: Register window class, create and display the main      *
*            window, and enter message loop.                         *
*                                                                    *
*                                                                    *
\********************************************************************/

							/* NOTE: This function is not too important to you, it only */
							/*       initializes a bunch of things.                     */
							/* NOTE: In windows WinMain is the start function, not main */

typedef struct serverMessage{
	char name[20];
	int error;
}serverMessage;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow ) {

	HWND hWnd;
	DWORD threadID;
	MSG msg;

	if (!InitializeCriticalSectionAndSpinCount(&dbAccess, 0x00000400)) 
		return 0;

							/* Create the window, 3 last parameters important */
							/* The tile of the window, the callback function */
							/* and the backgrond color */

	hWnd = windowCreate (hPrevInstance, hInstance, nCmdShow, "Universe", MainWndProc, COLOR_WINDOW+1);

							/* start the timer for the periodic update of the window    */
							/* (this is a one-shot timer, which means that it has to be */
							/* re-set after each time-out) */
							/* NOTE: When this timer expires a message will be sent to  */
							/*       our callback function (MainWndProc).               */
  
	windowRefreshTimer (hWnd, UPDATE_FREQ);
  

							/* create a thread that can handle incoming client requests */
							/* (the thread starts executing in the function mailThread) */
							/* NOTE: See online help for details, you need to know how  */ 
							/*       this function does and what its parameters mean.   */
							/* We have no parameters to pass, hence NULL				*/
  

	threadID = threadCreate (mailThread, NULL); 
  

							/* (the message processing loop that all windows applications must have) */
							/* NOTE: just leave it as it is. */
	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	DeleteCriticalSection(&dbAccess);
	return msg.wParam;
}


/********************************************************************\
* Function: mailThread                                               *
* Purpose: Handle incoming requests from clients                     *
* NOTE: This function is important to you.                           *
/********************************************************************/
DWORD WINAPI mailThread(LPVOID arg) {

	char buffer[1024];
	planet_type *planet = (planet_type*)malloc(sizeof(planet_type));
	DWORD bytesRead;
	static int posY = 0;
	char mailSlotString[18] = "\\\\.\\mailslot\\", inputString[1024];
	HANDLE serverMailslot;

							/* create a mailslot that clients can use to pass requests through   */
							/* (the clients use the name below to get contact with the mailslot) */
							/* NOTE: The name of a mailslot must start with "\\\\.\\mailslot\\"  */

	
	serverMailslot = mailslotCreate("\\\\.\\mailslot\\serverMailslot");

							/* (ordinary file manipulating functions are used to read from mailslots) */
							/* in this example the server receives strings from the client side and   */
							/* displays them in the presentation window                               */
							/* NOTE: binary data can also be sent and received, e.g. planet structures*/
 
	while (1)
	{
		bytesRead = mailslotRead(serverMailslot, (void *)planet, sizeof(planet_type));

		if (bytesRead != 0) {
			// If planet name doesnt exist, add it to linked list
			if (!planetExists(planet)) {
				EnterCriticalSection(&dbAccess);
				InsertAtHead(*planet);
				LeaveCriticalSection(&dbAccess);
				threadCreate(calculatePosition, &(head->data));
			}
			else
			{
				sendErrorToCreator(planet, 2);
			}
		}
		else {
			/* failed reading from mailslot                              */
			/* (in this example we ignore this, and happily continue...) */
		}
	}
  
	free(planet);
	return 0;
}

// Calculates the planets next position
void __stdcall calculatePosition(planet_type *planet)
{
	struct Node *iterator;
	double xF, yF, Ftot, Atot, Ax, Ay, resAx, resAy, distanceX, distanceY, distanceTot, maxX = 800, maxY = 600, dt = 10; // G = 0.00000000006667259;
	
	while (1)
	{
		EnterCriticalSection(&dbAccess);
		if (head->next != NULL) // Only one planet
		{
			iterator = head;
			resAx = 0;
			resAy = 0;
			while (iterator != NULL) // Multiple planets
			{
				if (strcmp(iterator->data.name, planet->name) != 0) // Ignore self
				{
					distanceX = planet->sx - iterator->data.sx; // Doesn't matter if negative, dissapears when quadrated
					distanceY = planet->sy - iterator->data.sy;
					distanceTot = sqrt((distanceX * distanceX) + (distanceY * distanceY));

					Atot = G*(iterator->data.mass / (distanceTot * distanceTot));

					Ax = Atot * ((iterator->data.sx - planet->sx) / distanceTot);

					Ay = Atot * ((iterator->data.sy - planet->sy) / distanceTot);

					resAx = resAx + Ax;
					resAy = resAy + Ay;
				}
				iterator = iterator->next;
			}
			planet->vx = planet->vx + (resAx * dt);
			planet->vy = planet->vy + (resAy * dt);
		}

		planet->sx = planet->sx + (planet->vx * dt);
		planet->sy = planet->sy + (planet->vy * dt);

		planet->life = planet->life - 1;

		

		if (planet->life <= 0)
		{
			//Send message to client
			killPlanet(planet, 0); //Kill 
			LeaveCriticalSection(&dbAccess);
			return; //Kill thread
		}
		else if (planet->sx > 800 || planet->sx < 0 || planet->sy > 600 || planet->sy < 0)
		{
			killPlanet(planet, 1);
			LeaveCriticalSection(&dbAccess);
			return;
		}
		LeaveCriticalSection(&dbAccess);
		Sleep(10);
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
		if ( strcmp(iterator->data.name, planet->name) == 0 )
		{
			// planet with this name exists
			return 1;
		}
		iterator = iterator->next;
	}

	return 0;
}

/*	Removes a planet from linkedlist and deallocates its memory
	a message will be sent to the client that created the planet with the reason of termination. */
int killPlanet(planet_type *planet, int flag) 
{
	char  clientMailslotName[256], mailSlotString[18] = "\\\\.\\mailslot\\", procIDString[30];
	HANDLE clientMailslot;
	serverMessage message;

	if (planetExists(planet))
	{
		wsprintf(clientMailslotName, "\\\\.\\mailslot\\%s", planet->pid); //Generate clientMailSlotName


		clientMailslot = mailslotConnect(clientMailslotName);
		if (clientMailslot == INVALID_HANDLE_VALUE) 
		{
			MessageBox(0, "Failed to get a handle to the client mailslot!!!", "", 1);
			return;
		}

		strcpy_s(message.name, sizeof(planet->name), planet->name);
		if (removeNode(planet)) 
		{
			// Send Message to client: Planet removed
			message.error = flag;

			mailslotWrite(clientMailslot, (void *)&message, sizeof(serverMessage));


			return 1;
		}

		//LeaveCriticalSection(&dbAccess);
	}
	else
		return 0;
}

void sendErrorToCreator(planet_type *planet, int flag)
{
	char  clientMailslotName[256], mailSlotString[18] = "\\\\.\\mailslot\\", procIDString[30];
	HANDLE clientMailslot;
	serverMessage message;


	wsprintf(clientMailslotName, "\\\\.\\mailslot\\%s", planet->pid); //Generate clientMailSlotName


	clientMailslot = mailslotConnect(clientMailslotName);
	if (clientMailslot == INVALID_HANDLE_VALUE)
	{
		MessageBox(0, "Failed to get a handle to the client mailslot!!!", "", 1);
		return;
	}

	strcpy_s(message.name, sizeof(planet->name), planet->name);

	// Send Message to client: Planet removed
	message.error = flag;

	mailslotWrite(clientMailslot, (void *)&message, sizeof(serverMessage));

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

LRESULT CALLBACK MainWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
  
	PAINTSTRUCT ps;
	HANDLE context;
	static DWORD color = 0;
	struct Node *iterator;
  
	switch( msg ) {
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
			EnterCriticalSection(&dbAccess);
			iterator = head;
			while (iterator != NULL)
			{
				SetPixel(hDC, iterator->data.sx, iterator->data.sy, (COLORREF)color);

				color += 12;

				iterator = iterator->next;
			}
			LeaveCriticalSection(&dbAccess);
			windowRefreshTimer (hWnd, UPDATE_FREQ);
			break;
							/****************************************************************\
							*     WM_PAINT: (received when the window needs to be repainted, *
							*               e.g. when maximizing the window)                 *
							\****************************************************************/

		case WM_PAINT:
							/* NOTE: The code for this message can be removed. It's just */
							/*       for showing something in the window.                */
			context = BeginPaint( hWnd, &ps ); /* (you can safely remove the following line of code) */
			//TextOut( context, 10, 10, "Hello, World!", 13 ); /* 13 is the string length */
			EndPaint( hWnd, &ps );
			break;
							/**************************************************************\
							*     WM_DESTROY: PostQuitMessage() is called                  *
							*     (received when the user presses the "quit" button in the *
							*      window)                                                 *
							\**************************************************************/
		case WM_DESTROY:
			PostQuitMessage( 0 );
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
			return( DefWindowProc( hWnd, msg, wParam, lParam )); 
   }
   return 0;
}




