#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define FF 255
#define SOI 0xD8    //Start of Image
#define EOI 0xD9    //End of Image
#define SOS 0xDA    //Start of Scan
#define COM 0xFE    //Comments

#define SOF 0xC0    //Baseline DCT
#define SOF1 0xC1   //Extended Sequential DCT
#define SOF2 0xC2   //Progressive DCT
#define SOF3 0xC3   //Lossless Sequential
#define SOF4 0xC4   //
#define SOF5 0xC5   //Differential Sequential DCT
#define SOF6 0xC6   //Differential Progressive DCT
#define SOF7 0xC7   //Differential Lossless
#define SOF8 0xC8   //
#define SOF9 0xC9   //Extended Sequential DCT
#define SOF10 0xCA  //Progressive DCT
#define SOF11 0xCB  //Lossless Sequential
#define SOF12 0xCC  //
#define SOF13 0xCD  //Differential Sequential DCT
#define SOF14 0xCE  //Differential Progressive DCT
#define SOF15 0xCF  //Differential Lossless

#define APP 0xE0    //
#define APP1 0xE1   //EXIF
#define APP2 0xE2   //
#define APP3 0xE3   //
#define APP4 0xE4   //
#define APP5 0xE5   //
#define APP6 0xE6   //
#define APP7 0xE7   //
#define APP8 0xE8   //
#define APP9 0xE9   //
#define APP10 0xEA  //
#define APP11 0xEB  //
#define APP12 0xEC  //
#define APP13 0xED  //
#define APP14 0xEE  //
#define APP15 0xEF  //

#define DAC 0xCC    //Define Arithmetic Conditioning Table
#define DHP 0xDE    //Define Hierarchical Progression
#define DHT 0xC4    //Define Huffman Table
#define DNL 0xDC    //Define Number of Lines
#define DQT 0xDB    //Define Quantization Table
#define DRI 0xDD    //Define Restart Interval

int isnum(char str[]) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (isdigit(str[i])) {
      return 1;
    }
  }
  return 0;
}

int setParams(int argc, char *argv[], int *testing, int *testvalue, int *fileout, int *fileoutvalue, char *image, int depth, char lastchangedvalue[5]) {
  char lastreadvalue[5] = "none";
  if (argc == depth) {
    return 0;
	} else {
    if (isnum(argv[depth])) {
      // Nothing yet
    } else if (strcmp(argv[depth], "test") == 0) {
			printf("Testing mode is enabled\n");
			*testing = 1;
      strcpy(lastreadvalue, "test");
    } else if (strcmp(argv[depth], "fileout") == 0) {
      printf("Fileout mode is enabled\n");
      *fileout = 1;
      strcpy(lastreadvalue, "file");
    } else if (strcmp(lastchangedvalue, "file") == 0) {
      if (strcmp(argv[depth], "all") == 0) {
        *fileoutvalue = 0;
        printf("Fileout is set to all\n");
      } else if (strcmp(argv[depth], "short") == 0) {
        *fileoutvalue = 1;
        printf("Fileout is set to short\n");
      } else if (strcmp(argv[depth], "verbose") == 0) {
        *fileoutvalue = 2;
        printf("Fileout is set to verbose\n");
      } else {
        *fileout = 0;
        printf("Unrecognized Value: Fileout is set to all\n");
      }
    } else if (strcmp(lastchangedvalue, "test") == 0) {
      // No testing flags yet
    } else {
      char *extension = strchr(argv[depth], '.');
      if ((strcasecmp(extension, ".jpg") != 0) && (strcasecmp(extension, ".jpeg") != 0)) {
        return 1;
      }
      printf("Image was an acceptable format\n");
      FILE *file = fopen(argv[depth], "r");
      if (file == NULL) {
        return 2;
      }
      
      fclose(file);

      strcpy(image, argv[depth]);
    }
  }

  depth++;

  return setParams(argc, argv, testing, testvalue, fileout, fileoutvalue, image, depth, lastreadvalue);
}

int main(int argc, char *argv[]) {
	char image[256] = "img/example.jpg";
	int testing = 0;
  int testvalue = 20;
  int fileout = 0;
  int fileoutvalue = 0;

  if (argc > 1) {
    switch (setParams(argc, argv, &testing, &testvalue, &fileout, &fileoutvalue, image, 1, "none")) {
      case 1: printf(RED "Error" RESET ": Only JPEGs are supported\n"); return 1;
      case 2: printf(RED "Error" RESET ": Image does not exist in the specified location\n"); return 2;
      default: printf(GRN "Success" RESET ": Running Akscii\n");
    }
  }
  
  printf("Image: %s\n", image);

  FILE *fp, *sfp, *vfp;

  if ((fp = fopen(image, "r")) == NULL) {
		printf(RED "Error" RESET ": Image could not be opened\n");
	} else {
    int c;
    int charbot = 32;
    int charlim = 127;
    int i, j, marker, length, scan;
    i = j = marker = length = scan = 0;
    char type[4] = "non";

    // APP
    char identifier[5] = {0};
    int versionMajor = 0;
    int versionMinor = 0;
    int units = 0;
    int densityX = 0;
    int densityY = 0;
    int thumbnailX = 0;
    int thumbnailY = 0;

    // Variable for Fileout mode
    char line[128] = {0};
    char linenum[4] = {0};
    char lineint[4] = {0};
    char linehex[4] = {0};
    char linechar[4] = {0};

    if (fileout) {
      char shortFile[256] = {0};
      char verboseFile[256] = {0};
      strcpy(shortFile, image);
      strcpy(verboseFile, image);
      char *shortExtension = strrchr(shortFile, '.');
      char *verboseExtension = strrchr(verboseFile, '.');
      strcpy(shortExtension, "_short.txt");
      strcpy(verboseExtension, "_verbose.txt");

      if ((fileoutvalue == 0) || (fileoutvalue == 1)) {
        sfp = fopen(shortFile, "wx");
        if (sfp == NULL) {
          printf(YEL "Warning" RESET ": %s already exists, would you like to overwrite it? (y/n) ", shortFile);
          char input = getchar();
          getchar();
          if (input == 'y') {
            printf("You chose to overwrite the file... continuing\n\n");
            sfp = fopen(shortFile, "w");
          } else {
            printf("You chose not to overwrite the file... continuing\n\n");
            if (fileoutvalue == 0) {
              fileoutvalue = 2;
            } else {
              fileout = 0;
            }
          }
        }
      }
      
      if ((fileoutvalue == 0) || (fileoutvalue == 2)) {
        vfp = fopen(verboseFile, "wx");
        if (vfp == NULL) {
          printf(YEL "Warning" RESET ": %s already exists, would you like to overwrite it? (y/n) ", verboseFile);
          char input = getchar();
          if (input == 'y') {
            printf("You chose to overwrite the file... continuing\n\n");
            vfp = fopen(verboseFile, "w");
          } else {
            printf("You chose not to overwrite the file... exiting\n\n");
            if (fileoutvalue == 0) {
              fileoutvalue = 1;
            } else {
              fileout = 0;
            }
          }
        }
      }
    }

    while ((c = getc(fp)) != EOF) {
      char shortinfo [256] = {0};

      if ((c == FF) && (marker == 0)) {
        marker = 1;
        length = 0;
        strcpy(type, "non");
      } else if ((marker == 1) && (strcmp(type, "non") == 0)) {
        switch ((int) c) {
          case 0: marker = 0; break;
          case SOI: strcpy(type, "SOI"); marker = 0; break;
          case EOI: strcpy(type, "EOI"); marker = 0; break;
          case APP: strcpy(type, "APP"); break;
          case APP1: strcpy(type, "EXF"); break;
          case DQT: strcpy(type, "DQT"); break;
          case SOF: strcpy(type, "SOF"); break;
          case DHT: strcpy(type, "DHT"); break;
          case DRI: strcpy(type, "DRI"); break;
          case SOS: strcpy(type, "SOS"); break;
          case COM: strcpy(type, "COM"); break;
          default: strcpy(type, "UNK"); break;;
        }

        j = 0;
        
        if ((scan != 1) || (strcmp(type, "EOI") == 0)) {
          strcpy(shortinfo, type);
        }
      } else if ((marker == 1) && (strcmp(type, "non") != 0) && (j <= 2) && (scan != 1)) {
        length += (int) c;
        if (j == 2) {
          char lengthstr[16] = {0};
          strcpy(shortinfo, "Length ");
          sprintf(lengthstr, "%d", length);
          strcat(shortinfo, lengthstr);
        }
      } else if ((marker == 1) && (strcmp(type, "non") != 0) && (j > 2) && (j <= length)) {


        if (j == length) {
          marker = 0;
          if (strcmp(type, "SOS") == 0) {
            strcpy(shortinfo, "Image Data Starts");
            scan = 1;
          }
        }
      } else {
        // Do nothing
      }
     
      // Fileout and Testing mode
      if (fileout || testing) {
        char shortline[256] = {0};
        char verboseline[256] = {0};

        char linenum[16] = {0};
        sprintf(linenum, "%d", i);
        char lineint[16] = {0};
        sprintf(lineint, "%d", c);
        char linehex[16] = {0};
        sprintf(linehex, "%X", c);
        char linechar[16] = {0};
        sprintf(linechar, "%c", c);

        if (shortinfo[0] != 0) {
          strcpy(shortline, "Line ");
          strcat(shortline, linenum);
          strcat(shortline, "\t");
          strcat(shortline, shortinfo);
          strcat(shortline, "\n");
          if (testing) {
            printf("%s", shortline);
          }
          if (fileout && ((fileoutvalue == 0) || (fileoutvalue == 1))) {
            fputs(shortline, sfp);
          }
        }

        strcpy(verboseline, "Line ");
        strcat(verboseline, linenum);
        strcat(verboseline, "\t");
        strcat(verboseline, "\tInteger ");
        strcat(verboseline, lineint);
        strcat(verboseline, "\tHexadecimal ");
        strcat(verboseline, linehex);
        strcat(verboseline, "\tCharacter ");
        if ((c > charbot) && (c < charlim)) {
          strcat(verboseline, linechar);
        } else {
          strcat(verboseline, "none");
        }
        strcat(verboseline, "\n");
        if (fileout && ((fileoutvalue == 0) || (fileoutvalue == 2))) {
          fputs(verboseline, vfp);
        }
      }

      i++;
      j++;

    }
	}

	fclose(fp);
  
  if (fileout) {
    if (sfp != NULL) {
      fclose(sfp);
    }
    if (vfp != NULL) {
      fclose(vfp);
    }
  }
	return 0;
}
