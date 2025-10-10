#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "decode.h"
#include "log.h"

int setParams(int argc, char *argv[], int depth, char *image, int *flags) {
  if (argc == depth) return 0;
  if (argv[depth][0] == '-') {
    for (int i = 0; i < sizeof(argv[depth]) / sizeof(argv[depth][0]); i++) {
      switch (argv[depth][i]) {
        case 'o': flags[0] = 1; log_status(0, "Set fileout to true"); break;
        case 's': flags[1] = 1; log_status(0, "Set summary to true"); break;
        case 'h': flags[2] = 1; log_status(0, "Set hex to true"); break;
        case 'v': flags[3] = 1; log_status(0, "Set verbose to true"); break;
      }
    }
  } else if (strchr(argv[depth], '.') != NULL) {
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
    FILE *file = fopen(argv[depth], "rb");
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

  depth++;

  return setParams(argc, argv, depth, image, flags);
}

int main(int argc, char *argv[]) {
	char image[256] = "img/example.jpg";
  int flags[5] = {0};

  switch (setParams(argc, argv, 1, image, flags)) {
    case 1: log_status(1, "Only JPEGs are supported"); return 1;
    case 2: log_status(1, "Image does not exist in the specified location"); return 2;
    default: log_status(0, "Running Akscii");
  }

  log_init(image, flags);
  decode_JPEG(image);

  return 0;
}
