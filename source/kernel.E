# 1 "kernel.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "kernel.c"
# 1 "stuff.h" 1


typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef char s8;

typedef struct registers
{
 u32 ds;
 u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
 u32 int_no, err_code;
 u32 eip, cs, eflags, useresp, ss;
} registers_t;


void outb(u16 port, u8 value)
{
 asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

u8 inb(u16 port)
{
 u8 ret;
 asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
 return ret;
}

u16 inw(u16 port)
{
 u16 ret;
 asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
 return ret;
}

u32 phys_offset = (0xC1000000 - 0x100000);
u32 EOK = 0xC1000000;

u32 kmalloc(u32 size)
{
 u32 ret = EOK;
 EOK += size;
 return ret;
}

u32 kmalloc_a(u32 size)
{
 if(EOK & 0xfffff000)
  EOK = (EOK & 0xfffff000) + 0x1000;

 u32 ret = EOK;
 EOK += size;
 return ret;
}

u32 kmalloc_ap(u32 size, u32 *phys)
{
 if(EOK & 0xfffff000)
   EOK = (EOK & 0xfffff000) + 0x1000;

 *phys = EOK - phys_offset;
 u32 ret = EOK;
 EOK += size;
 return ret;
}


void *zero(void *a){ *((u32 *)(a)) = 0; }
# 2 "kernel.c" 2
# 1 "datastr.c" 1
# 1 "data_structures/util.c" 1
# 16 "data_structures/util.c"
u8 get_first_set_alt(u32 v)
{
 if(v == 0){ return -1; }
 u8 MultiplyDeBruijnBitPosition[32] =
 {
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
 };

 v |= v >> 1;
 v |= v >> 2;
 v |= v >> 4;
 v |= v >> 8;
 v |= v >> 16;
 v = (v >> 1) + 1;

 return MultiplyDeBruijnBitPosition[(v * 0x077CB531UL) >> 27];
}

u8 get_first_set(u32 x)
{
 if(x == 0){ return -1; }
 x = (x & -x);

 u8 j = 0;

 while(x != 1)
 {
  x = x >> 1;
  j++;
 }
 return j;
}
# 71 "data_structures/util.c"
void setm(u32 *start, u32 size, u32 val){
  u32 i;
  for(i = 0; i < size; i += 1){
    *(start + i) = val;
  }
}
# 2 "datastr.c" 2
# 1 "data_structures/bitmap.c" 1
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
# 3 "datastr.c" 2
# 1 "data_structures/stack.c" 1
typedef struct
{
  u32 max;
  u32 *base;
  u32 *top;
} stack_t;

u32 stack_push(stack_t *s, u32 x){
  if((s->top - s->base >= s->max) && (s->top != s->base - 1)){
    return -1;
  }
  s->top += 1;


  *(s->top) = x;

  return 0;
}
u32 stack_pop(stack_t *s){
  if(s->top < s->base){
    return -1;
  }
  s->top -= 1;
  u32 x = *(s->top);
  return x;
}
u32 stack_peek(stack_t *s){
  if(s->top < s->base) return -1;
  return *((s->top));
}
stack_t *stack_create(u32 m){
  stack_t *s = (stack_t *)kmalloc(sizeof(stack_t));
  s->max = m;
  s->base = (u32 *)kmalloc(4*m);
  s->top = s->base - 1;


  return s;
}
# 4 "datastr.c" 2
# 1 "data_structures/imap.c" 1
typedef struct
{
  bitmap_t *bitmap;
  stack_t *stack;
} imap_t;

imap_t *imap_create(u32 size)
{

  imap_t *imap = (imap_t *)kmalloc(sizeof(imap_t));
  imap->bitmap = bitmap_create(size*8);
  imap->stack = stack_create(size);
  u32 i;



  for(i = 0; i < size; i++){

    stack_push(imap->stack, size - 1 - i);
  }
  return imap;
}

u32 imap_set_next(imap_t *imap)
{

  u32 index = stack_peek(imap->stack);

  if(index == -1){ return -1; }
  u32 offset = get_first_set(~(bitmap_get_context(imap->bitmap, index)));

  u32 entry = index + offset;

  bitmap_set(imap->bitmap, entry);
  if(bitmap_get_context(imap->bitmap, entry) == 0xFFFFFFFF){
    stack_pop(imap->stack);
  }
  return entry;
}

void imap_stack_reset(imap_t *imap)
{

  imap->stack->top = imap->stack->base - 1;
  u32 i;
  for(i = 0; i < imap->bitmap->size; i++){
    if(*(imap->bitmap->bits + i) != 0xFFFFFFFF){
      stack_push(imap->stack, i);
    }
  }


}

void imap_clear(imap_t *imap, u32 entry)
{
  u32 x = bitmap_get_context(imap->bitmap, entry);
  if(x == 0xFFFFFFFF){
    stack_push(imap->stack, entry/32);
  }
  bitmap_clear(imap->bitmap, entry);
}
# 5 "datastr.c" 2
# 3 "kernel.c" 2
# 1 "gui/monitor.c" 1
# 1 "gui/font.h" 1
u32 fwidth = 8;
u32 fheight = 8;

char font[128][8] = {0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
85,170,85,170,85,170,85,170,
0,0,0,0,0,0,0,0,
170,85,170,85,170,85,170,85,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,24,24,24,24,0,24,0,
0,54,54,54,0,0,0,0,
0,36,126,36,36,126,36,0,
24,60,90,120,28,90,62,24,
0,34,84,40,20,42,68,0,
0,60,66,36,58,68,58,0,
0,12,24,48,0,0,0,0,
16,32,64,64,64,64,32,16,
8,4,2,2,2,2,4,8,
16,16,124,56,68,0,0,0,
0,24,24,126,126,24,24,0,
0,0,0,0,0,24,48,96,
0,0,0,126,126,0,0,0,
0,0,0,0,0,48,48,0,
0,2,4,8,16,32,64,0,
0,60,70,74,82,98,60,0,
0,24,56,8,8,8,126,0,
0,60,66,4,8,16,126,0,
0,60,66,24,4,66,60,0,
0,12,20,36,126,4,4,0,
0,126,64,124,2,2,124,0,
0,60,64,120,68,68,56,0,
0,126,4,8,124,32,64,0,
0,60,66,66,60,66,60,0,
0,60,66,66,62,2,60,0,
0,24,24,0,0,24,24,0,
0,24,24,0,0,24,48,96,
0,0,16,32,64,32,16,0,
0,0,0,126,0,126,0,0,
0,0,8,4,2,4,8,0,
0,60,66,4,8,16,0,16,
0,60,76,84,94,64,60,0,
0,24,60,102,126,102,102,0,
0,124,102,124,102,102,124,0,
0,62,96,96,96,96,62,0,
0,124,102,102,102,102,124,0,
0,126,96,126,126,96,126,0,
0,126,96,126,96,96,96,0,
0,62,96,96,110,102,60,0,
0,102,102,126,126,102,102,0,
0,126,24,24,24,24,126,0,
0,126,24,24,24,88,120,0,
0,102,108,120,120,108,102,0,
0,96,96,96,96,96,126,0,
0,66,102,126,126,102,102,0,
0,66,98,82,74,70,66,0,
0,60,102,102,102,102,60,0,
0,124,102,102,124,96,96,0,
0,60,102,102,102,60,14,0,
0,124,102,102,124,108,102,0,
0,62,96,56,28,6,124,0,
0,126,126,24,24,24,24,0,
0,102,102,102,102,102,60,0,
0,102,102,102,102,36,24,0,
0,66,66,66,90,90,60,0,
0,66,36,24,24,36,66,0,
0,66,66,36,24,24,24,0,
0,126,12,24,60,48,126,0,
112,64,64,64,64,64,64,112,
0,64,32,16,8,4,2,0,

14,2,2,2,2,2,2,14,
24,36,66,0,0,0,0,0,
0,0,0,0,0,0,126,0,
96,48,24,0,0,0,0,0,
0,0,60,66,62,66,62,0,
0,64,64,124,66,66,124,0,
0,0,0,62,64,64,62,0,
0,2,2,62,66,66,62,0,
0,0,60,66,124,64,60,0,
0,8,20,16,56,16,16,0,
0,0,60,68,60,4,68,56,
0,64,64,64,124,66,66,0,
0,0,24,0,24,24,24,0,
0,0,8,0,8,8,72,48,
0,32,32,40,48,40,36,0,
0,56,24,24,24,24,28,0,
0,0,0,60,90,90,90,0,
0,0,0,120,68,68,68,0,
0,0,60,66,66,66,60,0,
0,0,60,66,66,124,64,64,
0,0,60,66,66,62,2,2,
0,0,64,126,64,64,64,0,
0,0,62,64,60,2,124,0,
0,16,16,16,56,16,20,8,
0,0,66,66,66,66,60,0,
0,0,0,66,66,36,24,0,
0,0,0,90,90,90,60,0,
0,0,0,100,24,24,38,0,
0,0,66,66,62,2,66,60,
0,0,0,60,8,16,60,0,
8,16,16,32,16,16,8,0,
24,24,24,24,24,24,24,24,
16,8,8,4,8,8,16,0,
0,0,50,126,204,0,0,0,
0,0,0,0,0,0,0,0,
};
# 2 "gui/monitor.c" 2

u32 cursor_x;
u32 cursor_y;

struct pixel
{
 u8 blue;
 u8 green;
 u8 red;
} __attribute__((packed));

typedef struct pixel pixel_t;

pixel_t *videoram;
u32 width;
u32 height;

void monitor_init(pixel_t *vidstart)
{
 videoram = vidstart;
 width = 800;
 height = 600;
 cursor_x = 0;
 cursor_y = 0;
}

void plot_pixel(u32 x, u32 y, pixel_t px)
{
 pixel_t *p = videoram + (y*width + x);
 p->red = px.red;
 p->green = px.green;
 p->blue = px.blue;
}

void put_bmp(u8 *bmp, u32 w, u32 h, u32 x, u32 y, u8 r, u8 g, u8 b)
{
 u32 bit = 0;
 pixel_t px;
 while(bit < w*h)
 {
  u32 idx = bit / 8;
  u32 off = (8 - (bit % 8)) % 8;
  if(bmp[idx] & (1 << off))
  {
   px.red = r;
   px.green = g;
   px.blue = b;
  }
  else
  {
   px.red = 0;
   px.green = 0;
   px.blue = 0;
  }
  u32 x_off = bit % w;
  u32 y_off = bit / h;
  plot_pixel(x + x_off, y + y_off, px);
  bit++;
 }
}

void cls()
{
 pixel_t *px = videoram;
 while(px < videoram + width * height)
 {
  px->red = px->green = px->blue = 0x00000000;
  px += 1;
 }
 cursor_x = 0;
 cursor_y = 0;
}

void newline()
{
 cursor_x = 0;
 cursor_y += fheight;
}

void scroll()
{
  cursor_y = 0;

}

void putch_rgb(char c, u8 r, u8 g, u8 b)
{
 if(c == '\n')
  newline();
 else if(c == '\r')
  cursor_x = 0;
 else if(c == 8 && cursor_x)
  cursor_x -= fwidth;
 else if(c == 9 || c == '\t')
         cursor_x = (cursor_x + 8*fwidth) - (cursor_x % (8*fwidth));
 else if(c >= 32)
 {
  put_bmp(font[c], fwidth, fheight, cursor_x, cursor_y, r, g, b);
  cursor_x += fwidth;
  if(cursor_x >= width)
   newline();
 }
 if(cursor_y + fheight >= height)
  scroll();
}

void putch(char c)
{
 putch_rgb(c, 0xff, 0xff, 0xff);
}



void print_hex_32(u32 x)
{
 char output[8];
 u8 i, len = 0;
 for(i = 0; i < 8; i++)
 {
  u8 d = x & 0x0f;
  if(d)
   len = i;
  if(d > 9)
   output[7 - i] = (55 + d);
  else
   output[7 - i] = (48 + d);
  x = x >> 4;
 }
 for(i = 7-len; i < 8; i++)
  putch(output[i]);
}

void print_dec_32(u32 x)
{
 char output[10];
 u8 i = 0;
 while(x)
 {
  output[i] = (48 + (x % 10));
  i++;
  x /= 10;
 }
 while(i > 0)
  putch(output[--i]);
}

void print_bin_32(u32 x)
{
 char output[32];
 u8 len = 0;
 u8 i = 0;
 u32 a = 1;
 while(a)
 {
  if(x & a)
  {
   output[i] = '1';
   len = i+1;
  }
  else
   output[i] = '0';
  a = a << 1;
  i++;
 }
 while(len)
  putch(output[--len]);
}

void prints(char *s)
{
 u32 i = 0;
 while(s[i])
  putch(s[i++]);
}

void bitmap_print(bitmap_t *bmap)
{
 u32 *i;
 for(i = bmap->bits; i < bmap->bits + bmap->size; i += 1)
 {
  print_bin_32(*i);
 }
 putch('\n');
}

void print_addr(void *ptr){
  print_hex_32((u32)(ptr));
}
# 4 "kernel.c" 2
# 1 "keybd/keyboard.c" 1
# 1 "keybd/scancodes.h" 1
# 33 "keybd/scancodes.h"
s8 scancodes[84] = {
0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0x8,
0x9,'q','w','e','r','t','y','u','i','o','p','[',']',-1,-2,'a','s',
'd','f','g','h','j','k','l',';','\'','`',-3,'\\','z','x','c','v',
'b','n','m',',','.','/',-4,'*',-5,' ',-6,0,0,0,0,0,
0,0,0,0,0,0,0,-7,-8,-14,'-',-11,'5',-10,'+',-13,
-9,-15,-16,-17};

s8 scancodes_shift[84] = {
0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0x8,
0x9,'Q','W','E','R','T','Y','U','I','O','P','{','}',-1,-2,'A','S',
'D','F','G','H','J','K','L',':','\"','~',-3,'|','Z','X','C','V',
'B','N','M','<','>','?',-4,'*',-5,' ',-6,0,0,0,0,0,
0,0,0,0,0,0,0,-7,-8,-14,'-',-11,'5',-10,'+',-13,
-9,-15,-16,-17};
# 2 "keybd/keyboard.c" 2

u8 shift_down = 0;

void keyboard_handler()
{
 u8 sc = inb(0x60);
 if(sc)
 {
  if(sc & (0x80))
  {
   sc &= 0x7f;
   if(scancodes[sc] == -3 || scancodes[sc] == -4)
    shift_down = 0;
  }
  else
  {
   if(scancodes[sc] == -3 || scancodes[sc] == -4)
   {
    shift_down = 1;
   }
   else
   {
    s8 input;
    input = (shift_down ? scancodes_shift[sc] : scancodes[sc]);
    if(input > 0)
     putch(input);
   }
  }
 }
 outb(0x20, 0x20);
}
# 5 "kernel.c" 2
# 1 "init/paging.c" 1
# 12 "init/paging.c"
typedef struct
{
  u32 present:1;
  u32 rw:1;
  u32 user:1;
  u32 accessed:1;
  u32 dirty:1;
  u32 unused:7;
  u32 frame:20;
} page_t;

typedef struct
{
  page_t pages[1024];
} page_table_t;

typedef struct
{
  u32 table_phys_addresses[1024];
  page_table_t *tables[1024];

} pagedir_t;

typedef struct
{
  u32 UID;
  u32 esp;
  u32 kesp;
  u32 ebp;
  u32 eip;
  pagedir_t *pagedir;
  u32 pagedir_physical_address;
  void *next;
} node_t;

imap_t *frames_map;
node_t *kernel_node;
node_t *current_node;

void get_a_frame(page_t *page, u8 mode, u8 rw)
{

  if(page->frame != 0){
    return;
  }
  else{

    u32 i = imap_set_next(frames_map);

    if(i == -1){
      return;
    }
    page->present = 1;
    page->rw = rw?1:0;
    page->user = mode?1:0;
    page->frame = i;
  }
}

void get_specific_frame(page_t *page, u32 frame_addr, u8 mode, u8 rw)
{
  if(page->frame != 0)
    return;
  else{
    bitmap_set(frames_map->bitmap, (frame_addr >> 12));
    page->present = 1;
    page->rw = rw?1:0;
    page->user = mode?1:0;
    page->frame = (frame_addr >> 12);
  }
}

void release_frame(page_t *page)
{
  if(!(page->frame)){
    return;
  }
  else{
    imap_clear(frames_map, page->frame);
    page->frame = 0;
  }
}

void switch_pagedir(u32 pagedir_physical_address)
{
  asm volatile("mov %0, %%cr3":: "r"(pagedir_physical_address));
  u32 cr0;
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000;
  asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u32 address, u8 make, pagedir_t *dir)
{
  address /= 0x1000;
  u32 table_index = address / 1024;
  if (dir->tables[table_index]){
    return &dir->tables[table_index]->pages[address%1024];
  }
  else if(make){
    u32 tmp;
    dir->tables[table_index] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
    setm((u32 *)dir->tables[table_index], 0, 0x1000);


    dir->table_phys_addresses[table_index] = tmp | 0x7;
    return &dir->tables[table_index]->pages[address%1024];
  }
  else{
      return 0;
  }
}

void paging_idmap(void *start, u32 size, u8 mode, u8 rw, pagedir_t *dir)
{
  void *i;
  for(i = start; i < start + size; i += 0x1000)
    get_specific_frame(get_page((u32)i, 1, dir), (u32)i, mode, rw);
}

void paging_mapto(void *start, u32 size, void *target, u8 mode, u8 rw, pagedir_t *dir)
{
  void *i;
  for(i = 0; (u32)i < size; i += 0x1000)
    get_specific_frame(get_page((u32)(target + (u32)i), 1, dir), (u32)(start + (u32)i), mode, rw);
}

void alloc_pages_for(void *start, u32 size, u8 mode, u8 rw, pagedir_t *dir){

  void *i;
  for(i = 0; (u32)i < size; i += 0x1000){
    page_t *p = get_page((u32)(start + (u32)i), 1, dir);


    get_a_frame(p, mode, rw);

  }

}

void free_pages(void *start, u32 size, pagedir_t *dir){
  void *i;
  for(i = 0; (u32)i < size; i += 0x1000)
    release_frame(get_page((u32)(start + (u32)i), 1, dir));
}

void paging_init()
{
  u32 mem_size = 0x10000000;
  u32 num_frames = mem_size/0x1000;

  frames_map = imap_create(num_frames);



  kernel_node = (node_t *)(kmalloc(sizeof(node_t)));
  setm((u32 *)kernel_node, sizeof(node_t), 0);

  kernel_node->pagedir = (pagedir_t *)kmalloc_a(sizeof(pagedir_t));
  setm((u32 *)kernel_node->pagedir, sizeof(pagedir_t), 0);


  kernel_node->pagedir_physical_address = (u32)(kernel_node->pagedir->table_phys_addresses) - phys_offset;
  current_node = kernel_node;



  paging_idmap((void *)(videoram), height*width*3, 1, 0, kernel_node->pagedir);
  paging_mapto((void *)0x10000, 0x20000, (void *)0xC0000000, 1, 0, kernel_node->pagedir);
  paging_mapto((void *)0x100000, 0x800000, (void *)0xC1000000, 1, 0, kernel_node->pagedir);
  paging_idmap((void *)0x40000, 0x1000, 1, 0, kernel_node->pagedir);
  paging_idmap((void *)0x30000, 0x10000, 1, 1, kernel_node->pagedir);


  u32 i;
  for(i = 0x300; i < 0x400; i++) get_page(i*0x400000, 1, kernel_node->pagedir);
# 200 "init/paging.c"
  switch_pagedir(kernel_node->pagedir_physical_address);

}
# 6 "kernel.c" 2

# 1 "kheap.c" 1
typedef struct slab_descriptor{
  void *start;
  u32 refcount;
  void *next;
  void *prev;
} slab_descriptor_t;

typedef struct block_descriptor{
  void *start;
  slab_descriptor_t *slab_desc;
  void *prev;
  void *next;
} block_descriptor_t;

typedef struct kheap{
  u32 flags;
  u32 block_size;
  u32 slab_size;
  u32 blocks_per_slab;
  u32 max_size;
  u32 alignment;
  void *(*constructor)(void *);
  void *(*destructor)(void *);
  void *start;
  void *end;
  block_descriptor_t *first_free;
  block_descriptor_t *last_free;
  slab_descriptor_t *first_free_slab;
  slab_descriptor_t *last_free_slab;
  u32 slabs_per_bchunk;
  u32 bchunks_per_schunk;
  u32 bchunk_metadata_size;
  u32 schunk_metadata_size;
  u32 effective_block_size;
  u32 bchunk_size;
  u32 schunk_size;
} kheap_t;

typedef struct block_info{
  block_descriptor_t *block_desc;
  slab_descriptor_t *slab_desc;
  void *schunk_start;
  void *bchunk_start;
} block_info_t;





void slab_debug_print(slab_descriptor_t *s);
void block_debug_print(block_descriptor_t *b);
void kheap_debug_print(kheap_t *heap, u32 detail);
void print_info(block_info_t *info);

inline u32 get_schunk_index(kheap_t *heap, u32 info_start){
  return (info_start - (u32)(heap->start))/(heap->schunk_size);
}
inline u32 get_schunk_start(kheap_t *heap, u32 info_start){
  return get_schunk_index(heap, info_start) * heap->schunk_size + (u32)(heap->start);
}
inline u32 get_bchunks_start(kheap_t *heap, u32 info_start){
  return get_schunk_start(heap, info_start) + heap->schunk_metadata_size;
}

void get_block_info(kheap_t *heap, void *block_start, block_info_t *info){



  u32 u32_block_start = (u32)(block_start);
  u32 schunk_index = (u32_block_start - (u32)(heap->start))/(heap->schunk_size);

  info->schunk_start = (void *)(schunk_index * (heap->schunk_size) + (u32)(heap->start));

  u32 bchunks_start = (u32)(info->schunk_start) + heap->schunk_metadata_size;

  u32 bchunk_index = (u32_block_start - bchunks_start)/(heap->bchunk_size);
  info->bchunk_start = (void *)(bchunk_index * (heap->bchunk_size) + bchunks_start);

  u32 blocks_start = (u32)(info->bchunk_start) + heap->bchunk_metadata_size;
  u32 block_index = (u32_block_start - blocks_start)/(heap->block_size);
  info->block_desc = (block_descriptor_t *)(info->bchunk_start + block_index*sizeof(block_descriptor_t));

  u32 slab_index = bchunk_index * (heap->slabs_per_bchunk) + (heap->slab_size) * ((u32_block_start - blocks_start)/(heap->slab_size));
  info->slab_desc = (slab_descriptor_t *)(info->schunk_start + sizeof(slab_descriptor_t)*slab_index);

  return;
}

u32 align_by(u32 size, u32 block_size){ return size + (block_size == 0 ? 0 : (size % block_size != 0 ? block_size : 0) - (size % block_size)); }






u32 expand(kheap_t *heap){
  void *new_end = heap->end + heap->slab_size;
  void *new_slab_start = heap->end;

  if((new_end - heap->start)/(heap->schunk_size) > (heap->end - heap->start)/(heap->schunk_size)){
    new_end += heap->schunk_metadata_size + heap->bchunk_metadata_size;

  }
  else if(((u32)new_end - get_bchunks_start(heap, (u32)new_end - heap->block_size))/(heap->bchunk_size) > ((u32)(heap->end) - get_bchunks_start(heap, (u32)(heap->end) - heap->block_size))/(heap->bchunk_size)){

    new_end += heap->bchunk_metadata_size;
  }






  alloc_pages_for(heap->end, (u32)new_end - (u32)(heap->end), 1, 1, kernel_node->pagedir);



  heap->end = new_end;






  block_info_t info;
  get_block_info(heap, new_slab_start, &info);


  info.slab_desc->start = new_slab_start;
  info.slab_desc->refcount = 0;

  info.slab_desc->prev = heap->last_free_slab;
  info.slab_desc->next = 0;
  if(heap->last_free_slab != 0) heap->last_free_slab->next = info.slab_desc;
  heap->last_free_slab = info.slab_desc;

  u32 i = 1;
  block_descriptor_t *b = info.block_desc;
  if(heap->first_free == 0) heap->first_free = b;
  b->start = new_slab_start;
  b->next = b+1;
  b->prev = heap->last_free;
  b->slab_desc = info.slab_desc;

  b = b+1;
  while(i < heap->blocks_per_slab - 1){
    b->start = new_slab_start + i*(heap->block_size);

    b->next = b+1;
    b->prev = b-1;
    b->slab_desc = info.slab_desc;
    b = b+1;
    i++;
  }
  b->next = 0;
  b->prev = b-1;
  heap->last_free = b;

  return 0;
}





u32 contract(kheap_t *heap){
  if(heap->first_free_slab == 0) return -1;
  free_pages(heap->first_free_slab->start, heap->slab_size, kernel_node->pagedir);
  heap->first_free_slab = heap->first_free_slab->next;
}





kheap_t *kheap_init(kheap_t *heap, u32 b_size, u32 s_size, u32 m_size, u32 align, void * (*constr)(void *), void *(*destr)(void *), void *location){
  heap->block_size = b_size;
  heap->slab_size = s_size;
  heap->blocks_per_slab = s_size / b_size;
  heap->max_size = m_size;
  heap->alignment = align;
  heap->constructor = constr;
  heap->destructor = destr;
  heap->start = location;
  heap->slabs_per_bchunk = 4096/sizeof(block_descriptor_t) / ((heap->slab_size)/(heap->block_size));
  heap->bchunks_per_schunk = 4096/sizeof(slab_descriptor_t) / heap->slabs_per_bchunk;
  heap->bchunk_metadata_size = align_by(4096/sizeof(block_descriptor_t)*sizeof(block_descriptor_t), 4096);
  heap->schunk_metadata_size = align_by(4096/sizeof(slab_descriptor_t)*sizeof(slab_descriptor_t), 4096);
  heap->end = location + heap->bchunk_metadata_size + heap->schunk_metadata_size;
  heap->effective_block_size = align_by(heap->block_size, heap->alignment);
  heap->bchunk_size = heap->slabs_per_bchunk*(heap->slab_size) + heap->bchunk_metadata_size;
  heap->schunk_size = 4096/sizeof(slab_descriptor_t)*(heap->slab_size) + (heap->bchunk_metadata_size)*(heap->bchunks_per_schunk) + (heap->schunk_metadata_size);
  heap->first_free = 0;
  heap->last_free = 0;
  heap->first_free_slab = 0;
  heap->last_free_slab = 0;
  alloc_pages_for(heap->start, (u32)(heap->end) - (u32)(heap->start), 1, 1, kernel_node->pagedir);
  if(expand(heap) != 0) return heap;
  return 0;
}





void *kheap_alloc(kheap_t *heap){

  if(heap->first_free == 0) if(expand(heap) != 0) return 0;



  block_descriptor_t *new_block = heap->first_free;

  block_debug_print(new_block);
  heap->first_free = heap->first_free->next;
  heap->first_free->prev = 0;
  new_block->next = 0;
  new_block->prev = 0;


  block_info_t info;
  get_block_info(heap, new_block->start, &info);


  info.slab_desc->refcount++;



  if(info.slab_desc->refcount == 1){
    if(info.slab_desc->next == 0) heap->last_free_slab = info.slab_desc->prev;
    else ((slab_descriptor_t *)(info.slab_desc->next))->prev = (slab_descriptor_t *)(info.slab_desc->prev);
    if(info.slab_desc->prev == 0) heap->first_free_slab = info.slab_desc->next;
    else ((slab_descriptor_t *)(info.slab_desc->prev))->next = (slab_descriptor_t *)(info.slab_desc->next);
  }


  return new_block->start;
}






void kheap_free(kheap_t *heap, void *block_start){

  block_info_t info;
  get_block_info(heap, block_start, &info);
  block_descriptor_t *to_free = info.block_desc;
  to_free->next = 0;
  to_free->prev = heap->last_free;
  heap->last_free = to_free;


  info.slab_desc->refcount--;


  if(info.slab_desc->refcount == 0){
    info.slab_desc->next = 0;
    info.slab_desc->prev = heap->last_free_slab;
    heap->last_free_slab = info.slab_desc;
  }




  return;
}
# 8 "kernel.c" 2

# 1 "initrd/initrd.c" 1
# 12 "initrd/initrd.c"
typedef struct rd_entry
{
  char name[16];

  u32 start;
  u32 size;
} rd_entry_t;

typedef struct rd_header
{
  u32 size;
  u32 num_files;
  rd_entry_t *root;
} rd_header_t;

u32 rd_num_files;
rd_entry_t *rd_entries;
u32 rd_data;

u32 rd_read(rd_entry_t *node, u8 offset, u32 size, char *buffer)
{
  u32 data_start = (u32)(node->start);
  data_start += rd_data;



  if(offset + size > node->size)
    return 1;
  u32 i = 0;

  while(i < size){
    *((char *)(buffer+i)) = *((char *)data_start + offset + i);
    i++;
  }


  return 0;
}

u32 rd_write(rd_entry_t *node, u8 offset, u32 size, u32 *buffer)
{
  return 0;
}
void rd_open(rd_entry_t *node, u8 read, u8 write)
{
  return;
}
void rd_close(rd_entry_t *node)
{
  return;
}
rd_entry_t *rd_readdir(rd_entry_t *node, u32 index)
{





  return 0;
}
rd_entry_t *rd_finddir(rd_entry_t *node, char *name)
{
# 82 "initrd/initrd.c"
  return 0;
}
rd_entry_t *rd_find(char *name)
{
  u32 i;
  for(i = 0; i <= rd_num_files; i++){
    rd_entry_t *next = &(rd_entries[i]);
    u32 j = 0;

    while(next->name[j] == name[j])
      if(name[j++] == 0) return next;
  }
  prints("NOT FOUND!");
  return 0;
}

void rd_init(void *ramdisk)
{
  rd_data = (u32)ramdisk;
  rd_entries = (rd_entry_t *)((u32 *)ramdisk + 1);
  rd_num_files = *((u32 *)(ramdisk));
  prints(" ");print_hex_32(rd_num_files);prints(" ");


  prints(rd_entries[1].name);prints(" ");
  rd_data = (u32)(rd_entries + rd_num_files);

  print_hex_32(sizeof(struct rd_entry));prints("! ");
  print_hex_32(rd_data);prints("! ");

}
# 10 "kernel.c" 2
# 1 "net/rtl8029.c" 1
typedef struct
{
  u16 bus, slot, func;
  u16 io_base;
} rtl8029_dev_t;

typedef struct
{
  u16 size;
  u16 *data;
} rtl8029_packet_t;

u8 COMMAND = 0;
u8 wPSTART=0x1,wPSTOP=0x2,wBNRY=0x3,wTPSR=0x4,wTBCR0=0x5,wTBCR1=0x6,wISR=0x7,wRSAR0=0x8,wRSAR1=0x9,wRBCR0=0xA,wRBCR1=0xB,wRCR=0xC,wTCR=0xD,wDCR=0xE,wIMR=0xF,wCURR=0x7;
u8 rCLDA0=0x1,rCLDA1=0x2,rBNRY=0x3,rTSR=0x4,rNCR=0x5,rFIFO=0x6,rISR=0x7,rCRDA0=0x8,rCRDA1=0x9,rRSR=0xC,rCNTR0=0xD,rCNTR1=0xE,rCNTR2=0xF;
u8 rPSTART=0x1,rPSTOP=0x2,rTPSR=0x4,rRCR=0xC,rTCR=0xD,rDCR=0xE,rIMR=0xF;
u8 IO_PORT=0x10;
u8 RESET_PORT=0x18;

void rtl8029_write(rtl8029_dev_t *dev, u8 offset, u8 value)
{
  outb(dev->io_base + offset, value);
}
u8 rtl8029_read(rtl8029_dev_t *dev, u8 offset)
{
  return inb(dev->io_base + offset);
}




void rtl8029_init(rtl8029_dev_t *dev)
{
  rtl8029_write(dev, 0, 0x21);
  rtl8029_write(dev, wDCR, 0x49);
  rtl8029_write(dev, wRBCR0, 0);
  rtl8029_write(dev, wRBCR1, 0);
  rtl8029_write(dev, wRCR, 0);
  rtl8029_write(dev, wTPSR, 0x20);
  rtl8029_write(dev, wTCR, 0x02);
  rtl8029_write(dev, wPSTART, 0x26);
  rtl8029_write(dev, wBNRY, 0x26);
  rtl8029_write(dev, wPSTOP, 0x40);
  rtl8029_write(dev, 0, 0x61);
  rtl8029_write(dev, wCURR, 0x26);
  rtl8029_write(dev, 0, 0x22);
  rtl8029_write(dev, wISR, 0xFF);
  rtl8029_write(dev, wIMR, 0x0B);
  rtl8029_write(dev, wTCR, 0);
}




void rtl8029_send(rtl8029_dev_t *dev, rtl8029_packet_t *packet)
{
  prints("Here we go!");
  rtl8029_write(dev, COMMAND, 0x22);
  rtl8029_write(dev, wRBCR0, (u8)(packet->size & 0xFF));
  rtl8029_write(dev, wRBCR1, (u8)((packet->size >> 8) & 0xFF));
  rtl8029_write(dev, wISR, rtl8029_read(dev, rISR) | 0x40);
  rtl8029_write(dev, 0, 0x12);
  u16 *i;
  for(i = packet->data; i < packet->data + packet->size; i++)
    rtl8029_write(dev, IO_PORT, *i);

  while(!(rtl8029_read(dev, rISR) & 0x40)){}

  prints("Done\n");
}
# 11 "kernel.c" 2
# 1 "pci/pci.c" 1
typedef struct
{
  u16 vendorID;
  u16 deviceID;
  u16 command;
  u16 status;
  u16 revisionID;
  u8 subclass;
  u8 class_code;
  u8 cache_line_size;
  u8 latency_timer;
  u8 header_type;
  u8 BIST;
  u32 BAR0;
  u32 BAR1;
  u32 BAR2;
  u32 BAR3;
  u32 BAR4;
  u32 BAR5;
  u32 CIS;
  u16 sub_vendorID;
  u16 subID;
  u32 expansion_ROM_base;
  u32 res1;
  u32 res2;
  u8 int_line;
  u8 int_pin;
  u8 min_grant;
  u8 max_latency;
} pci_t;

inline u32 inl(unsigned short port)
{
  u32 result;
  __asm__ __volatile__("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
  return result;
}

inline void outl(unsigned short port, u32 data)
{
  __asm__ __volatile__("outl %%eax, %%dx" : : "d" (port), "a" (data));
}

u16 pci_config_read(u16 bus, u16 slot, u16 func, u16 off)
{
  u32 address =
    ((u32)bus << 16) |
    (u32)slot << 11 |
    (u32)func << 8 |
    (u32)off & 0xfc |
    (u32)0x80000000;

  outl(0xcf8, address);
  return ((u16)(inl(0xcfc) >> (((u32)off & 2) * 8)) & 0xffff);
}

void pci_configure_device(u16 bus, u16 slot, u16 func, u16 VID, u16 DID)
{
  if(VID == 0x10EC && (DID == 0x8029 || DID == 0x8021)){
    prints("RTL8029 found!\n");
    rtl8029_dev_t *dev = (rtl8029_dev_t *)kmalloc(sizeof(rtl8029_dev_t));
    dev->io_base = pci_config_read(bus, slot, func, 0x10);
    dev->bus = bus;
    dev->slot = slot;
    dev->func = func;
    rtl8029_init(dev);
  }
  else if(VID == 0x10B7 && (DID == 0x9055 || DID == 0x9050 || DID == 0x9051 || DID == 0x9056 || DID == 0x9058 || DID == 0x905A || DID == 0x9200)){
    prints("3C905x found!\n");
  }
  else if(VID == 0x10EC && (DID == 0x0139 || DID == 0x8129 || DID == 0x8138 || DID == 0x8139)){
    prints("RTL8139 found\n");
  }
}

void pci_init()
{
  prints("INITIZING PCI: \n");
  u16 i, j, k;







  prints("BUS\tSLOT\tVID\tDID\tHTYPE\tBAR0\tBAR1\tBAR2\tBAR3\tBAR4\tBAR5\n");

  for(i = 0; i < 1; i++){
    for(j = 0; j < 100; j++){

      u16 VID = pci_config_read(i, j, 0, 0);
      if(VID != 0xFFFF){
 print_hex_32(i); prints("\t"); print_hex_32(j);prints("\t");
 u16 DID = pci_config_read(i, j, 0, 2);
 print_hex_32((u32)VID);prints("\t");print_hex_32((u32)DID);prints("\t");
 u8 HTYPE = (u8)(pci_config_read(i, j, 0, 0xE));
 print_hex_32((u32)HTYPE);prints("\t");

 u16 BAR[6];
 for(k = 0; k < 6; k++){
   BAR[k] = pci_config_read(i, j, 0, k*4 + 0x10);
   print_hex_32((u32)BAR[k]); prints("\t");
 }
 prints("\n");
 pci_configure_device(i, j, 0, VID, DID);
      }

    }
  }
}
# 12 "kernel.c" 2


# 1 "usermode.c" 1

struct tss_entry
{
  u32 prev_tss;
  u32 esp0;
  u32 ss0;
  u32 esp1;
  u32 ss1;
  u32 esp2;
  u32 ss2;
  u32 cr3;
  u32 eip;
  u32 eflags;
  u32 eax;
  u32 ecx;
  u32 edx;
  u32 ebx;
  u32 esp;
  u32 ebp;
  u32 esi;
  u32 edi;
  u32 es;
  u32 cs;
  u32 ss;
  u32 ds;
  u32 fs;
  u32 gs;
  u32 ldt;
  u16 trap;
  u16 iomap_base;
} __attribute__((packed));

typedef struct tss_entry tss_entry_t;

void to_usermode()
{
   asm volatile("       cli;      mov $0x23, %ax;      mov %ax, %ds;      mov %ax, %es;      mov %ax, %fs;      mov %ax, %gs;                         mov %esp, %eax;      pushl $0x23;      pushl %eax;      pushf;      pop %eax;      or $0x200, %eax;      push %eax;      pushl $0x1B;      push $1f;      nop;      iret;    1:      ");
# 58 "usermode.c"
}

void tss_init(tss_entry_t *tss, u32 ss0, u32 esp0)
{
  tss->ss0 = ss0;
  tss->esp0 = esp0;
  tss->cs = 0x0b;
  tss->ss = 0x13;
  tss->ds = 0x13;
  tss->es = 0x13;
  tss->fs = 0x13;
  tss->gs = 0x13;
}
# 15 "kernel.c" 2
# 1 "debug.c" 1
# 69 "debug.c"
void stack_debug_print(stack_t *stack){
  prints("\nmax: ");print_hex_32(stack->max);
  prints("\nbase: ");print_addr(stack->base);
  prints("\ntop: ");print_addr(stack->top);
  prints("\n");
}



void bitmap_debug_print(bitmap_t *bitmap){
  u32 i = 0;
  for(i = 0; i < bitmap->size; i++){
    print_hex_32((u32)(*(bitmap->bits + i*sizeof(u32))));
  }
}


void imap_debug_print(imap_t *imap){
  stack_debug_print(imap->stack);

}




void paging_debug_print()
{
  prints("phys=");print_hex_32(kernel_node->pagedir_physical_address);
  u32 i;
  for(i = 0; i <= (EOK / 0x10); i++){
    u32 tab = i/1024;
    u32 pag = i % 1024;
    prints("T=");print_hex_32(tab);prints(", P=");print_hex_32(pag);prints(": ");
    print_hex_32(kernel_node->pagedir->table_phys_addresses[tab]);
    prints("=");
    print_hex_32((u32)(kernel_node->pagedir->tables[tab]));
    prints(" ");
    print_hex_32( ((page_table_t *)((kernel_node->pagedir->table_phys_addresses[tab]) - 7)) -> pages[pag].frame);
    prints("=");
    print_hex_32(kernel_node->pagedir->tables[tab]->pages[pag].frame);
    prints("=");
    print_hex_32(*((u32 *)(&(kernel_node->pagedir->tables[tab]->pages[pag]))));
    prints("\n");
    if(i == 10) break;

  }




}

void kheap_debug_print(kheap_t *heap, u32 detail){
  prints("block_size: ");print_hex_32(heap->block_size);
  prints("  slab_size: ");print_hex_32(heap->slab_size);
  prints("  max_size: ");print_hex_32(heap->max_size);
  prints("  start: ");print_addr(heap->start);
  prints("  end: ");print_addr(heap->end);
  prints("  alignment: ");print_hex_32(heap->alignment);
  prints("\n");
  if(detail >= 1){
    prints("blocks_per_slab: ");print_hex_32(heap->blocks_per_slab);
    prints("  bchunk_size: ");print_hex_32(heap->bchunk_size);
    prints("  schunk_size: ");print_hex_32(heap->schunk_size);
    prints("\n");
    if(detail >= 2){
      prints("slabs_per_bchunk: ");print_hex_32(heap->slabs_per_bchunk);
      prints("  bchunks_per_schunk: ");print_hex_32(heap->bchunks_per_schunk);
      prints("  bchunk_metadata_size: ");print_hex_32(heap->bchunk_metadata_size);
      prints("  schunk_metadata_size: ");print_hex_32(heap->schunk_metadata_size);
      prints("\n");
      if(detail >= 3){
 prints("first_free: ");print_addr(heap->first_free);
 prints("  last_free: ");print_addr(heap->last_free);
 prints("  first_free_slab: ");print_addr(heap->first_free_slab);
 prints("  last_free_slab: ");print_addr(heap->last_free_slab);
 prints("\n");
      }
    }
  }
}

void print_info(block_info_t *info){
  prints("\nschunk_start: ");print_addr(info->schunk_start);
  prints("\nbchunk_start: ");print_addr(info->bchunk_start);
  prints("\nblock_desc: ");print_addr(info->block_desc);
  prints("\nslab_desc: ");print_addr(info->slab_desc);
  prints("\n");
}


void slab_debug_print(slab_descriptor_t *s){
  prints("\nstart: ");print_addr(s->start);
  prints("\nrefcount: ");print_hex_32(s->refcount);
  prints("\nnext: ");print_addr(s->next);
  prints("\nprev: ");print_addr(s->prev);
  prints("\n");
}

void block_debug_print(block_descriptor_t *b){
  prints("\nstart: ");print_addr(b->start);
  prints("\nslab_desc: ");print_addr(b->slab_desc);
  prints("\nprev: ");print_addr(b->prev);
  prints("\nnext: ");print_addr(b->next);
  prints("\n");
}
# 16 "kernel.c" 2
# 1 "tasking/task.c" 1
# 12 "tasking/task.c"
kheap_t *stack_heap;
kheap_t *pagedir_heap;
kheap_t *node_heap;
kheap_t *pagetable_heap;

kheap_t *event_heap;
kheap_t *connection_heap;

u32 pid = 0;
# 34 "tasking/task.c"
u32 get_EIP(){
  u32 ee;
  asm volatile("pop %eax");
  asm volatile("mov %%eax, %0": "=r"(ee));
  asm volatile("push %eax");
  return ee;
}

void *new_pagedir(void *start)
{
  pagedir_t *pd = (pagedir_t *)(start);
  u32 i;
  for(i = 0x300; i < 1024; i++){
    pd->tables[i] = kernel_node->pagedir->tables[i];
    pd->table_phys_addresses[i] = kernel_node->pagedir->table_phys_addresses[i];
  }
}

void *node_init(void *start)
{
  node_t *new_node = (node_t *)(start);
  new_node->UID = pid++;
  new_node->kesp = (u32)(kheap_alloc(stack_heap));
  new_node->esp = 0x080000;
  new_node->ebp = 0x080000;
  new_node->pagedir = (pagedir_t *)(kheap_alloc(pagedir_heap));
  return new_node;
}

void dump_node(node_t *to_dump)
{
  kheap_free(node_heap, to_dump);
}


void tasking_init()
{
  stack_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), 0x2000, 0x2000, 0x1000000, 0, &zero, &zero, (void *)0xE0000000);
  pagedir_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), 0x1000, 0x10000, 0x1000000, 0, &new_pagedir, &new_pagedir, (void *)0xE2000000);
  node_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), sizeof(node_t), 0x10000, 0x1000000, 0, &node_init, &zero, (void *)0xE1000000);
  pagetable_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), 0x1000, 0x10000, 0x1000000, 0, &zero, &zero, (void *)0xE3000000);
}

node_t *make_node()
{
  node_t *new_node = (node_t *)(kheap_alloc(node_heap));
  new_node->pagedir = (pagedir_t *)(kheap_alloc(pagedir_heap));
  new_node->esp = (void *)(kheap_alloc(stack_heap));
  new_node->UID = ++pid;
  return new_node;
}

node_t *load_node(rd_entry_t *to_load)
{
  node_t *target = make_node();
  switch_pagedir(target->pagedir);
  alloc_pages_for(0, to_load->size + 0x1000, 0, 1, target->pagedir);
  rd_read(to_load, 0, to_load->size, (void *)0x00);
}

void switch_node(node_t *next_node)
{
  current_node->eip = get_EIP();

  if(current_node->eip == 0xFEEDFACE)
    return;

  u32 eip, esp, ebp;
  asm volatile("mov %%esp, %0" : "=r"(esp));
  asm volatile("mov %%ebp, %0" : "=r"(ebp));
  current_node->ebp = ebp;
  current_node->esp = esp;
  current_node = next_node;
  eip = current_node->eip;
  esp = current_node->esp;
  ebp = current_node->ebp;
  asm volatile("      cli;      mov %0, %%ecx;      mov %1, %%esp;      mov %2, %%ebp;      mov $0xFEEDFACE, %%eax;      sti;      jmp *%%ecx" : : "r"(eip), "r"(esp), "r"(ebp));







  switch_pagedir(next_node->pagedir_physical_address);
}
# 17 "kernel.c" 2

void page_fault(registers_t regs)
{
  prints("PAGE FAULT: ");
  u32 faulting_address;
  asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
  prints("at 0x");
  print_hex_32(faulting_address);
  prints(" with error code ");
  print_bin_32(regs.err_code);
  prints("\n");
  asm volatile("hlt");
}

void isr_handler(registers_t regs)
{


  if(regs.int_no == 0xE)
    page_fault(regs);
  else if(regs.int_no == 0x21)
    keyboard_handler();
  else if(regs.int_no == 0xD){
    prints("GPF at ");print_hex_32(regs.eip);prints("!\n");
    asm("hlt");
  }
  else if(regs.int_no == 0x22){
    prints("hellohello!");
  }

}

kheap_t *node_heap;
kheap_t *connection_heap;
kheap_t *event_heap;
# 62 "kernel.c"
void ks_heap_init(){
  node_heap = (kheap_t *)(kmalloc(sizeof(kheap_t)));
  kheap_init(node_heap, sizeof(node_t), align_by(sizeof(node_t), 4096), 0x1000000, 0, &zero, &zero, (void *)0xE1000000);


}

void init(u32 vidstart, u32 rdstart, u32 tss_start)
{
  monitor_init((pixel_t *)vidstart);
  prints("INITIALIZING PAGING!          ");
  paging_init();
  prints("DONE!\nINITIZING KERNEL ALLOCATOR!   ");
  ks_heap_init();
  prints("DONE!\nPREPARING RAMDISK!            ");
  rd_init((void *)rdstart);
  prints("DONE!\nENTERING USERLAND!            ");
  u32 espv;
  asm volatile("mov %%esp, %0": "=r"(espv));
  tss_init((tss_entry_t *)tss_start, 0x10, espv);



  prints("DONE!\nCONFIGURING PCI DEVICES!      ");
  pci_init();
  prints("DONE!\nBOOTING COMPLETE!");
  asm("hlt");
}

void rd_test(){
  rd_entry_t *f = rd_find("hiohi");
  if(f != 0){
    char *b = (char *)(kmalloc(1000));
    rd_read(f, 0, 7, b);
    prints("\nREADING:\n");
    prints((char *)b);
  }
  prints("bye\n");
}

void kheap_test(kheap_t *heap){
  prints("TESTING HEAP\n");
  kheap_debug_print(heap, 3);
  void *p1 = kheap_alloc(heap);
  prints("p1 = ");print_hex_32((u32)p1);prints("\nALLOC SUCCESS!\n");
  void *p2 = kheap_alloc(heap);
  prints("p2 = ");print_hex_32((u32)p2);prints("\nALLOC SUCCESS\n");
  kheap_debug_print(heap, 3);
  kheap_free(heap, p1);
  prints("FREE SUCCESS\n");
  kheap_debug_print(heap, 3);
  void *p3 = kheap_alloc(heap);
  prints("p3 = ");print_hex_32((u32)p3);prints("\nALLOC SUCCESS\n");
  kheap_debug_print(heap, 3);
  prints("\nYAY\n");
}

void kmain(u32 vidstart, u32 rdstart, u32 tss_start){
  prints("hi");print_hex_32(rdstart);
  init(vidstart, rdstart, tss_start);
  u32 espv;
  asm volatile("mov %%esp, %0": "=r"(espv));
  prints("esp: ");print_hex_32(espv);prints("\n");
  prints("vidstart: ");print_hex_32(vidstart);prints("\n");
  prints("rdstart: ");print_hex_32(rdstart);prints("\n");
  prints("tss_start: ");print_hex_32(tss_start);prints("\n");

  rd_test();
  kheap_test(node_heap);

  print_hex_32(get_EIP());





}
