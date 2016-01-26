/*********************************************
* client.c
*
* Desc: lab-skeleton for the client side of an
* client-server application
*
* Revised by Dag Nystrom & Jukka Maki-Turja
* NOTE: the server must be started BEFORE the
* client.
*********************************************/
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "wrapper.h"
#include <assert.h>


planet_type* getUserInput(void);

void main(void) {

	HANDLE serverMailslot, clientMailslot;
	DWORD bytesWritten, procID;
	char procIDString[30], mailSlotString[18] = "\\\\.\\mailslot\\", clientMailslotName[256];
	planet_type *planet;

	procID = GetCurrentProcessId();
	assert(procID != 0);	//Check if function successful

	wsprintf(procIDString, "%d", procID);
	wsprintf(clientMailslotName, "\\\\.\\mailslot\\%d", mailSlotString, procID); //Generate clientMailSlotName

	serverMailslot = mailslotConnect("\\\\.\\mailslot\\serverMailslot");

	if (serverMailslot == INVALID_HANDLE_VALUE) {
		printf("Failed to get a handle to the server mailslot!!!\nHave you started the server?\n");
		getch();
		return;
	}

	clientMailslot = mailslotCreate(clientMailslotName);

	if (clientMailslot == INVALID_HANDLE_VALUE) {
		printf("Failed to get a handle to the client mailslot!!!\n");
		getch();
		return;
	}

	while (1)
	{
		planet = getUserInput();
		if (planet == NULL)
		{
			printf("\nError allocating memory. getUserInput()\n");
			return -1;
		}

		strcpy_s(planet->pid, 30, procIDString);



		bytesWritten = mailslotWrite(serverMailslot, (void *)planet, sizeof(planet_type));
		if (bytesWritten != -1)
			printf("\n---------------\nName: %s\nsx: %.2f\nsy: %.2f\nvx: %.2f\nvy: %.2f\nmass: %.2f\nlife: %d\npid: %s\n---------------\n\n%d bytes sent to server.\n",
			planet->name, planet->sx, planet->sy, planet->vx, planet->vy, planet->mass, planet->life, planet->pid, bytesWritten);
		else
			printf("Failed sending data to server\n");
		Sleep(1000);
	}


	mailslotClose(serverMailslot);
	mailslotClose(clientMailslot);
	free(planet);
	return;
}

planet_type* getUserInput(void)
{
	planet_type *planet;
	char input[256];
	int i;

	planet = (planet_type*)malloc(sizeof(planet_type)); //Alloc memory for planet
	if (planet == NULL)
		return NULL;

	printf("Please input the following information:");

	printf("\nName: ");
	if (fgets(input, sizeof(input), stdin)) {
		if (1 == sscanf_s(input, "%d", &i)) {
			/* i can be safely used */
			//strcpy_s(planet->name, 20, input);
			//printf("\n%s\n", planet->name);
		}
		strcpy_s(planet->name, 20, input);
		char *pos;
		if ((pos = strchr(planet->name, '\n')) != NULL)
			*pos = '\0';
	}

	printf("X-position: ");
	if (fgets(input, sizeof(input), stdin)) {
		if (1 == sscanf_s(input, "%d", &i)) {
			/* i can be safely used */
			planet->sx = atof(input);
		}
	}

	printf("Y-position: ");
	if (fgets(input, sizeof(input), stdin)) {
		if (1 == sscanf_s(input, "%d", &i)) {
			/* i can be safely used */
			planet->sy = atof(input);
		}
	}

	printf("X-velocity: ");
	if (fgets(input, sizeof(input), stdin)) {
		if (1 == sscanf_s(input, "%d", &i)) {
			/* i can be safely used */
			planet->vx = atof(input);
		}
	}

	printf("Y-velocity: ");
	if (fgets(input, sizeof(input), stdin)) {
		if (1 == sscanf_s(input, "%d", &i)) {
			/* i can be safely used */
			planet->vy = atof(input);
		}
	}

	printf("Mass: ");
	if (fgets(input, sizeof(input), stdin)) {
		if (1 == sscanf_s(input, "%d", &i)) {
			/* i can be safely used */
			planet->mass = atof(input);
		}
	}

	printf("Life: ");
	if (fgets(input, sizeof(input), stdin)) {
		if (1 == sscanf_s(input, "%d", &i)) {
			/* i can be safely used */
			planet->life = atoll(input);
		}
	}

	return planet;
}