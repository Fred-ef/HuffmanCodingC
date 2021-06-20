#include "huffman.h"

// creates a heap-node from given arguments, allocating its memory
hnode* create_node(unsigned char data, unsigned int frequency) {
  hnode* node = (hnode*)malloc(sizeof(hnode));    // generating the node

  node->data = data;    // store the data in its field as an ASCII character
  node->frequency = frequency;    // fill the frequency associated with the data
  node->left = node->right = NULL;

  return node;    // returning the node built
}

// creates a heap-tree of a given capacity, allocating its memory
heap* create_heap(unsigned int cap) {
  heap* h = (heap*)malloc(sizeof(heap));   // generating the tree

  h->dim=0;   // the heap initially has no nodes
  h->cap=cap;    // setting the tree's capacity
  h->arr=(hnode**)malloc(h->cap*sizeof(hnode*));    // allocates an array of sub-trees

  return h;    // returning the tree built
}

// swaps the nodes inverting their pointers
void swap_nodes(hnode** a, hnode** b) {
  hnode* temp = *a;
  *a=*b;
  *b=temp;
}

void min_heapify(heap* h, int index) {
  int min, left, right;
  left=2*index+1;
  right=2*index+2;

  if((left < h->dim) && (h->arr[left]->frequency < h->arr[index]->frequency)) min=left;
  else min=index;

  if((right < h->dim) && (h->arr[right]->frequency < h->arr[min]->frequency)) min=right;

  if(min != index) {
    swap_nodes(&(h->arr[min]), &(h->arr[index]));
    min_heapify(h, min);    // recursive call
  }
}

int has_one(heap* h) {
  return (h->dim == 1);    // returns true if the heap has size 1, false otherwise
}

hnode* get_min(heap* h) {
  hnode* temp = h->arr[0];
  h->arr[0] = h->arr[(h->dim--) - 1];
  min_heapify(h, 0);

  return temp;
}

void insert_node(heap* h, hnode* node) {
  int i = h->dim++;

  while(i && node->frequency < h->arr[(i-1)/2]->frequency) {
    h->arr[i] = h->arr[(i-1)/2];
    i = (i-1)/2;
  }

  h->arr[i] = node;
}

void build_min_heap(heap* h) {
  int i;    // for-loop index
  int d = h->dim-1;

  for(i=(d-1)/2; i>=0; i--) min_heapify(h, i);
}

void print_arr(int v[], int n) {
  int i;
  for(i=0; i<n; i++) {
    printf("%d", v[i]);
  }
  printf("\n");
}

int is_leaf(hnode* n) {
  return ((n->left==NULL) && (n->right==NULL));
}

heap* create_build_min_heap(unsigned char* data, int* frequency, int dim) {
  int i;    // for-loop index
  heap* h = create_heap(dim);   // creates a min-heap of capacity=cap

  for(i=0; i<dim; i++) h->arr[i] = create_node(data[i], frequency[i]);

  h->dim = dim;
  build_min_heap(h);

  return h;
}

hnode* build_huff_tree(unsigned char* data, int* frequency, int dim) {
  hnode *left, *right, *merge;
  heap* h = create_build_min_heap(data, frequency, dim);    // builds a min-heap of given capacitys

  while(!has_one(h)) {    // iterates untill the heap has a single element
    left=get_min(h);    // extracts the less frequent element from the tree
    right=get_min(h);   // extracts the second less frequent element from the tree

    merge=create_node(0, left->frequency + right->frequency);    // as non-leaf nodes aren't associated with data, convention will be 0
    merge->left=left;
    merge->right=right;

    insert_node(h, merge);
  }

  return get_min(h);    // after the loop, the tree is built and the root is the min element
}

void print_codes(hnode* r, int* v, int m) {
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

void alpha_encode(unsigned char* data, int* frequency, int dim) {
  hnode* root=build_huff_tree(data, frequency, dim);
  int v[255], m=0;
  print_codes(root, v, m);
}

int main() {
  unsigned char data[] = {1, 2, 3, 4, 5, 6};
  int frequency[] = {5, 9, 12, 13, 16, 45};

  int size = sizeof(data)/sizeof(data[0]);

  alpha_encode(data, frequency, size);

  return 0;
}
