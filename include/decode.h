#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "marker.h"

int decode_JPEG(char image[256]);
void decode_unpackMarker(FILE *fp, struct MARKER *marker, int *line);
void decode_unpackUKN(FILE *fp, struct MARKER *marker, int *line);
void decode_unpackAPP(FILE *fp, struct MARKER *marker, int *line);
void decode_unpackDQT(FILE *fp, struct MARKER *marker, int *line);
void decode_unpackSOF(FILE *fp, struct MARKER *marker, int *line);
void decode_unpackDHT(FILE *fp, struct MARKER *marker, int *line);
void decode_unpackSOS(FILE *fp, struct MARKER *marker, int *line);

#endif