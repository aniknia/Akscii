#include "log.h"

// TODO: Ideally these logs should capture the entire lifecycle of the program including the bootup
// parameter setting and logging activation

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

    char msg[1024];
    snprintf(msg, sizeof(msg), "Starting log for %s", image);
    log_status(0, msg);

    log_status(0, "File logging activated");
  }
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

  snprintf(msg, sizeof(msg), "%s%s%s", time, msgType, info);

  log_msg(msg);

  return;
}

void log_summary(struct MARKER marker){
  char msg[1024];
  char markerName[4];

  // TODO: log the marker plus info like length, version, units, etc

  switch (marker.code) {
        case SOI: snprintf(markerName, sizeof(markerName), "SOI"); break;
        case EOI: snprintf(markerName, sizeof(markerName), "EOI"); break;
        case APP: snprintf(markerName, sizeof(markerName), "APP"); break;
        case APP1: snprintf(markerName, sizeof(markerName), "EXF"); break;
        case DQT: snprintf(markerName, sizeof(markerName), "DQT"); break;
        case SOF: snprintf(markerName, sizeof(markerName), "SOF"); break;
        case DHT: snprintf(markerName, sizeof(markerName), "DHT"); break;
        case DRI: snprintf(markerName, sizeof(markerName), "DRI"); break;
        case SOS: snprintf(markerName, sizeof(markerName), "SOS"); break;
        case COM: snprintf(markerName, sizeof(markerName), "COM"); break;
        default: snprintf(markerName, sizeof(markerName), "UNK"); break;
  }

  snprintf(msg, sizeof(msg), "Marker: %s \t Length: %d", markerName, marker.length);

  log_msg(msg);

  return;
}

void log_verbose(int lineNumber, int currentChar) {
  char msg[1024];

  if (verbose) {
    snprintf(msg, sizeof(msg), "Line: %-8d Decimal: %-8d Hex: %-8X Character: %s", lineNumber, currentChar, currentChar, currentChar ? (char[]){ (char) currentChar, '\0'} : "none");

    log_msg(msg);
  }

  return;
}

void log_msg(char* message) {
  if(fileout) {
    FILE *fp;
    fp = fopen(logFile, "a");

    char fileoutput[strlen(message) + 2];
    snprintf(fileoutput, sizeof(fileoutput), "%s\n", message);

    fputs(fileoutput, fp);
    fclose(fp);
  }

  printf("%s\n", message);

  return;
}

char* get_time() {
  static char currentTime[64];
  char format[20] = "%Y-%m-%d %Hh%Mm%Ss";

  time_t now = time(NULL);
  strftime(currentTime, sizeof(currentTime), format, localtime(&now));
  return currentTime;
}

int get_fileout() {
  return fileout;
}

int get_verbose() {
  return verbose;
}