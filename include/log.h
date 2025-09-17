#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

static int fileout = 0;
// 0: no fileout
// 1: fileout

static int verbose = 0;
// 0: quiet
// 1: verbose

static char logFile[256 + 64] = {0};
// image name is 256, time stamp is 64

void log_init(char *image, int fileoutFlag, int verboseFlag);
void log_status(int messageType, char *info); // Log a status
void log_summary(); // Log a summary of the marker
void log_verbose(int lineNumber, int currentChar); // Log the line and character
void log_fileout(char* message); // Output message to file

char* get_time();
int get_fileout();
int get_verbose();

#endif