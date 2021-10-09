/* DESCRIZIONE */

#ifndef huffman_h
#define huffman_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>

#define MAX_UCHAR 256   // defines the maximum value for the byte (unsigned char) data type
#define BYTE_LEN 8    // defines the size (in bits) of a byte variable (8 bits)

typedef unsigned char byte;   // type of data use to represent raw bytes



/* ##################################################################### */
/* #################### DATA STRUCTURES DEFINITIONS #################### */
/* ##################################################################### */

/* HEAP-RELATED DATA STRUCTURES */

typedef struct heap_node {   // implementation of a heap sub-tree
  byte data;    // 8 bits of data interpreted as a raw bytes
  unsigned int frequency;   // stores the frequency in which the 8bits appear
  struct heap_node* left;   // link to left child
  struct heap_node* right;  // link to right child
} hnode;

typedef struct heap_tree {    // implementation of a min-heap
  unsigned int size;    // current size of the min-heap
  unsigned int cap;    // maximum size of the min-heap
  struct heap_node** arr;   // array of min-heap nodes, each node can hold an heap
} heap;



/* ABR-RELATED DATA STRUCTURES */

typedef struct h_abr_node {
  byte data;
  unsigned int frequency;
  char* encoding;
  struct h_abr_node *left, *right;
} abr_node;



/* COMPRESSED-FILE DATA STRUCTURE */

typedef struct huff_compressed_file {
  byte* encoded_data;     // contains the whole compressed file
  hnode* htree;      // contains the huffman tree associated with the compressed file
  unsigned long bit_size;      // size of the compressed file in bits TODO remove e recupera il dato dall'htree
  unsigned long original_byte_size;     // size of the uncompressed file in bytes TODO remove e recupera il dato dall'htree
} huff_comp_file;



/* ##################################################################### */
/* ####################### FUNCTIONS DEFINITIONS ####################### */
/* ##################################################################### */

/* HEAP-RELATED FUNCTIONS */

hnode* heap_create_node(byte, int);
heap* heap_create_heap(int);
void min_heapify(heap*, int);
void heap_insert(heap*, hnode*);
heap* heap_build_min_heap(byte*, int);



/* ABR-RELATED FUNCTIONS */

abr_node* abr_insert(abr_node*, byte, unsigned int, char*);
char* abr_search(abr_node*, byte);



/* COMPRESSION-RELATED FUNCTIONS */

huff_comp_file* huff_compress(byte*, int);
huff_comp_file* huff_encode(byte*, int, hnode*);
hnode* build_huff_tree(byte*, int);
byte* huff_decompress(huff_comp_file*);

#endif // huffman_h