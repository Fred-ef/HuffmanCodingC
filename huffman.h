/* DESCRIZIONE */

#ifndef huffman_h
#define huffman_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_UCHAR 256   // defines the maximum value for the byte (unsigned char) data type

typedef unsigned char byte;   // type of data use to represent raw bytes



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



/* HEAP-RELATED FUNCTIONS */

hnode* create_node(byte, int);
heap* create_heap(int);
void min_heapify(heap*, int);
void insert_node(heap*, hnode*);
void build_min_heap(heap*);
heap* create_build_min_heap(byte*, int);



/* COMPRESSION-RELATED FUNCTIONS */

void compress(byte*, int);
hnode* build_huff_tree(byte*, int);
byte* process_data(byte*, int);

#endif // huffman_h
