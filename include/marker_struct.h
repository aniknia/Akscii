#ifndef MARKER_STRUCT_H
#define MARKER_STRUCT_H

// Marker Codes
enum jpeg_marker {
  // TODO: remove this type and find an alternative
  MARKER_DATA = 0x00,   //DATA (This one i made up)

  MARKER_SOI = 0xD8,    //Start of Image
  MARKER_EOI = 0xD9,    //End of Image
  MARKER_SOS = 0xDA,    //Start of Scan
  MARKER_COM = 0xFE,    //Comments

  MARKER_SOF = 0xC0,    //Baseline DCT
  MARKER_SOF1 = 0xC1,   //Extended Sequential DCT
  MARKER_SOF2 = 0xC2,   //Progressive DCT
  MARKER_SOF3 = 0xC3,   //Lossless Sequential
  MARKER_SOF5 = 0xC5,   //Differential Sequential DCT
  MARKER_SOF6 = 0xC6,   //Differential Progressive DCT
  MARKER_SOF7 = 0xC7,   //Differential Lossless
  MARKER_SOF9 = 0xC9,   //Extended Sequential DCT
  MARKER_SOF10 = 0xCA,  //Progressive DCT
  MARKER_SOF11 = 0xCB,  //Lossless Sequential
  MARKER_SOF13 = 0xCD,  //Differential Sequential DCT
  MARKER_SOF14 = 0xCE,  //Differential Progressive DCT
  MARKER_SOF15 = 0xCF,  //Differential Lossless

  MARKER_APP = 0xE0,    //
  MARKER_APP1 = 0xE1,   //EXIF
  MARKER_APP2 = 0xE2,   //
  MARKER_APP3 = 0xE3,   //
  MARKER_APP4 = 0xE4,   //
  MARKER_APP5 = 0xE5,   //
  MARKER_APP6 = 0xE6,   //
  MARKER_APP7 = 0xE7,   //
  MARKER_APP8 = 0xE8,   //
  MARKER_APP9 = 0xE9,   //
  MARKER_APP10 = 0xEA,  //
  MARKER_APP11 = 0xEB,  //
  MARKER_APP12 = 0xEC,  //
  MARKER_APP13 = 0xED,  //
  MARKER_APP14 = 0xEE,  //
  MARKER_APP15 = 0xEF,  //

  MARKER_DAC = 0xCC,    //Define Arithmetic Conditioning Table
  MARKER_DHP = 0xDE,    //Define Hierarchical Progression
  MARKER_DHT = 0xC4,    //Define Huffman Table
  MARKER_DNL = 0xDC,    //Define Number of Lines
  MARKER_DQT = 0xDB,    //Define Quantization Table
  MARKER_DRI = 0xDD,    //Define Restart Interval
};

// FIXME: change from camel case to snake case as per linux standards
struct marker {
  // Generic
  enum jpeg_marker code;
  int length;
  int *data; // use malloc at runtime (check largest image/frame size)

  union {
    struct {
      char identifier[5];
      int major_version;
      int minor_version;
      int units;
      int density_x;
      int density_y;
      int thumbnail_x;
      int thumbnail_y;
    } app;

    struct {
      int destination;
      int table[8][8];
    } dqt;

    struct {
      int precision;
      int lines;
      int samples;
      int components;
      int *factor_table;
    } sof;

    struct {
      int number_of_tables;
      int *class;
      int *destination;
      int (*number_of_bytes)[16];
      int ***bytes;
    } dht;

    struct {
      int components;
      int *component_selector;
      int *dc_table_selector;
      int *ac_table_selector;
      int spectral_select_start;
      int spectral_select_end;
      int successive_high;
      int successive_low;
    } sos;
  };
};

#endif