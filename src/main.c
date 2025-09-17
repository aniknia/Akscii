#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "decode.h"
#include "log.h"


int isnum(char str[]) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (isdigit(str[i])) {
      return 1;
    }
  }
  return 0;
}

//change to add concise, verbose, and message option
int setParams(int argc, char *argv[], int depth, char *image, int *fileoutptr, int *verboseptr) {
  if (argc == depth) {
    return 0;
	} else {
    if (isnum(argv[depth])) {
      // Nothing yet
    } else if (strcmp(argv[depth], "-o") == 0) {
      // Checks for fileout flag
      *fileoutptr = 1;
      log_status(0, "Set fileout to true");
    } else if (strcmp(argv[depth], "-v") == 0) {
      // Checks for verbose flag
      *verboseptr = 1;
      log_status(0, "Set verbose to true");
    } else {
      // Assume an image file
      char *extension = strchr(argv[depth], '.');
      if ((strcasecmp(extension, ".jpg") != 0) && (strcasecmp(extension, ".jpeg") != 0)) {
        char msg[512];
        snprintf(msg, sizeof(msg), "%s does not have an acceptable extension", argv[depth]);
        log_status(1, msg);
        return 1;
      }
      log_status(0, "Image was an acceptable format");

      // Tests that file can be opened
      FILE *file = fopen(argv[depth], "r");
      if (file == NULL) {
        char msg[512];
        snprintf(msg, sizeof(msg), "Failed to open %s", argv[depth]);
        log_status(1, msg);
        return 2;
      }
      fclose(file);

      strcpy(image, argv[depth]);

      char msg[512];
      snprintf(msg, sizeof(msg), "%s exists", argv[depth]);
      log_status(0, msg);
    }
  }

  depth++;

  return setParams(argc, argv, depth, image, fileoutptr, verboseptr);
}

int main(int argc, char *argv[]) {
	char image[256] = "img/example.jpg";
  int fileout = 0;
  int verbose = 0;

  switch (setParams(argc, argv, 1, image, &fileout, &verbose)) {
    case 1: log_status(1, "Only JPEGs are supported"); return 1;
    case 2: log_status(1, "Image does not exist in the specified location"); return 2;
    default: log_status(0, "Running Akscii");
  }

  log_init(image, fileout, verbose);
  decode_JPEG(image);

  return 0;
}
