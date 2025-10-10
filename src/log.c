#include "log.h"

// Flags
static int fileout = 0;
static int verbose = 0;
static int summary = 0;
static int hex = 0;

static char logFile[256 + 64] = {0}; // image name is 256, time stamp is 64

static int line = 0;

// TODO: Ideally these logs should capture the entire lifecycle of the program including the bootup

// TODO: Make the output more verbose by using vsnprintf

void log_init(char *image, int *flags) {
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
  char msg[1024];
  char message_type[64];

  snprintf(time, sizeof(time), "[%s%s%s] ", RED, get_time(), RESET);

  // TODO: Colors contain escape codes that cause the log logFile to be unreadable, fix
  // Right now this is mitigated by using a viewer that processes ansi
  switch (messageType) {
    case 1:
      snprintf(message_type, sizeof(message_type), "%sError%s: ", RED, RESET);
      break;
    case 2:
      snprintf(message_type, sizeof(message_type), "%sWarning%s: ", YEL, RESET);
      break;
    default:
      snprintf(message_type, sizeof(message_type), "%sSuccess%s: ", GRN, RESET);
  }

  snprintf(msg, sizeof(msg), "%s%s%s", time, message_type, info);

  log_message(msg);

  return;
}

void log_marker(struct marker *marker){

  if (summary) log_summary(marker);

  if (hex) log_hex(marker);
  
  return;
}

void log_summary(struct marker *marker) {
  char msg[1024];
  char marker_name[6];
  
  // FIXME: remove redundant code if possible

  switch (marker->code) {
    case MARKER_SOI: {
      snprintf(marker_name, sizeof(marker_name), "SOI");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
    case MARKER_EOI: {
      snprintf(marker_name, sizeof(marker_name), "EOI");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
    case MARKER_APP: {
      snprintf(marker_name, sizeof(marker_name), "APP");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nIdentifier: %s\nVersion: %d.%d\nUnits: %d\nDensity: (%d, %d)\nThumbnail: (%d, %d)", marker_name, marker->length, marker->app.identifier, marker->app.major_version, marker->app.minor_version, marker->app.units, marker->app.density_x, marker->app.density_y, marker->app.thumbnail_x, marker->app.thumbnail_y);
      break;
    }
    case MARKER_APP1: {
      snprintf(marker_name, sizeof(marker_name), "EXF");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
    case MARKER_DQT: {
      // TODO: Add quality tables
      snprintf(marker_name, sizeof(marker_name), "DQT");
      int table_length_x = sizeof(marker->dqt.table) / sizeof(marker->dqt.table[0]);
      int table_length_y = sizeof(marker->dqt.table[0]) / sizeof(marker->dqt.table[0][0]);
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nTable: (%d, %d)", marker_name, marker->length, table_length_x, table_length_y);
      break;
    }
    case MARKER_SOF: {
      // TODO: Add factor tables
      snprintf(marker_name, sizeof(marker_name), "SOF");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nPrecision: %d\nLines: %d\nSamples/Line: %d\nComponents: %d", marker_name, marker->length, marker->sof.precision, marker->sof.lines, marker->sof.samples, marker->sof.components);
      break;
    }
    case MARKER_DHT: {
      snprintf(marker_name, sizeof(marker_name), "DHT");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
    case MARKER_DRI: {
      snprintf(marker_name, sizeof(marker_name), "DRI");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
    case MARKER_SOS: {
      snprintf(marker_name, sizeof(marker_name), "SOS");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d\nComponents: %d\nSpectral Select Start: %d\nSpectral Select End: %d\nSuccessive Approx High: %d\nSuccessive Approx Low: %d", marker_name, marker->length, marker->sos.components, marker->sos.spectral_select_start, marker->sos.spectral_select_end, marker->sos.successive_high, marker->sos.successive_low);
      break;
    }
    case MARKER_DATA: {
      snprintf(marker_name, sizeof(marker_name), "DATA");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
    case MARKER_COM: {
      snprintf(marker_name, sizeof(marker_name), "COM");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
    default: {
      snprintf(marker_name, sizeof(marker_name), "UNK");
      snprintf(msg, sizeof(msg), "Marker: %s\nLength: %d", marker_name, marker->length);
      break;
    }
  }

  log_message(msg);
}

void log_hex(struct marker *marker) {
  char msg[1024];
  char *current_position = msg;
  char *end_position = msg + sizeof(msg);

  current_position += snprintf(current_position, end_position - current_position, "%02X %02X ", 0xFF, marker->code);

  for (int i = 0; i < marker->length; i++) {

    if (i % 11 == 0 && i != 0) {
      int written = snprintf(current_position, end_position - current_position, "\n      ");
      if (written < 0 || written >= end_position - current_position) break;
      current_position += written;
    }

    int written = snprintf(current_position, end_position - current_position, "%02X ", marker->data[i]);
    if (written < 0 || written >= end_position - current_position) break;
    current_position += written;
  }

  size_t used = strlen(msg);
  if (used < sizeof(msg) - 1) {
    msg[used++] = '\n';
    msg[used] = '\0';
  } else {
    msg[sizeof(msg) - 2] = '\n';
  }

  log_message(msg);
}

void log_verbose(int current_character) {
  char msg[1024];

  line++;

  if (verbose == 1) {
    snprintf(msg, sizeof(msg), "Line: %-8d Decimal: %-8d Hex: %-8X Character: %s", line, current_character, current_character, current_character ? (char[]){ (char) current_character, '\0'} : "none");

    log_message(msg);
  }

  if (verbose == 2) {
    snprintf(msg, sizeof(msg), "%02X", current_character);

    log_message(msg);
  }

  return;
}

void log_message(char* msg) {
  if(fileout) {
    FILE *fp;
    fp = fopen(logFile, "a");

    char fileoutput[strlen(msg) + 2];
    snprintf(fileoutput, sizeof(fileoutput), "%s\n", msg);

    fputs(fileoutput, fp);
    fclose(fp);
  }

  printf("%s\n", msg);

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