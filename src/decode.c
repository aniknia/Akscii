#include "decode.h"

int decode_JPEG(char image[256]) {
  FILE *fp;

  if ((fp = fopen(image, "r")) == NULL) {
		log_status(1, "Image could not be opened");
	} else {
    char msg[128];
    strcpy(msg, image);
    strcat(msg, " opened");
    log_status(0, msg);
  }
  

  int currentChar;
  int charbot = 32;
  int charlim = 127;
  int line, i, marker, length, scan;
  line = i = marker = length = scan = 0;
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

  while ((currentChar = getc(fp)) != EOF) {
    if ((currentChar == FF) && (marker == 0)) {
      marker = 1;
      length = 0;
      strcpy(type, "non");
    } else if ((marker == 1) && (strcmp(type, "non") == 0)) {
      switch ((int) currentChar) {
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

      i = 0;
        
      if ((scan != 1) || (strcmp(type, "EOI") == 0)) {
      }
    } else if ((marker == 1) && (strcmp(type, "non") != 0) && (i <= 2) && (scan != 1)) {
      length += (int) currentChar;
      if (i == 2) {
        char lengthstr[16] = {0};
      }
    } else if ((marker == 1) && (strcmp(type, "non") != 0) && (i > 2) && (i <= length)) {


      if (i == length) {
        marker = 0;
        if (strcmp(type, "SOS") == 0) {
          scan = 1;
        }
      }
    } else {
      // Do nothing
    }

    if (get_verbose()) {
      log_verbose(line, currentChar);
    }
    line++;
  }
  
	fclose(fp);

	return 0;
}
