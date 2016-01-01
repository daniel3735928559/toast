#define KHEAP_HEAP_SLAB_SIZE 0x1000
#define MAX_KHEAPS 0x100000
#define KHEAP_INIT_SIZE 0x4000
#define KHEAP_START 0xC1000000

typedef struct block_descriptor{
  void *start;
  block_descriptor_t *next;
  block_descriptor_t *prev;
  slab_descriptor_t *slab;
} block_descriptor_t;

typedef struct slab_descriptor{
  void *start;
  kheap_t *heap;
  u32 ref_count;
  //u32 bitmap[];//for storing the free/allocated blocks
}

typedef struct kheap{
  u32 flags; //0:(1=Small) 1-31:RESERVED
  u32 block_size;
  u32 slab_size;
  u32 max_size;
  u32 alignment;
  void (*constructor)(void *);
  void (*destructor)(void *);
  void *start;
  void *end;
  block_descriptor_t *first_free;
  block_descriptor_t *last_free;
  void *alloc(u32);
  void free(void *);
} kheap_t;

inline u32 get_slab_index(kheap_t *heap, void *block_addr){
  return (block_address - heap->start)/heap->slab_size;
}

inline u32 get_net_block_size(kheap_t *heap){
  return heap->block_size + (heap->align - ((heap->block_size) % heap->align));
}

inline u32 get_block_per_slab(kheap_t *heap){
  return (heap->slab_size)/get_net_block_size(heap);
}

inline u32 get_block_offset_in_slab(kheap_t *heap, void *block_addr){
  return (block_address - (heap_start + (heap->slab_size)*get_slab_index(heap, block_addr)))/get_net_block_size(heap);
}

u32 get_block_index(kheap_t *heap, void block_addr){
  return get_slab_index(heap, block_addr)*(heap->slab_size)/get_net_block_size(heap) + get_block_offset_in_slab(heap, block_addr)/get_net_block_size(heap);
}

void *kheap_get_block(kheap_t *heap, u32 index){
  return (void *)((u32)(heap->start) + index*((u32)(heap->block_size)));
}

/*
Or perhaps we ought have a separate kernel-side heap for the block and slab structures, which will improve security by not requiring any metadata to be stored in the heap itself.
*/

/*
safe_offset refers to an offset within the constructed block in which to place the block_descriptor struct when the block is free (which can be overwritten with the data in heap->ro_data when the block is allocated).  So if there are sizeof(block_descriptor_t) contiguous dwords of constructed state that are the same for all objects, they can be overwritten with block data and then re-written with the stored immutable data at allocation time.  

Set safe_offset to -1 when there is no `safe' place in the constructed object to put ro data, in which case there will be .  */

kheap_t *block_heap;
kheap_t *slab_heap;
kheap_t *kheap_heap;

void allocator_init(){
  kheap_heap = kheap_init(kmalloc_z(sizeof(kheap_t));, sizeof(heap_t), KHEAP_HEAP_SLAB_SIZE, MAX_KHEAPS, KHEAP_INIT_SIZE, zero, zero);
  block_heap = kheap_init(kheap_create(), sizeof(block_descriptor_t), BLOCK_HEAP_SLAB_SIZE, MAX_BLOCKS, BLOCK_HEAP_INIT_SIZE, zero, zero);
  slab_heap = kheap_init(kheap_create(), sizeof(slab_descriptor_t), SLAB_HEAP_SLAB_SIZE, MAX_SLABS, SLAB_HEAP_INIT_SIZE, zero, zero);
}

kheap_t *kheap_init(kheap_t *heap, u32 b_size, u32 s_size, u32 m_size, u32 init_slabs, u32 align, void *constr(), void *destr(), u32 safe_offest, void *location){
  if(b_size < PAGE_SIZE/8){
    heap->flags != 1;
    heap->alloc = kheap_small_alloc(u32);
    b_size += sizeof(block_descriptor_t *);
  }
  else{
    heap->alloc = kheap_alloc(u32);
  }

  heap->block_size = b_size;
  heap->slab_size = s_size;
  heap->max_size = m_size;
  heap->alignment = align;
  heap->constructor = constr;
  heap->destructor = destr;
  heap->start = location;
  heap->first_free = heap->get_block(0);
  heap->last_free = heap->get_block(init_slabs*s_size/block_size);
  kheap_expand(heap, init_slabs);
  return heap;
}

kheap_t *kheap_create(){
  return (kheap_t *)(heap_alloc(kheap_heap));
}

void *kheap_alloc(kheap_t *heap){
  if(heap->first_free == NULL) kheap_expand(heap, 1);//Get block address from freelist, expanding the heap if needed
  if((heap->first_free)->next != NULL) ((heap->first_free)->next)->prev = (heap->first_free)->prev;//Unlink the linked list
  return (heap->first_free)->start;//RETURN!
}

void kheap_free(kheap_t *heap, void *block){
  //Check if the block is actually free !?

  //Adjust linked list data
  block_descriptor_t *freed_block = (block_descriptor_t *)(kheap_alloc(block_heap));
  freed_block->slab = get_slab;
  freed_block->next = NULL;
  freed_block->prev = heap->last_free;
  heap->last_free = freed_block;
  freed_block->start = block;

  //
  freed_block->slab = get_slab(block);
  

  //If slab is entirely free now, do something else to allow for possible reclaiming
}

u32 kheap_get_index(kheap_t *heap, void *block_addr){
  //The actual answer is more complicated than this: If there are alignment constraints or if block_size does not divide slab_size there are issues
  
  return ((u32)block_addr - (u32)(heap->start))/(heap->block_size);
}

u32 kheap_expand(kheap_t *heap, u32 slabs){
  if((u32)(heap->top) - (u32)(heap->start) > m_size*block_size) return -1;
  
  u32 target = (heap->slab_size)*slabs + heap->top;
  while(heap->top <= target){
    ....
    heap->top += 0x1000;
  }
}

void kheap_contract(kheap_t *heap, u32 slabs){
  
}

void kheap_debug_print(kheap_t *heap){
  prints("Hello World!");
}
