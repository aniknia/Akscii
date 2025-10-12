#ifndef MARKER_H
#define MARKER_H

#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "marker_struct.h"

#define HUFFMAN_CODE_LENGTH 16

void marker_free(struct marker *marker, int marker_count);
struct marker* marker_unpack_image(FILE *fp, int *marker_count);

#endif