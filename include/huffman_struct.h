#ifndef HUFFMAN_STRUCT_H
#define HUFFMAN_STRUCT_H

struct huffman_entry {
  unsigned char length;
  unsigned int code;
  unsigned char character;
};

struct huffman_table {
  int length;
  unsigned char table_class;
  unsigned char table_destination;
  unsigned char number_of_bytes[16];
  struct huffman_entry entry[256];
};

#endif