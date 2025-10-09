#include "decode.h"

int decode_JPEG(char image[256]) {

  if ((aksciiImageFilePointer = fopen(image, "rb")) == NULL) {
		log_status(1, "Image could not be opened");
    return 1;
	} else {
    char msg[1024];
    snprintf(msg, sizeof(msg), "%s opened", image);
    log_status(0, msg);
  }

  int scanStatus = 0;
  int currentChar;
  int line, markerPosition, scanPosition = 0;

  // Array of markers
  struct MARKER marker[1024]; // FIXME: see if i can remove the magic number
  int currentMarker = 0;

  // FIXME: see if i can remove the excess nesting
  while ((currentChar = getc(aksciiImageFilePointer)) != EOF) {
    log_verbose(currentChar);

    if ((currentChar == FF) && (!scanStatus)) {
      // Get next identifier
      currentChar = getc(aksciiImageFilePointer);
      log_verbose(currentChar);

      // Identifying markerStatus
      marker[currentMarker].code = currentChar;
      if (marker[currentMarker].code == SOI) {
        log_summary(marker[currentMarker]);
        currentMarker++;
        continue;
      }
      if (marker[currentMarker].code == EOI) {
        log_summary(marker[currentMarker]);
        break;
      }
      
      // Unpack Marker
      decode_unpackMarker(&marker[currentMarker]);
      log_summary(marker[currentMarker]);

      // Checking for Start of Scan
      if (marker[currentMarker++].code == SOS) {
        scanStatus = 1;
        marker[currentMarker].code = DATA;
        // FIXME: img data doesnt have a fixed size (using magic number)
        marker[currentMarker].data = malloc(64 * 1024 * 1024);
      }
    } else {
      if (currentChar == FF) {
        int nextChar = getc(aksciiImageFilePointer);
        log_verbose(nextChar);
        if (nextChar == 0x00) {
          marker[currentMarker].data[marker[currentMarker].length++] = FF;
          continue;
        }
        ungetc(nextChar, aksciiImageFilePointer);
        ungetc(currentChar, aksciiImageFilePointer);
        scanStatus = 0;
        log_summary(marker[currentMarker]);
        currentMarker++;
        continue;
      }
      marker[currentMarker].data[marker[currentMarker].length++] = currentChar;
    }
    
    line++;
  }
  
	fclose(aksciiImageFilePointer);

	return 0;
}

void decode_unpackMarker(struct MARKER *marker) {
  int currentChar = 0;

  int lenHigh;
  int lenLow;

  // Grab Marker length
  for (int i = 0; i < 2; i++) {
    currentChar = getc(aksciiImageFilePointer);
    log_verbose(currentChar);
    if (i == 0) {
      lenHigh = currentChar;
      marker->length = currentChar << 8;
    } else {
      lenLow = currentChar;
      marker->length |= currentChar;
    }
  }

  marker->data = malloc(sizeof(int) * (marker->length + 2));
  marker->data[0] = FF;
  marker->data[1] = marker->code;
  marker->data[2] = lenHigh;
  marker->data[3] = lenLow;

  // Grab Marker Data
  switch (marker->code) {
    case APP: decode_unpackAPP(marker); break;
    case DQT: decode_unpackDQT(marker); break;
    case SOF: decode_unpackSOF(marker); break;
    case DHT: decode_unpackDHT(marker); break;
    case SOS: decode_unpackSOS(marker); break;
    default: {
      log_status(1, "this marker hasnt been implemented yet");
      decode_unpackUKN(marker);
    }
  }

}

// TODO: Try to consolidate these calls, maybe look at dispatch tables?
void decode_unpackUKN(struct MARKER *marker) {
  int currentChar = 0;
  int length = marker->length - 2;
  for (int i = 0; i < length; i++) {
    currentChar = getc(aksciiImageFilePointer);
    log_verbose(currentChar);
    marker->data[i + 4] = currentChar;
  }
}

void decode_unpackAPP(struct MARKER *marker) {
  int currentChar = 0;
  int length = marker->length - 2;
  for (int i = 0; i < length; i++) {
    currentChar = getc(aksciiImageFilePointer);
    log_verbose(currentChar);
    marker->data[i + 4] = currentChar;
    // I know this could be a list but i want the data to be more explicit
    // APP length should always be 16
    switch (i) {
      case 6: marker->majorVerion = currentChar; break;
      case 7: marker->minorVersion = currentChar; break;
      case 8: marker->units = currentChar; break;
      case 9: marker->densityX = currentChar << 8; break;
      case 10: marker->densityX |= currentChar; break;
      case 11: marker->densityY = currentChar << 8; break;
      case 12: marker->densityY |= currentChar; break;
      case 13: marker->thumbnailX = currentChar; break;
      case 14: marker->thumbnailY = currentChar; break;
      default: marker->identifier[i] = currentChar;
    }
  }
}

void decode_unpackDQT(struct MARKER *marker) {
  int currentChar = 0;
  int length = marker->length - 2;

  currentChar = getc(aksciiImageFilePointer);
  log_verbose(currentChar);
  marker->destination = currentChar;
  marker->data[4] = currentChar;
  length--;

  // FIXME: Make sure DQT is always 8x8
  // From Page 119 this can be 8 or 16 bits
  // For now this is fine
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      currentChar = getc(aksciiImageFilePointer);
      log_verbose(currentChar);
      marker->table[i][j] = currentChar;
      length--;
    }
  }

  if (length != 0) log_status(0, "deocode_unpackDQT ran into an error");
}

void decode_unpackSOF(struct MARKER *marker) {
  int currentChar = 0;
  int length = marker->length - 2;

  // This first part should always be a length of 8
  for (int i = 0; i < length; i++) {
    currentChar = getc(aksciiImageFilePointer);
    log_verbose(currentChar);
    switch (i) {
      case 0: marker->precision = currentChar; break;
      case 1: marker->lines = currentChar << 8; break;
      case 2: marker->lines |= currentChar; break;
      case 3: marker->samples = currentChar << 8; break;
      case 4: marker->samples |= currentChar; break;
      case 5: {
        marker->components = currentChar;
        marker->factorTable = malloc(sizeof(int) * marker->components * 3);
        break;
      }
      default: {
        // FIXME: find a different way to do this
        // Factor Table ID: 8 bits
        // Horizontal Scaling: 4 bits
        // Vertical Scaling: 4 bits
        // Quant Table Selector: 8 bits
        // Hori and Vert Scaling share a byte
        marker->factorTable[i - 6] = currentChar;
      }
    }
  }
}

void decode_unpackDHT(struct MARKER *marker) {
  int currentChar = 0;
  int length = marker->length - 2;

  currentChar = getc(aksciiImageFilePointer);
  log_verbose(currentChar);
  marker->class = currentChar >> 4; // (currentChar >> 4) & 0x0F 
  marker->destination = currentChar & 0x0F;
  length--;

  for (int i = 0; i < (sizeof(marker->bytes)/sizeof(marker->bytes[0])); i++) {
    currentChar = getc(aksciiImageFilePointer);
    log_verbose(currentChar);
    marker->bytes[i] = malloc(sizeof(int) * currentChar);
    marker->numOfBytes[i] = currentChar;
    length--;
  }

  for (int i = 0; i < (sizeof(marker->bytes)/sizeof(marker->bytes[0])); i++) {
    if(marker->numOfBytes[i] == 0) continue;

    for (int j = 0; j < marker->numOfBytes[i]; j++) {
      currentChar = getc(aksciiImageFilePointer);
      log_verbose(currentChar);
      marker->bytes[i][j] = currentChar;
      length--;
    }
  }

  if (length != 0) log_status(0, "deocode_unpackDHT ran into an error");
}

void decode_unpackSOS(struct MARKER *marker) {
  int currentChar = 0;
  int length = marker->length - 2;

  currentChar = getc(aksciiImageFilePointer);
  log_verbose(currentChar);
  marker->components = currentChar;
  marker->componentSelector = malloc(sizeof(int) * marker->components);
  marker->dcTableSelector = malloc(sizeof(int) * marker->components);
  marker->acTableSelector = malloc(sizeof(int) * marker->components);
  length--;

  for (int i = 0; i < marker->components; i++) {
    currentChar = getc(aksciiImageFilePointer);
    log_verbose(currentChar);
    marker->componentSelector[i] = currentChar;

    currentChar = getc(aksciiImageFilePointer);
    log_verbose(currentChar);
    marker->dcTableSelector[i] = currentChar >> 4;
    marker->acTableSelector[i] = currentChar & 0x0F;

    length -= 2;
  }

  currentChar = getc(aksciiImageFilePointer);
  log_verbose(currentChar);
  marker->spectralSelectStart = currentChar;
  length--;

  currentChar = getc(aksciiImageFilePointer);
  log_verbose(currentChar);
  marker->spectralSelectEnd = currentChar;
  length--;

  currentChar = getc(aksciiImageFilePointer);
  log_verbose(currentChar);
  marker->successiveHigh = currentChar >> 4;
  marker->successiveLow = currentChar & 0x0F;
  length--;

  if (length != 0) log_status(0, "deocode_unpackSOS ran into an error");
}