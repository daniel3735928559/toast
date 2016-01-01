#define INIT_HEAP_SIZE 0x10000
#define INDEX_SIZE 0x200
#define HEAP_FREE 0xF2EEF233
#define HEAP_OCCUPIED 0x0CC1A1ED
#define HEADER_SIZE sizeof(block_header_t) + sizeof(block_footer_t)

typedef struct
{
  block_header_t *header;
  u32 magic;
} block_footer_t;

typedef struct
{
  u32 *start;
  u32 size;
  bin_tree_t *index;
} heap_t;

heap_t *kheap;

block_footer_t *get_block_footer(block_header_t *h)
{
  return (block_footer_t *)((void *)h + h->length + sizeof(block_header_t));
}

void put_block_info(heap_t *heap, block_header_t *h, u32 size, node_t *node, u32 magic)
{
  h->length = size;
  h->magic = magic;
  h->entry = node;
  block_footer_t *f = get_block_footer(h);
  f->magic = magic;
  f->header = h;
}

heap_t *heap_create(u32 init_size)
{
  heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));
  heap->size = init_size;
  heap->start = (u32 *)kmalloc_a(init_size + HEADER_SIZE);
  heap->index = bin_tree_create(INDEX_SIZE, init_size, heap->start);
  block_header_t *b = (block_header_t *)(heap->start);
  put_block_info(heap, b, init_size, heap->index->root, HEAP_FREE);
  return heap;
}

void *heap_alloc(heap_t *heap, u32 s)
{
  u32 size = s + HEADER_SIZE;

  node_t *old_hole = bin_tree_lookup(heap->index, size, 1);
  bin_tree_remove(heap->index, old_hole);
	
  block_header_t *annexee = old_hole->header;	
  u32 hole_size = annexee->length;
  
  put_block_info(heap, annexee, s, NULL, HEAP_OCCUPIED);
  
  if(size < hole_size){
    block_header_t *new_hole = (block_header_t *)((u8 *)annexee + size);
    u32 new_hole_size = hole_size - size - HEADER_SIZE;
    node_t *n = NULL;
    n = bin_tree_insert(heap->index, new_hole_size, new_hole);
    put_block_info(heap, new_hole, new_hole_size, n, HEAP_FREE);
  }
  return (void *)annexee + sizeof(block_header_t);;
}

void heap_free(heap_t *heap, void *start)
{
  block_header_t *free_header = (block_header_t *)(start - sizeof(block_header_t));
  block_footer_t *free_footer = get_block_footer(free_header);
  put_block_info(heap, free_header, free_header->length, free_header->entry, HEAP_FREE);
  
  block_header_t *prev = ((block_footer_t *)((void *)free_header - sizeof(block_footer_t)))->header;
  block_header_t *next = (block_header_t *)((void *)free_footer + sizeof(block_header_t));
  
  u32 free_length = free_header->length;
  if(next->magic == HEAP_FREE){
    bin_tree_remove(heap->index, next->entry);
    free_length += next->length + HEADER_SIZE;
  }
  if(prev->magic == HEAP_FREE){
    bin_tree_remove(heap->index, prev->entry);
    free_length += prev->length + HEADER_SIZE;
    free_header = prev;
  }
  put_block_info(heap, free_header, free_length, bin_tree_insert(heap->index, free_length, free_header), HEAP_FREE);
}

/*void block_print(block_header_t *b)
{
	block_footer_t *f = get_block_footer(b);
	printf("Header @ %d; Head Magic = %d; Length = %d; Entry @ %d; Footer @ %d; Foot Magic = %d; Footer Header @ %d\n", b, b->magic, b->length, b->entry, f, f->magic, f->header);
}

void heap_debug_print(heap_t *heap)
{
	printf("\n==============================BEGIN HEAP=================================\n");
	printf("======\n|HEAP|\n======\n");
	printf("Start: %d, Size: %d, Index @ %d\n", heap->start, heap->size, heap->index);
	u8 *h = (u8 *)(heap->start);
	while(h < (u8 *)((u8 *)heap->start + heap->size))
	{
		block_header_t *b = (block_header_t *)h;
		block_print(b);
		h += HEADER_SIZE + b->length;
	}

	printf("\nINDEX: \n");
	bin_tree_print(heap->index);
	printf("\n==============================END HEAP=================================\n\n");
}*/
