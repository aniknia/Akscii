#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "marker_struct.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

void log_init(char *image, int *flags);
void log_status(int message_type, char *info); // Log a status
void log_marker(struct marker *m);
void log_summary(struct marker *m); // Log a summary of the data
void log_hex(struct marker *m); // Log a shortened hex of the data
void log_verbose(int current_character); // Log the line and character
void log_message(char* message); // Output message message

char* get_time();
int get_fileout();
int get_verbose();

#endif