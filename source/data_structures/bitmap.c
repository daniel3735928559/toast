typedef struct{
  u32 size;
  u32 *bits;
} bitmap_t;

bitmap_t *bitmap_create(u32 entries){
  bitmap_t *b = (bitmap_t *)kmalloc(sizeof(bitmap_t));
  b->size = entries/32;
  b->bits = (u32 *)kmalloc(4*(b->size));
  setm((u32 *)(b->bits), b->size, 0);
  return b;
}

u32 frame_bitmap_entry(u32 frame_address){
  return frame_address/0x1000;
}

void bitmap_set(bitmap_t *bmap, u32 entry){
  *((bmap->bits)+entry/32) |= (0x1 << (entry % 32));
}

u32 bitmap_get_context(bitmap_t *bmap, u32 entry){
  //print_hex_32(entry);prints("!\n");
  //print_hex_32(bmap);prints("!\n");
  //print_hex_32(bmap->bits + entry/32);prints("!\n");
  return *(bmap->bits + entry/32);
}

void bitmap_clear(bitmap_t *bmap, u32 entry){
  *((bmap->bits)+entry/32) &= ~(0x1 << (entry % 32));
}

u8 bitmap_test(bitmap_t *bmap, u32 entry){
  if(entry > (bmap->size)*8)
    return ((*((bmap->bits)+entry/32) & 0x1 << (entry % 32)) >> (entry % 32));
}

u32 get_first_free(bitmap_t *bmap){
  u32 i = 0;
  for(i = 0; i < bmap->size; i++){
    u32 x = *(bmap->bits + i*sizeof(u32));
    if(x != 0xFFFFFFFF){
      return 8*i + get_first_set(~x);
    }
  }
  return -1;
}
