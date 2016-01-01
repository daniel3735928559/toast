typedef struct
{
  bitmap_t *bitmap;
  stack_t *stack;
} imap_t;

imap_t *imap_create(u32 size)
{
  //prints("a");
  imap_t *imap = (imap_t *)kmalloc(sizeof(imap_t));
  imap->bitmap = bitmap_create(size*8);
  imap->stack = stack_create(size);
  u32 i;
    //for(i = 0; i < size; i++){
    //  stack_push(imap->stack, size - 1 - i);
    //}
  for(i = 0; i < size; i++){
    //prints("b");print_hex_32(imap->stack);
    stack_push(imap->stack, size - 1 - i);
  }
  return imap;
}

u32 imap_set_next(imap_t *imap)
{
  //stack_debug_print(imap->stack);
  u32 index = stack_peek(imap->stack);
  //prints("index=");print_hex_32(index);prints("!\n");
  if(index == -1){ return -1; }
  u32 offset = get_first_set(~(bitmap_get_context(imap->bitmap, index)));
  //prints("B");
  u32 entry = index + offset;
  //prints("C");
  bitmap_set(imap->bitmap, entry);
  if(bitmap_get_context(imap->bitmap, entry) == 0xFFFFFFFF){
    stack_pop(imap->stack);
  }
  return entry;
}

void imap_stack_reset(imap_t *imap)
{
  //stack_debug_print(imap->stack);
  imap->stack->top = imap->stack->base - 1;
  u32 i;
  for(i = 0; i < imap->bitmap->size; i++){
    if(*(imap->bitmap->bits + i) != 0xFFFFFFFF){
      stack_push(imap->stack, i);
    }
  }
  //stack_debug_print(imap->stack);
  //prints("hi");
}

void imap_clear(imap_t *imap, u32 entry)
{
  u32 x = bitmap_get_context(imap->bitmap, entry);
  if(x == 0xFFFFFFFF){
    stack_push(imap->stack, entry/32);
  }
  bitmap_clear(imap->bitmap, entry);
}

