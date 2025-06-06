//
// Created by Bot on 31/05/2025.
//
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "hub.h"
#include "../drone/drone.h"

Hub * server;
char isServerRunning = 0;
int exitCode = 0;
int droneStopedCount =0;


void handleDroneTermination(int sig){
    info("Drone termination registered!");
    droneStopedCount++;
}

void handleServerTermination(int sig){
    info("Terminating drones...");
    for (int i = 0; i < server->nDrones; ++i) {
        kill(server->dronePids[i], sig);
    }
    isServerRunning=0;
    if (sig==SIGINT) exitCode=1;
    exitCode=sig;
}

int forwardNextStep(Hub * hub){
	hub->step++;
    int newSize = sizeof(Vector3D*) * (hub->step +1);

    // Safely realloc and handle error
    Vector3D** temp = realloc(hub->simulation, newSize);
    if (temp == NULL) {
        perror("realloc failed");
        return -1;
    }
    hub->simulation = temp;

    // Allocate the new step (index is hub->step - 1)
    hub->simulation[hub->step] = calloc(hub->nDrones, sizeof(Vector3D));
    if (hub->simulation[hub->step] == NULL) {
        perror("calloc failed");
        return -1;
    }

    return 0;
}

int init(int nDrones, Hub *hub) {
    info("Initializing hub...");
    hub->nDrones = nDrones;
    hub->step = 0;
    hub->droneCollisions = calloc(nDrones, sizeof(int));
    hub->dronePids = calloc(nDrones, sizeof(pid_t));
    hub->simulation = calloc(1, sizeof(Vector3D *));
	hub->simulation[0] = calloc(nDrones, sizeof(Vector3D));
    hub->globalCollisions=0;
    info("Opening pipe....");
    int res = pipe(hub->serverPipe);
    if (res < 0) {
        error("Error opening server pipe...");
        return PIPE_ERROR;
    }
    return 0;
}


int spawnDrones(Hub * hub){
	for (int i = 0; i < hub->nDrones; ++i) {
        char debugMessage[100];
        sprintf(debugMessage, "Forking drone %d", i);
        debug(debugMessage);
        pid_t pid = fork();
        if (pid < 0) {
            error("Error forking drone...");
            return FORK_ERROR;
        }
        if (pid == 0) {
            //Child
            info("Starting drone...");
            Drone drone;
            drone.position.x = 0;
            drone.position.y = 0;
            drone.position.z = 0;
            drone.rotation = 0.0f;
            drone.id = i;
            drone.collisions = 0;
            drone.serverPipe = hub->serverPipe;
            drone.hubProcess=getppid();
            close(drone.serverPipe[READ_FD]);
            info("Drone started!");
            exit(execDrone(&drone));
        }
        hub->dronePids[i] = pid;
    }
	return 0;
}

int start(Hub *hub) {
    server=hub;
    isServerRunning=1;
    info("Starting simulation...");
    signal(SIGINT, handleServerTermination);
    signal(SIGTERM, handleServerTermination);
    signal(SIGUSR2, handleDroneTermination);
    spawnDrones(hub);
    close(hub->serverPipe[WRITE_FD]);
    while (isServerRunning) {
        if(droneStopedCount>=hub->nDrones){
            break;
        }
        for(int i=0;i<hub->nDrones;++i){
            Message message;
			debug("Listening to messages...");
            read(hub->serverPipe[READ_FD], &message, sizeof(Message));
            char buffer[200];
			sprintf(buffer, "Message received from drone %d!", message.droneId);
			debug(buffer);
            hub->simulation[hub->step][message.droneId]=message.vector3D;
        }

        int step = hub->step;
        int colisionDrones[hub -> nDrones];
        int numberOfColisions=0;

		debug("Detecting for colisions...");
        for(int i=0;i<hub->nDrones;++i){
           for(int j=i; j<hub->nDrones;++j){
                if(i==j) continue;
                Vector3D pos1 = hub->simulation[step][i];
                Vector3D pos2 = hub->simulation[step][j];
                if(pos1.x == pos2.x && pos1.y == pos2.y && pos1.z == pos2.z){
                    if(colisionDrones[i]==0 || colisionDrones[j]==0){
                        hub->globalCollisions++;
                    }
                    colisionDrones[i]=1;
                    colisionDrones[j]=1;
                }
           }
        }
		debug("Signaling for colisions...");
        for(int i=0;i<hub->nDrones; ++i){
            if(colisionDrones[i]==1){
                kill(hub->dronePids[i],SIGUSR1);
            }
        }
		debug("Forward to next step...");
       	forwardNextStep(hub);

		debug("Signaling next step...");
        for(int i=0;i<hub->nDrones;++i){
            kill(hub->dronePids[i], SIGUSR2);
        }
		debug("Repeating cycle...");
    }

	FILE * reportFile = fopen("./report.txt", "w+");
    time_t now = time(NULL);
    struct tm *utc = gmtime(&now);  // use localtime(&now) for local time

    fprintf(reportFile, "Shodrone Report                                                                                         %02d/%02d/%04d %02d:%02d\n\n",
        utc->tm_mday,
        utc->tm_mon + 1,         // tm_mon is 0-based
        utc->tm_year + 1900,     // tm_year is years since 1900
        utc->tm_hour,
        utc->tm_min);
    fprintf(reportFile, "Drones in use: %d\n", hub->nDrones);
    fprintf(reportFile, "Collisions Detected in general: %d\n", hub->globalCollisions);
    fprintf(reportFile, "Early Shutdown Drones: %d\n", droneStopedCount);
    fprintf(reportFile, "Status: %s\n\n", droneStopedCount!=0 ? "NO GO" : hub->globalCollisions==0 ? "OK" : "PROBLEMATIC");
    fprintf(reportFile, "==============================================LOG TRACE====================================================\n");
	fclose(reportFile);
    return 0;
}

void clean(Hub *hub) {
    info("Cleaning up simulation...");
    for (int i = 0; i < hub->nDrones; ++i) {
        char buff[200];
        sprintf(buff, "Waiting for drone %d to stop!", i);
        info(buff);
        wait(hub->dronePids + i);
        sprintf(buff, "Drone %d stopped!", i);
        info(buff);
    }
    info("General Cleanup...");
    free(hub->droneCollisions);
    free(hub->dronePids);
    info("Close server pipes...");
    close(hub->serverPipe[READ_FD]);
    close(hub->serverPipe[WRITE_FD]);
    info("Cleaned up completed!");
}
