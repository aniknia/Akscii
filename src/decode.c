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

  int markerStatus, scanStatus = 1;
  int currentChar;
  int line, markerPosition, scanPosition = 0;

  // Array of markers
  struct MARKER marker[1024]; // FIXME: see if i can remove the magic number
  int currentMarker = 0;

  // FIXME: see if i can remove the excess nesting
  while ((currentChar = getc(fp)) != EOF) {
    log_verbose(line, currentChar);

    if ((currentChar == FF) && (!scanStatus)) {
      // Get next identifier
      currentChar = getc(fp);
      log_verbose(++line, currentChar);

      // Identifying markerStatus
      marker[currentMarker].code = currentChar;
      if (marker[currentMarker].code == SOI) {
        log_summary(marker[currentMarker]);
        currentMarker++;
        markerStatus = 0;
        continue;
      } else if (marker[currentMarker].code == EOI) {
        log_summary(marker[currentMarker]);
        break;
      } else {
        // Unpack Marker
        decode_unpackMarker(fp, &marker[currentMarker], &line);
        log_summary(marker[currentMarker]);
      }

      // Checking for Start of Scan
      if (marker[currentMarker++].code == SOS) {
        scanStatus = 1;
        // FIXME: img data doesnt have a fixed size (using magic number)
        marker[currentMarker].data = malloc(64 * 1024 * 1024);
      }
    } else {
      if (currentChar != FF) {
        marker[currentMarker].data[scanPosition++] = currentChar;
      } else {
        int nextChar = getc(fp);
        if (nextChar == 0x00) {
          marker[currentMarker].data[scanPosition++] = FF;
          continue;
        }
        ungetc(nextChar, fp);
        ungetc(currentChar, fp);
        scanStatus = 0;
        scanPosition = 0;
        currentMarker++;
      }
    }
    
    line++;
  }
  
	fclose(fp);

	return 0;
}

int decode_unpackJPEG(FILE *fp, struct MARKER *marker) {
  return 0;
}

void decode_unpackMarker(FILE *fp, struct MARKER *marker, int *line) {
  int currentChar = 0;
  // Grab Marker length
  for (int i = 0; i < 2; i++) {
    currentChar = getc(fp);
    log_verbose(++(*line), currentChar);
    if (i == 0) {
      marker->length += currentChar << 8;
    } else {
      marker->length |= currentChar;
    }
  }
  // Grab Marker Data
  for (int j = 2; j < (marker->length); j++) {
    currentChar = getc(fp);
    log_verbose(++(*line), currentChar);
    // printf("%d\n", j);
  }
}
