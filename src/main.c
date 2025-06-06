#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "banner/banner.h"
#include "logger/logger.h"
#include "simulation/hub/hub.h"

void shutdownAppOnError(int code) {
    if (code != 0) {
        char buff[200];
        sprintf(buff, "Shutting down app with error code: %d!", code);
        error(buff);
        exit(code);
    }
}

void aggregateReportAndLog(){
    FILE *report = fopen("./report.txt", "a");
    if (!report) {
        perror("Failed to open report.txt");
        return;
    }
    fputc('\n', report);  // mimic `echo` by appending a newline
    fclose(report);

    // Open log.txt for reading
    FILE *log = fopen("./log.txt", "r");
    if (!log) {
        perror("Failed to open log.txt");
        return;
    }

    // Re-open report.txt for appending
    report = fopen("./report.txt", "a");
    if (!report) {
        perror("Failed to open report.txt for appending");
        fclose(log);
        return;
    }

    // Copy contents of log.txt to report.txt
    int ch;
    while ((ch = fgetc(log)) != EOF) {
        fputc(ch, report);
    }

    fclose(log);
    fclose(report);
}


int main() {
    print_banner();
    char * banner =  bannerStr();
    initLogger(banner);
    free(banner);
    Hub hub;
    info("Starting Shodrone app...");
    shutdownAppOnError(init(5, &hub));
    info("Hub initialized! Starting simulation...");
    shutdownAppOnError(start(&hub));
    info("Cleaning up simulation");
    clean(&hub);
    info("Simulation ended!");
    aggregateReportAndLog();
    return 0;
}