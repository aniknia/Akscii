#include "decode.h"

int decode_JPEG(char image[256]) {
  FILE *fp;

  if ((fp = fopen(image, "rb")) == NULL) {
		log_status(1, "Image could not be opened");
	} else {
    char msg[1024];
    snprintf(msg, sizeof(msg), "%s opened", image);
    log_status(0, msg);
  }

  int currentChar;
  int line, markerStatus, markerPosition, scanStatus, scanPosition;
  line = markerStatus = markerPosition = scanStatus, scanPosition = 0;

  // Array or markers
  struct MARKER marker[1024];
  int currentMarker = 0;

  while ((currentChar = getc(fp)) != EOF) {
    log_verbose(line, currentChar);

    if ((currentChar == FF) && (markerStatus == 0) && (scanStatus == 0)) {
      // Not in a Marker
      // Not Scanning image data
      markerStatus = 1;
      markerPosition = 0;

      // Get next identifier
      currentChar = getc(fp);
      line++;
      log_verbose(++line, currentChar);

      // Identifying markerStatus
      marker[currentMarker].code = currentChar;
      if (currentChar == SOI) {
        log_summary(marker[currentMarker]);
        currentMarker++;
        markerStatus = 0;
      } else if (currentChar == EOI) {
        log_summary(marker[currentMarker]);
        break;
      }
    } else if (markerStatus == 1) {
      // log_status (2, "in marker status loop");
      if (markerPosition < 2) {
        // Get Length
        if (markerPosition == 0) {
          marker[currentMarker].length += currentChar << 8;
        } else if (markerPosition == 1) {
          marker[currentMarker].length |= currentChar;
        }
      } else if ((markerPosition >= 2) && (markerPosition < marker[currentMarker].length - 1)) {
        // TODO: Grab Marker Data


      } else if (markerPosition == (marker[currentMarker].length - 1)) {
        log_summary(marker[currentMarker]);
        markerStatus = 0;
        if (marker[currentMarker++].code == SOS) {
          scanStatus = 1;
          // FIXME: img data doesnt have a fixed size
          marker[currentMarker].data = malloc(64 * 1024 * 1024);
        }
      } else {
        log_status(0, "error in marker data occured");
      }
      markerPosition++;
    } else if (scanStatus == 1) {
      if (currentChar != FF) {
        marker[currentMarker].data[scanPosition++] = currentChar;
      } else {
        int nextChar = getc(fp);
        if (nextChar == 0x00) {
          marker[currentMarker].data[scanPosition++] = FF;
        } else {
          ungetc(nextChar, fp);
          ungetc(currentChar, fp);
          scanStatus = 0;
          scanPosition = 0;
          currentMarker++;
        }
      }
    } else {
      log_status(1, "error occured");
    }
    
    line++;
  }
  
	fclose(fp);

	return 0;
}

int decode_unpackJPEG(FILE *fp, struct MARKER *marker) {
  return 0;
}
