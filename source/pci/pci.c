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
  /*i=0;j=0;
  u16 VID = pci_config_read(i, j, 0, 0);
  u16 DID = pci_config_read(i, j, 0, 2);
  prints("VID: "); print_hex_32((u32)VID);
  prints("\nDID: "); print_hex_32((u32)DID);
  */
  
  prints("BUS\tSLOT\tVID\tDID\tHTYPE\tBAR0\tBAR1\tBAR2\tBAR3\tBAR4\tBAR5\n");

  for(i = 0; i < 1; i++){
    for(j = 0; j < 100; j++){

      u16 VID = pci_config_read(i, j, 0, 0);
      if(VID != 0xFFFF){
	print_hex_32(i); prints("\t"); print_hex_32(j);prints("\t");
	u16 DID = pci_config_read(i, j, 0, 2);
	print_hex_32((u32)VID);prints("\t");print_hex_32((u32)DID);prints("\t");//prints("\n");
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
