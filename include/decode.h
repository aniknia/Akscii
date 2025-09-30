#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "marker.h"

int decode_JPEG(char image[256]);
int decode_unpackJPEG(FILE *fp, struct MARKER *marker);

#endif