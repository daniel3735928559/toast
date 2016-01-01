#define K_BASE_PHYS 0x10000
#define K_DATA_PHYS 0x100000
#define K_DATA_SIZE 0x800000
#define K_RAMDISK_PHYS 0x40000
#define K_RAMDISK_SIZE 0x1000
#define K_STACK_PHYS 0x30000
#define K_STACK_SIZE 0x10000
#define K_SIZE 0x20000
//#define STACK_BASE 0x90000
//#define STACK_SIZE 0x4000

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
  //u32 physical_address;
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
  //stack_debug_print(frames_map->stack);
  if(page->frame != 0){
    return;
  }
  else{
    //prints("c\n");print_hex_32((u32)(frames_map));prints("\n\n");print_hex_32((u32)(frames_map->stack));prints("\n\n");
    u32 i = imap_set_next(frames_map);
    //prints("a\na");
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
  cr0 |= 0x80000000; // Enable paging!
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
    //print_hex_32(table_index);prints(": ");print_hex_32(tmp);prints("! ");

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
  //prints("Filling ");print_hex_32(size);prints(" b from ");print_hex_32((u32)start);
  void *i;
  for(i = 0; (u32)i < size; i += 0x1000){
    page_t *p = get_page((u32)(start + (u32)i), 1, dir);
    //prints("\nhi\n");
    //print_hex_32((u32)i);prints(" ");
    get_a_frame(p, mode, rw);
    //prints("\nbye");
  }
  //prints("\nabc\n");
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
  //prints("hi");
  frames_map = imap_create(num_frames);

  //setm((u32 *)(frames_map->bitmap->bits), num_frames/32, 0);
  
  kernel_node = (node_t *)(kmalloc(sizeof(node_t)));
  setm((u32 *)kernel_node, sizeof(node_t), 0);

  kernel_node->pagedir = (pagedir_t *)kmalloc_a(sizeof(pagedir_t));
  setm((u32 *)kernel_node->pagedir, sizeof(pagedir_t), 0);

  //Set the kernel dir's physical address by taking its virtual address (which we have now) and subtracting the offset we know has been applied: 
  kernel_node->pagedir_physical_address = (u32)(kernel_node->pagedir->table_phys_addresses) - phys_offset;
  current_node = kernel_node;
  //paging_debug_print();


  paging_idmap((void *)(videoram), height*width*3, 1, 0, kernel_node->pagedir);  //If this is moved to the end of this block then the next few characters printed have the top line of pixels zeroed??
  paging_mapto((void *)K_BASE_PHYS, K_SIZE, (void *)0xC0000000, 1, 0, kernel_node->pagedir);
  paging_mapto((void *)K_DATA_PHYS, K_DATA_SIZE, (void *)0xC1000000, 1, 0, kernel_node->pagedir);
  paging_idmap((void *)K_RAMDISK_PHYS, K_RAMDISK_SIZE, 1, 0, kernel_node->pagedir);
  paging_idmap((void *)K_STACK_PHYS, K_STACK_SIZE, 1, 1, kernel_node->pagedir);

  //Allocate the remainder of the pagetables for kernel space
  u32 i;
  for(i = 0x300; i < 0x400; i++) get_page(i*0x400000, 1, kernel_node->pagedir);
  //alloc_pages_for((void *)KERNEL_SPACE_PAGETABLES, 0x300*0x1000, 1, 1, kernel_node->pagedir);

  //stack_debug_print(frames_map->stack);
  //imap_stack_reset(frames_map);
  //stack_debug_print(frames_map->stack);
  //prints("\n\n");
  //print_hex_32((u32)(kernel_dir->table_phys_addresses[0x300]));
  //asm("hlt");
  //paging_debug_print();
  //print_hex_32((u32)(kernel_node->pagedir->table_phys_addresses));prints(" ");
  //print_hex_32(kernel_node->pagedir_physical_address);
  //while(1);
  switch_pagedir(kernel_node->pagedir_physical_address);
  //prints("hi");
}
