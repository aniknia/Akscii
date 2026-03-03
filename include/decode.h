#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "marker_struct.h"
#include "marker.h"
#include "quantization_struct.h"
#include "quantization.h"
#include "huffman_struct.h"
#include "huffman.h"

int decode_JPEG(char image[256]);

#endif