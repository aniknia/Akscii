#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <string.h>

#include "log.h"

#define FF 255
#define SOI 0xD8    //Start of Image
#define EOI 0xD9    //End of Image
#define SOS 0xDA    //Start of Scan
#define COM 0xFE    //Comments

#define SOF 0xC0    //Baseline DCT
#define SOF1 0xC1   //Extended Sequential DCT
#define SOF2 0xC2   //Progressive DCT
#define SOF3 0xC3   //Lossless Sequential
#define SOF4 0xC4   //
#define SOF5 0xC5   //Differential Sequential DCT
#define SOF6 0xC6   //Differential Progressive DCT
#define SOF7 0xC7   //Differential Lossless
#define SOF8 0xC8   //
#define SOF9 0xC9   //Extended Sequential DCT
#define SOF10 0xCA  //Progressive DCT
#define SOF11 0xCB  //Lossless Sequential
#define SOF12 0xCC  //
#define SOF13 0xCD  //Differential Sequential DCT
#define SOF14 0xCE  //Differential Progressive DCT
#define SOF15 0xCF  //Differential Lossless

#define APP 0xE0    //
#define APP1 0xE1   //EXIF
#define APP2 0xE2   //
#define APP3 0xE3   //
#define APP4 0xE4   //
#define APP5 0xE5   //
#define APP6 0xE6   //
#define APP7 0xE7   //
#define APP8 0xE8   //
#define APP9 0xE9   //
#define APP10 0xEA  //
#define APP11 0xEB  //
#define APP12 0xEC  //
#define APP13 0xED  //
#define APP14 0xEE  //
#define APP15 0xEF  //

#define DAC 0xCC    //Define Arithmetic Conditioning Table
#define DHP 0xDE    //Define Hierarchical Progression
#define DHT 0xC4    //Define Huffman Table
#define DNL 0xDC    //Define Number of Lines
#define DQT 0xDB    //Define Quantization Table
#define DRI 0xDD    //Define Restart Interval

// TODO: Should i combine these into a single struct that includes a marker name?
// Maybe thatll make it easier to pass it generically to the log function.
// It might also make the code more generic and hygenic.

struct MARKER {
  char marker[4];
  int length;
  int *data; // use malloc at runtime (maybe 524288 numbers?)

  // APP
  char identifier[5];
  int versionMajor;
  int versionMinor;
  int units;
  int densityX;
  int densityY;
  int thumbnailX;
  int thumbnailY;

};

struct APP_d {
  int length;
  char identifier[5];
  int versionMajor;
  int versionMinor;
  int units;
  int densityX;
  int densityY;
  int thumbnailX;
  int thumbnailY;
};

struct DQT_d {
  int length;
};

struct SOF_d {
  int length;
};

struct DHT_d {
  int length;
};

struct SOS_d {
  int length;
};

// TODO: Check the largest amount of image data that could exist in an image
struct ID_d {
  int length;
  int data[524288]; // pow(2, 19)
};

int decode_JPEG(char image[256]);

#endif