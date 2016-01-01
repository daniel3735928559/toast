#define STACK_START 0xE0000000
#define STACK_SIZE 0x2000

typedef struct task{
  u32 id;
  u32 esp;
  u32 kesp;
  u32 ebp;
  u32 eip;
  pagedir_t *pagedir;
  struct task *next;
} task_t;

volatile u32 current_pid = 0;
volatile task_t *current_task;
volatile task_t *ready_queue;

pagedir_t *new_pd()
{
  u32 phys;
  pagedir_t *new_dir = (pagedir_t *)kmalloc_ap(sizeof(pagedir_t), &phys);
  memset(new_dir, 0, sizeof(pagedir_t));
  u32 offset = (u32)(new_dir->table_phys_addresses) - (u32)(new_dir);
  new_dir->physical_address = phys + offset;
  u32 i;
  for(i = 0; i < 1024; i++){
    if(kernel_dir->tables[i] != 0){
      new_dir->tables[i] = kernel_dir->tables[i];
      new_dir->table_phys_addresses[i] = kernel_dir->table_phys_addresses[i];
    }
  }
  return new_dir;
}

void TLB_flush()
{
  u32 pd_addr;
  asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
  asm volatile("mov %0, %%cr3" : : "r" (pd_addr));
}

u32 get_PID()
{
  return ++current_pid;
}

void new_stack(u32 *start, u32 size)
{
  u32 i = 0;
  for(i = start; i >= start - size; i -= 0x1000){
    get_a_frame(get_page(start + i, 1, current_dir), 1, 1);
  }
  TLB_flush();
  asm volatile("mov %0, %%esp" : : "r" (start));
  asm volatile("mov %0, %%ebp" : : "r" (start));
}

u32 load_task(rd_entry_t *file)
{
  pagedir_t *pd = new_pd();
  u32 i = 0;
  while(pd->tables[i]) i++;
  u32 start = i;
  u32 j;
  for(j = 0; j < file->size; j++)
    {
      if(!(j % 1024)){
	i++;
	pd->tables[i] = (page_table_t *)kmalloc_ap(sizeof(page_table_t), &(pd->tables[i]));
      }
      get_a_frame(&((pd->tables[i])->pages[j%1024]), 1, 1);
      rd_read(file, j*0x1000, (j+1)*0x1000, pd->tables[i]->pages[j%1024].frame);
    }
  task_t *new_task = kmalloc(sizeof(task_t));
  new_task->id = get_PID();
  new_task->pagedir = pd;
  new_task->next = current_task->next;
  current_task->next = new_task;
  new_task->esp = STACK_START;
  new_task->ebp = STACK_START;
  new_stack(STACK_START, STACK_SIZE);
  new_task->eip = start*0x1000;
}

void tasking_init()
{
  asm volatile("cli");
  
  //move_stack((void*)0xE0000000, 0x2000);
  
  current_task = ready_queue = (task_t*)kmalloc(sizeof(task_t));
  current_task->id = get_PID();
  current_task->esp = current_task->ebp = 0;
  current_task->eip = 0;
  current_task->pagedir = current_dir;
  current_task->next = 0;
  
  asm volatile("sti");
}

void advance_task()
{
  if(!current_task) return;
  u32 esp, ebp, eip;
  asm volatile("mov %%esp, %0" : "=r"(esp));
  asm volatile("mov %%ebp, %0" : "=r"(ebp));
  eip = read_eip();
  if(eip == 0xdeadbeef) return;
  current_task->eip = eip;
  current_task->ebp = ebp;
  current_task->esp = esp;
  current_task = current_task->next;
  if(!current_task) current_task = ready_queue;
  switch_pagedir(current_task->pagedir);
  ebp = current_task->ebp;
  esp = current_task->esp;
  asm volatile(" \
	cli;	       \
	mov %0, %%ecx; \
	mov %1, %%esp; \
	mov %2, %%ebp; \
	mov %3, %%cr3;		\
	mov $0xdeadbeef, %%eax; \
	sti;			\
	jmp *%%ecx"
	       : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_dir->physical_address));
}
