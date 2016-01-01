#define STACK_HEAP_SIZE    0x1000000
#define NODE_HEAP_SIZE     0x100000
#define PD_HEAP_SIZE       0x1000000
#define PT_HEAP_SIZE       0x1000000
#define STACK_HEAP_START  0xE0000000
#define NODE_HEAP_START   0xE1000000
#define PD_HEAP_START     0xE2000000
#define PT_HEAP_START     0xE3000000

#define USER_STACK        0x080000

kheap_t *stack_heap;
kheap_t *pagedir_heap;
kheap_t *node_heap;
kheap_t *pagetable_heap;

kheap_t *event_heap;
kheap_t *connection_heap;

u32 pid = 0;

/*typedef struct
{
  u32 UID;
  u32 esp;
  u32 kesp;
  u32 ebp;
  u32 eip;
  pagedir_t *pagedir;
  u32 pagedir_physical_address;
  void *next;
} node_t;*/

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
  new_node->esp = USER_STACK;
  new_node->ebp = USER_STACK;
  new_node->pagedir = (pagedir_t *)(kheap_alloc(pagedir_heap));
  return new_node;
}

void dump_node(node_t *to_dump)
{
  kheap_free(node_heap, to_dump);
}

//kheap_t *kheap_init(kheap_t *heap, u32 b_size, u32 s_size, u32 m_size, u32 align, void * (*constr)(void *), void *(*destr)(void *), void *location){
void tasking_init()
{
  prints("B");
  stack_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), 0x2000, 0x2000, STACK_HEAP_SIZE, 0, &zero, &zero, (void *)STACK_HEAP_START);
  prints("B");
  pagedir_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), 0x1000, 0x10000, PD_HEAP_SIZE, 0, &new_pagedir, &new_pagedir, (void *)PD_HEAP_START);
  prints("B");
  pagetable_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), 0x1000, 0x10000, PT_HEAP_SIZE, 0, &zero, &zero, (void *)PT_HEAP_START);
  prints("B");
  node_heap = kheap_init((kheap_t *)(kmalloc(sizeof(kheap_t))), sizeof(node_t), 0x2000, NODE_HEAP_SIZE, 0, &zero, &zero, (void *)NODE_HEAP_START);
  prints("B");
}

node_t *make_node()
{
  prints("C");
  node_t *new_node = (node_t *)(kheap_alloc(node_heap));
  prints("C");
  new_node->pagedir = (pagedir_t *)(kheap_alloc(pagedir_heap));
  prints("C");
  new_node->esp = (int)(kheap_alloc(stack_heap));
  prints("C");
  new_node->UID = ++pid;
  return new_node;
}

node_t *load_node(rd_entry_t *to_load)
{
  prints("A");
  node_t *target = make_node();
  prints("A");
  print_hex_32(to_load->size);
  //switch_pagedir((int)(target->pagedir));
  alloc_pages_for(0, to_load->size + 0x1000, 0, 1, target->pagedir);
  prints("A");
  rd_read(to_load, 0, to_load->size, (void *)0x00);
  prints("A");
  target->eip = 0;
  prints("A");
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
  asm volatile(" \
     cli; \
     mov %0, %%ecx; \
     mov %1, %%esp; \
     mov %2, %%ebp; \
     mov $0xFEEDFACE, %%eax; \
     sti; \
     jmp *%%ecx" : : "r"(eip), "r"(esp), "r"(ebp));
  switch_pagedir(next_node->pagedir_physical_address);
}

/*extern get_EIP;

get_EIP: 
  pop eax;
  jmp eax;
*/
