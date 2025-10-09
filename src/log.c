#include "log.h"

// TODO: Ideally these logs should capture the entire lifecycle of the program including the bootup
// parameter setting and logging activation

// TODO: Add some more logging options e.g.
// Summary: Marker: SOI
//          Length: 0
// Verbose: Line: 0        Decimal: 255      Hex: FF       Character: �
// Verbose Summary: FF DA 00 00 00 00 00 00 00 00
//                        00 00 00 00 00 00 00 00

// TODO: Make the output more verbose by using vsnprintf

void log_init(char *image, int fileoutFlag, int *flags) {
  fileout = flags[0];
  summary = flags[1];
  hex = flags[2];
  verbose = flags[3];

  if (fileout) {
    // Temporarily stop fileout from activating for terminal logging
    fileout--;

    image = strrchr(image, '/') ? strrchr(image, '/') + 1: image;

    char *extentionPosition = strrchr(image, '.');
    char imageName[256];

    size_t nameLength = extentionPosition - image;
    strncpy(imageName, image, nameLength);
    imageName[nameLength] = '\0';

    snprintf(logFile, sizeof(logFile), "logs/%s %s.log", get_time(), imageName);

    FILE *fp;

    char msg[1024];

    fp = fopen(logFile, "wx");
    if (fp == NULL) {
      snprintf(msg, sizeof(msg), "%s already exists, would you like to overwrite it? (y/n) ", logFile);
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

void log_marker(struct MARKER marker){

  if (summary) log_summary(marker);

  if (hex) log_hex(marker);
  
  return;
}

void log_summary(struct MARKER marker) {
  char msg[1024];
  char markerName[6];
  
  // FIXME: remove redundant code if possible

  switch (marker.code) {
    case SOI: {
      snprintf(markerName, sizeof(markerName), "SOI");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", markerName, marker.length);
      break;
    }
    case EOI: {
      snprintf(markerName, sizeof(markerName), "EOI");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", markerName, marker.length);
      break;
    }
    case APP: {
      snprintf(markerName, sizeof(markerName), "APP");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nIdentifier: %s\nVersion: %d.%d\nUnits: %d\nDensity: (%d, %d)\nThumbnail: (%d, %d)", markerName, marker.length, marker.identifier, marker.majorVerion, marker.minorVersion, marker.units, marker.densityX, marker.densityY, marker.thumbnailX, marker.thumbnailY);
      break;
    }
    case APP1: {
      snprintf(markerName, sizeof(markerName), "EXF");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", markerName, marker.length);
      break;
    }
    case DQT: {
      // TODO: Add quality tables
      snprintf(markerName, sizeof(markerName), "DQT");
      int tableLengthX = sizeof(marker.table) / sizeof(marker.table[0]);
      int tableLengthY = sizeof(marker.table[0]) / sizeof(marker.table[0][0]);
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nTable: (%d, %d)", markerName, marker.length, tableLengthX, tableLengthY);
      break;
    }
    case SOF: {
      // TODO: Add factor tables
      snprintf(markerName, sizeof(markerName), "SOF");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nPrecision: %d\nLines: %d\nSamples/Line: %d\nComponents: %d", markerName, marker.length, marker.precision, marker.lines, marker.samples, marker.components);
      break;
    }
    case DHT: {
      snprintf(markerName, sizeof(markerName), "DHT");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nClass: %d\nDestination: %d", markerName, marker.length, marker.class, marker.destination);
      break;
    }
    case DRI: {
      snprintf(markerName, sizeof(markerName), "DRI");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", markerName, marker.length);
      break;
    }
    case SOS: {
      snprintf(markerName, sizeof(markerName), "SOS");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nComponents: %d\nSpectral Select Start: %d\nSpectral Select End: %d\nSuccessive Approx High: %d\nSuccessive Approx Low: %d", markerName, marker.length, marker.components, marker.spectralSelectStart, marker.spectralSelectEnd, marker.successiveHigh, marker.successiveLow);
      break;
    }
    case DATA: {
      snprintf(markerName, sizeof(markerName), "DATA");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", markerName, marker.length);
      break;
    }
    case COM: {
      snprintf(markerName, sizeof(markerName), "COM");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", markerName, marker.length);
      break;
    }
    default: {
      snprintf(markerName, sizeof(markerName), "UNK");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", markerName, marker.length);
      break;
    }
  }

  log_msg(msg);
}

void log_hex(struct MARKER marker) {
  char msg[1024];
  char *currentPosition = msg;
  char *endPosition = msg + sizeof(msg);

  currentPosition += snprintf(currentPosition, endPosition - currentPosition, "%02X %02X ", FF, marker.code);

  for (int i = 0; i < marker.length; i++) {

    if (i % 11 == 0 && i != 0) {
      int written = snprintf(currentPosition, endPosition - currentPosition, "\n      ");
      if (written < 0 || written >= endPosition - currentPosition) break;
      currentPosition += written;
    }

    int written = snprintf(currentPosition, endPosition - currentPosition, "%02X ", marker.data[i]);
    if (written < 0 || written >= endPosition - currentPosition) break;
    currentPosition += written;
  }

  size_t used = strlen(msg);
  if (used < sizeof(msg) - 1) {
    msg[used++] = '\n';
    msg[used] = '\0';
  } else {
    msg[sizeof(msg) - 2] = '\n';
  }

  log_msg(msg);
}

void log_verbose(int currentChar) {
  char msg[1024];

  line++;

  if (verbose == 1) {
    snprintf(msg, sizeof(msg), "Line: %-8d Decimal: %-8d Hex: %-8X Character: %s", line, currentChar, currentChar, currentChar ? (char[]){ (char) currentChar, '\0'} : "none");

    log_msg(msg);
  }

  if (verbose == 2) {
    snprintf(msg, sizeof(msg), "%02X", currentChar);

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