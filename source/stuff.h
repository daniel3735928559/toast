#define NULL 0

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
