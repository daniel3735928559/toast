
#define KS_HEAP_BLOCK_SIZE 4096
#define KS_HEAP_SMALL_BLOCK_SIZE 128 //4096/32
#define KS_HEAP_SIZE 0x2000000

typedef struct
{
  u32 start;
  imap_t *ks_imap;
  stack_t *smalls;
} ks_heap_t;

ks_heap_t *ks_heap;

void *ks_heap_alloc_small_block(ks_heap_t *heap){
  u32 x = stack_pop(heap->smalls);
  u32 p;
  if(x == -1){
    p = (u32)(ks_heap_alloc_block(heap));
    stack_push(heap->smalls, 1);
    stack_push(heap->smalls, p);
  }
  else{
    u32 small_bmp = stack_pop(heap->smalls);
    u32 off = get_first_bit_set(~small_bmp);
    small_bmp |= (1 << off);
    if(small_bmp != 0xFFFFFFFF){
      stack_push(heap->smalls, small_bmp);
      stack_push(heap->smalls, x);
    }
    p = x + KS_HEAP_SMALL_BLOCK_SIZE*off;
  }
  return p;
}

void *ks_heap_alloc_block(ks_heap_t *heap){
  u32 idx = imap_set_next(heap->ks_imap);
  u32 address = idx * KS_HEAP_BLOCK_SIZE + heap->start;
  get_a_frame(get_page(address, 1, kernel_dir), 1, 0);
  return (void *)(address);
}

void *ks_heap_free_block(ks_heap_t *heap, u32 block_address){
  u32 idx = (block_address - heap->start)/KS_HEAP_BLOCK_SIZE;
  imap_clear(heap->ks_imap, idx);
}

void *ks_heap_free_small_block(ks_heap_t *heap, u32 block_address){
  u32 idx = (block_address - heap->start)/KS_HEAP_BLOCK_SIZE;
  
}

void ks_heap_init(){
  ks_heap = (ks_heap_t *)(kmalloc(sizeof(ks_heap_t)));
  ks_heap->start = 0xC2000000;
  ks_heap->ks_imap = imap_create(KS_HEAP_SIZE / KS_HEAP_BLOCK_SIZE);
  ks_heap->smalls = stack_create((KS_HEAP_SIZE / KS_HEAP_BLOCK_SIZE)*2+1);
  stack_push(smalls, -1);
}
