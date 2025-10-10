#ifndef MARKER_H
#define MARKER_H

#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "marker_struct.h"

void marker_free(struct marker *marker);
struct marker* marker_unpack_image(FILE *fp);

#endif