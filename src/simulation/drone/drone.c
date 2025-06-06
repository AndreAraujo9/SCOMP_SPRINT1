#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "drone.h"
#include "../../structs.h"
#include "../../logger/logger.h"
#include <stdlib.h>
#include <time.h>

int droneId=0;
pid_t parentPid;
int nColisions=0;
char isRunning = 0;
char output=0;
int maxColisions=10;
int processStep=1;
char ariseMessage[100];

void handleNextStep(int sig){
	debug(ariseMessage);
    processStep=1;
}

void handleColision(int sig){
    nColisions++;
    char buffer[200];
    sprintf(buffer, "Colision on Drone %d detected!",droneId);
    warning(buffer);
}

void handleTermination(int sig){
    info("Drone terminated!");
    isRunning=0;
    if (sig == SIGINT){
        output=0;
    }
    output=sig;
    kill(parentPid,SIGUSR2);
}


int simulateMovement(int min, int max, Vector3D* vector3D) {
    vector3D->x = rand() % (max - min + 1) + min;
    vector3D->y = rand() % (max - min + 1) + min;
    vector3D->z = rand() % (max - min + 1) + min;
    return 0;
}




int execDrone(Drone * drone){
	sprintf(ariseMessage, "Awaken drone %d!", drone->id);
    isRunning=1;
    droneId=drone->id;
    parentPid = drone->hubProcess;
    signal(SIGINT, handleTermination);
    signal(SIGTERM, handleTermination);
    signal(SIGUSR1, handleColision);
    signal(SIGUSR2, handleNextStep);

    while (isRunning) {
        srand(time(NULL));
        Vector3D newPosition;
		if(maxColisions<=nColisions){
            char buffer[500];
            sprintf(buffer,"Max colisions (%d) reached for drone %d! Shutting down!", maxColisions, droneId);
            error(buffer);
            kill(getpid(), SIGTERM);
        }
        simulateMovement(0, 100, &newPosition);
        Message message;
        message.vector3D = newPosition;
        message.droneId = drone->id;
        int result = write(drone->serverPipe[WRITE_FD], &message, sizeof(Message));
		processStep=0;
		char buffer[500];
		sprintf(buffer, "Drone %d sent message with message size %d!", droneId, result);
		info(buffer);
		debug("Awaiting further instructions...");
		while(!processStep){}
    }
    kill(getpid(), SIGTERM);
    return output;
}
