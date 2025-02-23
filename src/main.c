#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "decoder.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


int isnum(char str[]) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (isdigit(str[i])) {
      return 1;
    }
  }
  return 0;
}

int setParams(int argc, char *argv[], char *image, char *mode, int depth, char lastchangedvalue[5]) {
  char lastreadvalue[5] = "none";
  if (argc == depth) {
    return 0;
	} else {
    if (isnum(argv[depth])) {
      // Nothing yet
    } else if (strcmp(argv[depth], "mode") == 0) {
      strcpy(lastreadvalue, "mode");
    } else if(strcmp(lastreadvalue, "none") != 0) {
      // Setting mode
      if(strcmp(lastreadvalue, "mode") == 0) {
        if(strcmp(argv[depth], "decode")) {
          strcpy(mode, "decode");
          printf("Set mode to decode\n");
        } else if(strcmp(argv[depth], "encode") == 0) {
          strcpy(mode, "encode");
          printf("Set mode to encode\n");
        }
      }
    } else {
      // Assume an image file
      char *extension = strchr(argv[depth], '.');
      if ((strcasecmp(extension, ".jpg") != 0) && (strcasecmp(extension, ".jpeg") != 0)) {
        return 1;
      }
      printf("Image was an acceptable format\n");

      // Tests that file can be opened
      FILE *file = fopen(argv[depth], "r");
      if (file == NULL) {
        return 2;
      }
      fclose(file);

      strcpy(image, argv[depth]);
    }
  }

  depth++;

  return setParams(argc, argv, image, depth, lastreadvalue);
}

int main(int argc, char *argv[]) {
	char image[256] = "img/example.jpg";
  char mode[256] = "decode";

  if (argc > 1) {
    switch (setParams(argc, argv, image, mode, 1, "none")) {
      case 1: printf(RED "Error" RESET ": Only JPEGs are supported\n"); return 1;
      case 2: printf(RED "Error" RESET ": Image does not exist in the specified location\n"); return 2;
      default: printf(GRN "Success" RESET ": Running Akscii\n");
    }
  }

    printf("Image: %s\n", image);

  if(strcmp(mode, "decode") == 0) {
    decoder.decodeJPEG(image);
  }
  
  return 0;
}
