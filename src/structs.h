//
// Created by Bot on 31/05/2025.
//

#include <sys/types.h>
#ifndef SHODRONE_STRUCTS_H
#define SHODRONE_STRUCTS_H

#define READ_FD 0
#define WRITE_FD 1


typedef struct{
    int x;
    int y;
    int z;
} Vector3D;


typedef struct {
    Vector3D vector3D;
    int droneId;
} Message;

typedef struct{
    Vector3D position;
    int * serverPipe;
    pid_t pid;
    float rotation;
    int id;
    int collisions;
    pid_t hubProcess;
}Drone;

typedef struct {
    Vector3D ** simulation;
    int nDrones;
    int step;
    int* droneCollisions; //Essentially int droneCollisions[ndrones]
    pid_t* dronePids; // Essentially pid_t dronePids[ndrones]
    int serverPipe[2];
    int globalCollisions;
} Hub;

#endif //SHODRONE_STRUCTS_H
