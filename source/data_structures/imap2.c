typedef struct
{
  bitmap_t *bitmap;
  stack_t *stack;
  u32 max_used;
  u32 entries;
} imap_t;

imap_t *imap_create(u32 size)
{
  imap_t *imap = (imap_t *)kmalloc(sizeof(imap_t));
  imap->bitmap = bitmap_create(size*32);
  imap->stack = stack_create(size);
  imap->max_used = -1;
  imap->entries = size*32;
  return imap;
}

u32 imap_set_next(imap_t *imap)
{
  u32 index = stack_peek(imap->stack);
  if(index == -1){
    if(imap->max_used < entries){
      index = imap->max_used + 1;
      imap->max_used = index;
      bitmap_set(imap->bitmap, index);
    }
    else return -1;
  }
  else{
    index *= 32;  
    u32 offset = get_first_set(~(bitmap_get_context(imap->bitmap, index)));
    u32 entry = index + offset;
    bitmap_set(imap->bitmap, entry);
    if(bitmap_get_context(imap->bitmap, entry) == 0xFFFFFFFF){
      stack_pop(imap->stack);
    }
    return entry;
  }
}

void imap_stack_reset(imap_t *imap)
{
  imap->stack->top = imap->stack->base - 1;
  u32 i;
  for(i = 0; i < imap->max_used; i++){
    if(*(imap->bitmap->bits + i) != 0xFFFFFFFF)
      stack_push(imap->stack, i);
  }
}

void imap_clear(imap_t *imap, u32 entry)
{
  if(entry == imap->max_used) imap->max_used -= 1;
  else if(bitmap_get_context(imap->bitmap, entry) == 0xFFFFFFFF) stack_push(imap->stack, entry/32);
  bitmap_clear(imap->bitmap, entry);
}
