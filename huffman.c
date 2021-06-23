#include "huffman.h"



/* #################################################################################################### */
/* ###################################### HEAP RELATED FUNCTIONS ###################################### */
/* #################################################################################################### */

// creates a heap-node from given arguments, allocating its memory
hnode* create_node(byte data, int frequency) {
  hnode* node = (hnode*)malloc(sizeof(hnode));    // generating the node

  node->data = data;    // store the data in its field as an ASCII character
  node->frequency = frequency;    // fill the frequency associated with the data
  node->left = node->right = NULL;

  return node;    // returning the node built
}

// creates a heap-tree of a given capacity, allocating its memory
heap* create_heap(int cap) {
  heap* h = (heap*)malloc(sizeof(heap));   // generating the tree

  h->size=0;   // the heap initially has no nodes
  h->cap=cap;    // setting the tree's capacity
  h->arr=(hnode**)malloc(h->cap*sizeof(hnode*));    // allocates an array of sub-trees

  return h;    // returning the tree built
}

heap* create_build_min_heap(byte* data, int actual_size) {
  int i, j;    // for-loop indexes
  heap* h = create_heap(actual_size);   // creates a min-heap of capacity=cap

  for(i=0, j=0; i<MAX_UCHAR && j<actual_size; i++) {
    if(data[i]!=0) {    // if the element's frequency is not null...
      h->arr[j] = create_node(i, data[i]);    // ...creates a node out of it and inserts it in the heap
      j++;
    }
  }

  h->size = actual_size;   // updating the tree's size
  build_min_heap(h);    // building a min-heap out of the tree

  return h;   // returns the min-heap constructed
}

void build_min_heap(heap* h) {
  int i;    // for-loop index
  int d = h->size-1;

  for(i=(d-1)/2; i>=0; i--) min_heapify(h, i);
}

void min_heapify(heap* h, int index) {
  int min, left, right;
  left=2*index+1;
  right=2*index+2;

  if((left < h->size) && (h->arr[left]->frequency < h->arr[index]->frequency)) min=left;
  else min=index;

  if((right < h->size) && (h->arr[right]->frequency < h->arr[min]->frequency)) min=right;

  if(min != index) {
    hnode* temp = h->arr[min];
    h->arr[min]=h->arr[index];
    h->arr[index]=temp;
    min_heapify(h, min);    // recursive call
  }
}

void insert_node(heap* h, hnode* node) {
  int i = h->size++;

  while(i && node->frequency < h->arr[(i-1)/2]->frequency) {
    h->arr[i] = h->arr[(i-1)/2];
    i = (i-1)/2;
  }

  h->arr[i] = node;
}



/* #################################################################################################### */
/* ###################################### COMPRESSION FUNCTIONS ####################################### */
/* #################################################################################################### */

void compress(byte* buf, int size) {

  byte* data=process_data(buf, size);    // constructs an array out of buffer values' frequencies
  byte num_different_elements=get_size(data);   // gets the number of different elements in the array
  hnode* root=build_huff_tree(data, num_different_elements);    // builds a huffman tree out of the data extracted

  int v[256], m=0;
  print_codes(root, v, m);

  return;
}

byte* process_data(byte* buf, int size) {
  int i;    // for-loop index
  byte* freq = (byte*)malloc(MAX_UCHAR*sizeof(byte));    // allocates a byte array with a cell for each possible byte value
  memset(freq, 0, MAX_UCHAR);   // initializing every element of the array to 0

  for(i=0; i<size; i++) {
    freq[buf[i]]++;
  }

  return freq;    // returns the constructed array, ready to be encoded
}

hnode* build_huff_tree(byte* data, int actual_size) {
  hnode *left, *right, *merge;
  heap* h = create_build_min_heap(data, actual_size);    // builds a min-heap of given capacity

  while(!(h->size == 1)) {    // iterates untill the heap has a single element
    left=get_min(h);    // extracts the less frequent element from the tree
    right=get_min(h);   // extracts the second less frequent element from the tree

    merge=create_node(0, left->frequency + right->frequency);    // as non-leaf nodes aren't associated with data, convention will be 0
    merge->left=left;
    merge->right=right;

    insert_node(h, merge);
  }

  return get_min(h);    // after the loop, the tree is built and the root is the min element
}



/* #################################################################################################### */
/* ########################################  HELPER FUNCTIONS  ######################################## */
/* #################################################################################################### */

static hnode* get_min(heap* h) {
  hnode* temp = h->arr[0];
  h->arr[0] = h->arr[(h->size--) - 1];
  min_heapify(h, 0);

  return temp;
}

static short is_leaf(hnode* n) {
  return ((n->left==NULL) && (n->right==NULL));
}

static byte get_size(byte* buf) {
  int i;    // for-loop index
  byte size=0;    // holds the number of elements with non-zero frequency

  for(i=0; i<MAX_UCHAR; i++) {
    if(buf[i]!=0) size++;   // for every element present, increase size by 1
  }

  return size;    // returns the size of the non-null part of the array
}

static void print_arr(int v[], int n) {
  int i;
  for(i=0; i<n; i++) {
    printf("%d", v[i]);
  }
  printf("\n");
}

static void print_arr_c(byte* v, int size) {
  int i;
  for(i=0; i<size; i++) {
    printf("%d", v[i]);
  }

  return;
}

static void print_codes(hnode* r, int* v, int m) {
  if(r->left) {
    v[m]=0;
    print_codes(r->left, v, m+1);
  }
  if(r->right) {
    v[m]=1;
    print_codes(r->right, v, m+1);
  }

  if(is_leaf(r)) {
    printf("%d: ", r->data);
    print_arr(v, m);
  }
}



/* #################################################################################################### */
/* ############################################ DEBUG MAIN ############################################ */
/* #################################################################################################### */

int main() {
  byte fl[] = {1, 4, 5, 5, 2, 9, 21, 15, 18, 33, 74, 1, 2, 2, 15, 18, 25, 27, 30, 40};
  
  compress(fl, sizeof(fl));

  return 0;
}
