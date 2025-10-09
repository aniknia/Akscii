#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "marker.h"

static FILE *aksciiImageFilePointer = NULL;

int decode_JPEG(char image[256]);
void decode_unpackMarker(struct MARKER *marker);
void decode_unpackUKN(struct MARKER *marker);
void decode_unpackAPP(struct MARKER *marker);
void decode_unpackDQT(struct MARKER *marker);
void decode_unpackSOF(struct MARKER *marker);
void decode_unpackDHT(struct MARKER *marker);
void decode_unpackSOS(struct MARKER *marker);

#endif