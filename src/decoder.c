#include "decoder.h"

int decodeJPEG(char image[256]) {
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

    // APP (Application)
    char identifier[5] = {0};
    int versionMajor = 0;
    int versionMinor = 0;
    int units = 0;
    int densityX = 0;
    int densityY = 0;
    int thumbnailX = 0;
    int thumbnailY = 0;

    // DQT (Quantization Table)
    
    // SOF (Start of Frame)

    // DHT (Huffman Table)

    // Image Data

    while ((c = getc(fp)) != EOF) {
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
        }
      } else if ((marker == 1) && (strcmp(type, "non") != 0) && (j <= 2) && (scan != 1)) {
        length += (int) c;
        if (j == 2) {
          char lengthstr[16] = {0};
        }
      } else if ((marker == 1) && (strcmp(type, "non") != 0) && (j > 2) && (j <= length)) {


        if (j == length) {
          marker = 0;
          if (strcmp(type, "SOS") == 0) {
            scan = 1;
          }
        }
      } else {
        // Do nothing
      }
    }
  }
  
	fclose(fp);

	return 0;
}
