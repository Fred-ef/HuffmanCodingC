/* DESCRIZIONE */

#ifndef huffman_h
#define huffman_h

#include <stdlib.h>
#include <stdio.h>

typedef struct heap_node {   // implementation of a heap sub-tree
  unsigned char data;    // 8 bits of data interpreted as a raw bytes
  unsigned int frequency;   // stores the frequency in which the 8bits appear
  struct heap_node* left;   // link to left child
  struct heap_node* right;  // link to right child
} hnode;

typedef struct heap_tree {    // implementation of a min-heap
  unsigned int dim;    // current dimension (size) of the min-heap
  unsigned int cap;    // maximum dimension (size) of the min-heap
  struct heap_node** arr;   // array of min-heap nodes, each node can hold an heap
} heap;



hnode* create_node(unsigned char, unsigned int);
heap* create_heap(unsigned int);
void swap_nodes(hnode**, hnode**);
void min_heapify(heap*, int);
int has_one(heap*);
hnode* get_min(heap*);
void insert_node(heap*, hnode*);
void build_min_heap(heap*);
void print_arr(int*, int);
int is_leaf(hnode*);
heap* create_build_min_heap(unsigned char*, int*, int);
hnode* build_huff_tree(unsigned char*, int*, int);
void print_codes(hnode*, int*, int);
void alpha_encode(unsigned char*, int*, int);

#endif // huffman_h
