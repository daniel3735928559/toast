#include "stuff.h"
#include "datastr.c"
#include "gui/monitor.c"
#include "keybd/keyboard.c"
#include "init/paging.c"
//#include "init/kheap.c"
#include "kheap.c"
//#include "vfs/vfs.c"
#include "initrd/initrd.c"
#include "net/rtl8029.c"
#include "pci/pci.c"
//#include "ks_heap.c"
//#include "tasking/tasking.c"
#include "usermode.c"
#include "debug.c"
#include "tasking/task.c"

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
  //print_hex_32(regs.eip);
  //print_hex_32(regs.int_no);
  if(regs.int_no == 0xE)
    page_fault(regs);
  else if(regs.int_no == 0x21)
    keyboard_handler();
  else if(regs.int_no == 0xD){
    prints("GPF at ");print_hex_32(regs.eip);prints("!\n");
    asm("hlt");
  }
  else if(regs.int_no == 0x22){
    prints("A");
  }
  else if(regs.int_no == 0x23){
    prints("B");
  }
  //print_hex_32(regs.eax);
}

kheap_t *node_heap;
kheap_t *connection_heap;
kheap_t *event_heap;

//#define NODE_HEAP_START 0xD0000000
//#define CONNECTION_HEAP_START 0xD0000000
//#define EVENT_HEAP_START 0xE0000000
//#define NODE_HEAP_SIZE 0xE000000
//#define CONNECTION_HEAP_SIZE 0x10000000
//#define EVENT_HEAP_SIZE 0x8000000

//kheap_t *kheap_init(kheap_t *heap, u32 b_size, u32 s_size, u32 m_size, u32 init_slabs, u32 align, void *constr(void *), void *destr(void *), void *location){

void ks_heap_init(){
  node_heap = (kheap_t *)(kmalloc(sizeof(kheap_t)));
  kheap_init(node_heap, sizeof(node_t), align_by(sizeof(node_t), 4096), NODE_HEAP_SIZE, 0, &zero, &zero, (void *)NODE_HEAP_START);
  //kheap_init(connection_heap, sizeof(connection_t), align_by(sizeof(connection_t), 4096), CONNECTION_HEAP_SIZE, 1, 0, &zero, &zero, CONNECTION_HEAP_START);
  //kheap_init(event_heap, sizeof(event_t), align_by(sizeof(event_t), 4096), EVENT_HEAP_SIZE, 1, 0, &zero, &zero, EVENT_HEAP_START);
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
  //to_usermode();
  //tasking_init();
  //tasking_init(tss_start);
  prints("DONE!\nCONFIGURING PCI DEVICES!      ");
  pci_init();
  prints("DONE!\nBOOTING COMPLETE!");
  asm("hlt");
}

void rd_test(){
  rd_entry_t *f = rd_find("hiohi");
  if(f != NULL){
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

void tasking_test(){
  cls();
  prints("\nLoading\n");
  node_t *A = load_node(rd_find("A_printer"));
  node_t *B = load_node(rd_find("B_printer"));
  prints("Loaded\n");
  u32 i;
  for(i = 0; i < 32; i++){
    switch_node(A);
    switch_node(B);
  }
  dump_node(A);
  dump_node(B);
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

  tasking_init();
  tasking_test();

  //to_usermode();
  //asm("int $0x22");
  //to_usermode();
  //asm("int $0x22");
}
