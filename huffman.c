#include "huffman.h"



/* #################################################################################################### */
/* ########################################  HELPER FUNCTIONS  ######################################## */
/* #################################################################################################### */

static hnode* heap_get_min(heap* h) {
  hnode* temp = h->arr[0];
  h->arr[0] = h->arr[(h->size--) - 1];
  min_heapify(h, 0);

  return temp;
}


static short is_leaf(hnode* n) {
  return ((n->left==NULL) && (n->right==NULL));
}


static byte get_diff_elems_num(byte* buf) {
  int i;    // for-loop index
  byte size=0;    // holds the number of elements with non-zero frequency

  for(i=0; i<MAX_UCHAR; i++) {
    if(buf[i]!=0) size++;   // for every element present, increase size by 1
  }

  return size;    // returns the size of the non-null part of the array
}


static unsigned int get_compressed_size_bit(abr_node* root) {
  if(root==NULL) return 0;

  return (get_compressed_size_bit(root->left) + ((root->frequency)*(strlen(root->encoding))) + get_compressed_size_bit(root->right));
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
    printf("%c: ", r->data);
    print_arr(v, m);
  }
}


static int int_div_ceil(int a, int b) {
  double c = ((double)a)/((double)b);
  int d = c;

  return (d<c ? (d+1) : d);
}



/* #################################################################################################### */
/* ###################################### HEAP RELATED FUNCTIONS ###################################### */
/* #################################################################################################### */

// creates a heap-node from given arguments, allocating its memory
hnode* heap_create_node(byte data, int frequency) {
  hnode* node = (hnode*)malloc(sizeof(hnode));    // generating the node

  node->data = data;    // store the data in its field as an ASCII character
  node->frequency = frequency;    // fill the frequency associated with the data
  node->left = node->right = NULL;

  return node;    // returning the node built
}


// creates a heap-tree of a given capacity, allocating its memory
heap* heap_create_heap(int cap) {
  heap* h = (heap*)malloc(sizeof(heap));   // generating the tree

  h->size=0;   // the heap initially has no nodes
  h->cap=cap;    // setting the tree's capacity
  h->arr=(hnode**)malloc(h->cap*sizeof(hnode*));    // allocates an array of sub-trees

  return h;    // returning the tree built
}


heap* heap_build_min_heap(byte* data, int actual_size) {
  int i, j;    // for-loop indexes
  heap* h = heap_create_heap(actual_size);   // creates a min-heap of capacity=cap

  for(i=0, j=0; i<MAX_UCHAR && j<actual_size; i++) {
    if(data[i]!=0) {    // if the element's frequency is not null...
      h->arr[j] = heap_create_node(i, data[i]);    // ...creates a node out of it and inserts it in the heap
      j++;
    }
  }

  h->size = actual_size;   // updating the tree's size
  for(i=((h->size-1)-1)/2; i>=0; i--) min_heapify(h, i);

  return h;   // returns the min-heap constructed
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


void heap_insert(heap* h, hnode* node) {
  int i = h->size++;

  while(i && node->frequency < h->arr[(i-1)/2]->frequency) {
    h->arr[i] = h->arr[(i-1)/2];
    i = (i-1)/2;
  }

  h->arr[i] = node;
}



/* #################################################################################################### */
/* ###################################### ABR RELATED FUNCTIONS ####################################### */
/* #################################################################################################### */

abr_node* abr_insert(abr_node* root, byte data, unsigned int frequency, char* encoding) {
  if(root==NULL) {
    abr_node* newnode=(abr_node*)malloc(sizeof(abr_node));
    newnode->data=data;
    newnode->frequency=frequency;
    newnode->encoding=(char*)malloc((strlen(encoding))*sizeof(char));
    strcpy(newnode->encoding, encoding);
    // printf("ABR - data: %c\t encoding: %s\n", newnode->data, newnode->encoding);    // TODO REMOVE
    newnode->left=newnode->right=NULL;
    return newnode;
  }

  if(data<=root->data) root->left=abr_insert(root->left, data, frequency, encoding);
  else root->right=abr_insert(root->right, data, frequency, encoding);
  return root;
}


char* abr_search(abr_node* root, byte data) {
  if(root==NULL) return NULL;

  if(data==root->data) return root->encoding;
  else if(data<root->data) return (abr_search(root->left, data));
  else return (abr_search(root->right, data));
}



/* #################################################################################################### */
/* ###################################### COMPRESSION FUNCTIONS ####################################### */
/* #################################################################################################### */

static byte* get_value_frequency(byte* buf, int size) {
  int i;    // for-loop index
  byte* freq = (byte*)malloc(MAX_UCHAR*sizeof(byte));    // allocates a byte array with a cell for each possible byte value
  memset(freq, 0, MAX_UCHAR);   // initializing every element of the array to 0

  for(i=0; i<size; i++) {
    freq[buf[i]]++;
  }

  return freq;    // returns the constructed array, ready to be encoded
}


huff_comp_file* huff_compress(byte* buf, int size) {

  byte* data=get_value_frequency(buf, size);    // constructs an array out of buffer values' frequencies
  byte num_different_elements=get_diff_elems_num(data);   // gets the number of different elements in the array
  hnode* root=build_huff_tree(data, num_different_elements);    // builds a huffman tree out of the data extracted

  int v[MAX_UCHAR], m=0;
  print_codes(root, v, m);

  return huff_encode(buf, size, root);
}


static void build_encoding_tree(hnode* r, abr_node** root, char* v, int acc, byte* max_code_length) {
  if(r->left) {
    v[acc]='0';
    build_encoding_tree(r->left, root, v, acc+1, max_code_length);
  }
  if(r->right) {
    v[acc]='1';
    build_encoding_tree(r->right, root, v, acc+1, max_code_length);
  }

  if(is_leaf(r)) {
    v[acc]='\0';    // inserts the string terminator
    if(acc>(*max_code_length)) (*max_code_length)=acc;    // updates the max num of bits used for a single element encoding
    (*root) = abr_insert((*root), r->data, r->frequency, v);   // puts the data and its encoding in the abr
  }
}


huff_comp_file* huff_encode(byte* original_data, int size, hnode* tree) {
  int i, j, k=0;    // for-loop indexes
  byte byte_counter=0;    // used to count the number of bits encoded
  byte current_byte=0;    // serves as an 8-bit buffer to write data into the compressed file
  byte remaining_bits=0;    // will hold the number of remaining bits (padding) at the end of the compression
  char* bit_pointer=NULL;    // will hold the temporary codified bit to read
  byte bit_pointer_size=0;    // will hold the size (in bit) of the temporary codified bits to read
  byte max_code_length=0;    // will be used to store the maximum number of bits to encode a single element 
  
  unsigned long int compressed_size_bit;    // will hold the size of the compressed data in bits
  unsigned int compressed_size_byte;    // will hold the size of the compressed data in bytes
  byte* compressed_data=NULL;   // will hold the compressed version of the data

  // will hold the whole structure of the compressed file
  huff_comp_file* compressed_file=(huff_comp_file*)malloc(sizeof(huff_comp_file));

  abr_node* abr_root=NULL;    // ABR used to search encoding by character
  char* codes=(char*)malloc(MAX_UCHAR*sizeof(char));    // used to store code values

  build_encoding_tree(tree, &abr_root, codes, 0, &max_code_length);   // builds the ABR used for encoding
  compressed_size_bit=get_compressed_size_bit(abr_root);    // size(bit) of all elements combined
  compressed_size_byte = int_div_ceil((int)compressed_size_bit, (int)BYTE_LEN);   // size(byte)
  printf("comp data: %lu bits,\t %u bytes\n", compressed_size_bit, compressed_size_byte);    // TODO remove
  compressed_data=(byte*)malloc(compressed_size_byte * sizeof(byte));
  bit_pointer=(char*)malloc(max_code_length*sizeof(char));

  
  for(i=0; i<size; i++) {
    strcpy(bit_pointer, abr_search(abr_root, original_data[i]));
    bit_pointer_size=strlen(bit_pointer);
    printf("%s", bit_pointer);

    for(j=0; j<bit_pointer_size; j++) {
      if(byte_counter==BYTE_LEN) {
        compressed_data[k++]=current_byte;
        current_byte=0;
        byte_counter=0;
      }
      current_byte = (current_byte << 1) | (int)(bit_pointer[j] - '0');
      byte_counter++;
    }

    if((i==size-1) && byte_counter) {   // implementation of the end-file padding
      for(j=0; j<(BYTE_LEN - (compressed_size_bit%BYTE_LEN)); j++) {    // iterates until the last byte is padded (0-filled)
        current_byte = (current_byte << 1) | 0;
        printf("Pad n %d\n", j+1);    // TODO REMOVE
      }
      compressed_data[k]=current_byte;
    }
  }

  for(i=0; i<compressed_size_byte; i++) printf("Byte %d: %d\n", i, compressed_data[i]);

  compressed_file->encoded_data=compressed_data;
  compressed_file->htree=tree;
  compressed_file->bit_size=compressed_size_bit;
  compressed_file->original_byte_size=size;

  return compressed_file;   // returns the file, completely compressed and zero-padded
}


hnode* build_huff_tree(byte* data, int actual_size) {
  hnode *left, *right, *merge;
  heap* h = heap_build_min_heap(data, actual_size);    // builds a min-heap of given capacity

  while(!(h->size == 1)) {    // iterates untill the heap has a single element
    left=heap_get_min(h);    // extracts the less frequent element from the tree
    right=heap_get_min(h);   // extracts the second less frequent element from the tree

    merge=heap_create_node(0, left->frequency + right->frequency);    // as non-leaf nodes aren't associated with data, convention will be 0
    merge->left=left;
    merge->right=right;

    heap_insert(h, merge);
  }

  return heap_get_min(h);    // after the loop, the tree is built and the root is the min element
}



/* #################################################################################################### */
/* ###################################### DECOMPRESSION FUNCTIONS ##################################### */
/* #################################################################################################### */

static void copy_byte(const byte src, byte* dest, byte* position) {
  int i;    // for-loop index
  byte* temp=(byte*)malloc(BYTE_LEN*sizeof(byte));
  byte mod, div=src;    // used to convert value to binary
  memset(temp, 0, BYTE_LEN);    // setting all array's values to 0

  for(i=BYTE_LEN-1; i>=0 && div>0; i--) {
    temp[i]=div%2;
    div=(int)(div/2);
  }

  for(i=0; i<BYTE_LEN; i++, (*position)++) {
    dest[*position]=temp[i];
  }

  free(temp);
}


// returns the value of the next series of bits codified in the huffman tree and advances bit position
static byte get_value_from_code(const byte* buf_src, byte* position, hnode* root) {
  hnode* aux1=NULL;   // auxiliary pointer for traversing the huffman tree
  hnode* aux2=NULL;   // auxiliary pointer for traversing the huffman tree
  byte result=0;    // will hold the final result
  for(aux1=aux2=root; (aux1);) {
    aux2=aux1;
    if(buf_src[(*position)]==0) aux1=aux1->left;
    else aux1=aux1->right;
    printf("Bit_position: %d\n", (*position));
    if(aux1) (*position)++;
  }

  printf("Value: %c\n", aux2->data);

  return (aux2->data);    // last non-null node contains the data associated with the path
}


byte* huff_decompress(huff_comp_file* comp_file) {
  int i;    // for loop index
  byte* comp_data=comp_file->encoded_data;    // retrieving compressed data
  hnode* huff_tree=comp_file->htree;    // retrieving huffman tree
  unsigned long comp_bit_size=comp_file->bit_size;    // retrieving size in bits
  unsigned int comp_byte_size=int_div_ceil((int)comp_bit_size, (int)BYTE_LEN);   // retrieving size in bytes
  unsigned int uncomp_byte_size=comp_file->original_byte_size;    // retrieving uncompressed size in bytes

  printf("comp bit size: %lu \t comp_byte_size: %u\t uncomp_byte_size: %u\n", comp_bit_size, comp_byte_size, uncomp_byte_size);

  byte* uncomp_data=(byte*)malloc(uncomp_byte_size*sizeof(byte));    // allocating space for the file
  byte* buffer_bit=(byte*)malloc(MAX_UCHAR*sizeof(byte));    // acts as a buffer to write bytes into the final file
  byte buffer_bit_start=0;    // keeps track of the current position in the buffer
  byte buffer_bit_end=0;   // keeps track of the last copied bit in the buffer
  byte padding=BYTE_LEN - (comp_bit_size%BYTE_LEN);   // number of 0-padding-bits to pad the last compressed byte (NOT TO DECOMPRESS)
  unsigned int comp_byte_pointer=0;    // keeps track of the number of bytes compressed
  unsigned int uncomp_byte_pointer=0;   // keeps track of the number of bytes written to final file

  // fill the first 256bytes (if the compressed file is large enough)
  while((comp_byte_pointer<(MAX_UCHAR/BYTE_LEN)) && (comp_byte_pointer<comp_byte_size)) {
    copy_byte(comp_data[comp_byte_pointer], buffer_bit, &buffer_bit_end);
    comp_byte_pointer++;
  }

  // get bits from the buffer, de-code them, write them to the output file and re-fill the buffer
  while((uncomp_byte_pointer<=uncomp_byte_size)) {   // repeat until the output file has been built
    uncomp_data[uncomp_byte_pointer++]=get_value_from_code(buffer_bit, &buffer_bit_start, huff_tree);
    while(((buffer_bit_start-buffer_bit_end)%MAX_UCHAR)>8 && (comp_byte_pointer<=comp_byte_size)) {
      copy_byte(comp_data[comp_byte_pointer], buffer_bit, &buffer_bit_end);
      comp_byte_pointer++;
      printf("\nREFILL!\nBuffer ends at bit: %d\n\n", buffer_bit_end);
    }
    printf("Comp point: %d\t Comp size: %d\n\n", comp_byte_pointer, comp_byte_size);
  }

  return uncomp_data;
}



/* #################################################################################################### */
/* ############################################ DEBUG MAIN ############################################ */
/* #################################################################################################### */

int main() {
  int err;
  int i;
  int j;
  int fd;
  int size;
  struct stat* info=(struct stat*)malloc(sizeof(struct stat));

  if((fd=open("file1.txt", O_RDONLY))==-1) {
    perror("Opening file4.txt: ");
    exit(EXIT_FAILURE);
  }

  if((fstat(fd, info))==-1) {
    perror("Getting file4.txt info: ");
    exit(EXIT_FAILURE);
  }

  size = info->st_size;
  printf("Dimensione file originale: %d\n", size);

  byte* buf=(byte*)malloc(size*sizeof(byte));
  huff_comp_file* compressed_file=NULL;


  for(i=0; i<size; i++) {
    err=read(fd, (buf+i), 1);
    // printf("bytes read in cycle %d: %d\n", i, err);
  }

  compressed_file=huff_compress(buf, size);

  byte* decomp_file=huff_decompress(compressed_file);

  printf("De-compressed file size: %lu\n", sizeof(decomp_file));

  if((fd=open("decompressed.txt", O_WRONLY | O_CREAT, 0777))==-1) {    // creating file
      perror("restoring file: ");
      exit(EXIT_FAILURE);
  }

  if((err=write(fd, decomp_file, size))==-1) {    // writing file
      error("writing decompressed file: ");
      exit(EXIT_FAILURE);
  }

  printf("Closing file...\n");

  if((close(fd))==-1) {
    perror("Closing file4.txt: ");
    exit(EXIT_FAILURE);
  }

  return 0;
}