#include "log.h"

// TODO: Ideally these logs should capture the entire lifecycle of the program including the bootup
// parameter setting and logging activation

// TODO: change log_status to have variable number of inputs

void log_init(char *image, int fileoutFlag, int verboseFlag) {
  fileout = fileoutFlag;
  verbose = verboseFlag;

  if (fileout) {
    // Temporarily stop fileout from activating for terminal logging
    fileout--;

    image = strrchr(image, '/') ? strrchr(image, '/') + 1: image;

    char *extentionPosition = strrchr(image, '.');
    char imageName[256];

    size_t nameLength = extentionPosition - image;
    strncpy(imageName, image, nameLength);

    snprintf(logFile, sizeof(logFile), "logs/%s %s.log", get_time(), imageName);

    FILE *fp;

    fp = fopen(logFile, "wx");
    if (fp == NULL) {
      char msg[512];
      snprintf(msg, sizeof(msg), "%s already exists, would you like to overwrite it? (y/n) ", image);
      log_status(2, msg);
      char input = getchar();
      getchar();
      if (input == 'n') {
        log_status(1, "File logging deactivated");
        return;
      } else {
        fp = fopen(logFile, "w");
      }
    }
    fileout++;
    fclose(fp);

    char msg[512];
    snprintf(msg, sizeof(msg), "Starting log for %s", image);
    log_status(0, msg);

    log_status(0, "File logging activated");
  }
}

char* get_time() {
  static char currentTime[64];
  char format[20] = "%Y-%m-%d %Hh%Mm%Ss";

  time_t now = time(NULL);
  strftime(currentTime, sizeof(currentTime), format, localtime(&now));
  return currentTime;
}

void log_status(int messageType, char *info) {
  char time[64];
  char msgType[64];
  char msg[1024];

  snprintf(time, sizeof(time), "[%s%s%s] ", RED, get_time(), RESET);

  // TODO: Colors contain escape codes that cause the log logFile to be unreadable, fix
  // Right now this is mitigated by using a viewer that processes ansi
  switch (messageType) {
    case 1:
      snprintf(msgType, sizeof(msgType), "%sError%s: ", RED, RESET);
      break;
    case 2:
      snprintf(msgType, sizeof(msgType), "%sWarning%s: ", YEL, RESET);
      break;
    default:
      snprintf(msgType, sizeof(msgType), "%sSuccess%s: ", GRN, RESET);
  }

  snprintf(msg, sizeof(msg), "%s%s%s\n", time, msgType, info);

  if (fileout) {
    log_fileout(msg);
  }

  printf("%s", msg);
  return;
}

void log_summary(){
  char msg[1024];

  // TODO: log the marker plus info like length, version, units, etc

  return;
}

void log_verbose(int lineNumber, int currentChar) {
  char msg[1024];

  // TODO: This should be reworked to remove the edge case
  if (currentChar == 0) {
    snprintf(msg, sizeof(msg), "Line: %-8d Decimal: %-8d Hex: %-8X Character: none\n", lineNumber, currentChar, currentChar);
  } else {
    snprintf(msg, sizeof(msg), "Line: %-8d Decimal: %-8d Hex: %-8X Character: %c\n", lineNumber, currentChar, currentChar, currentChar);
  }

  if (fileout) {
    log_fileout(msg);
  }

  printf("%s", msg);
  return;
}

void log_fileout(char* message) {
  FILE *fp;
  fp = fopen(logFile, "a");

  char fileoutput[strlen(message) + 2];
  snprintf(fileoutput, sizeof(fileoutput), "%s", message);

  fputs(fileoutput, fp);
  fclose(fp);

  return;
}

int get_fileout() {
  return fileout;
}

int get_verbose() {
  return verbose;
}