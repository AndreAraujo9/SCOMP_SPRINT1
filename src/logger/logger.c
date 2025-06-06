//
// Created by Bot on 31/05/2025.
//

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "logger.h"


const char* LOG_FILE_NAME= "./log.txt";


void initLogger(char * initStr){
    FILE * logFile = fopen(LOG_FILE_NAME, "w");
    fprintf(logFile,"%s\n", initStr);
    fclose(logFile);
}

void get_timestamp(char *buffer, size_t size) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm *tm_info = localtime(&tv.tv_sec);
    int millis = tv.tv_usec / 1000;

    // Format: dd-MM-yyyyThh:mm:ss:SSSZ
    strftime(buffer, size, "%d-%m-%YT%H:%M:%S", tm_info);
    snprintf(buffer + strlen(buffer), size - strlen(buffer), ":%03dZ", millis);
}

void logMessage(char *message, char *level) {
    FILE* f = fopen(LOG_FILE_NAME,"a");
    char date[40];
    get_timestamp(date, 40);
    char lev[10];
    sprintf(lev, "[%s]", level);
    char buffer[500];
    sprintf(buffer, "%-10s %s [PID: %d] %s\n", lev, date, getpid(), message);
    fprintf(f, "%s", buffer);
    printf("%s", buffer);
    fclose(f);
}


void info(char *message) {
    logMessage(message, "INFO");
}

void warning(char *message) {
    logMessage(message, "WARNING");
}

void error(char *message) {
    logMessage(message, "ERROR");
}

void debug(char *message) {
    logMessage(message, "DEBUG");
}