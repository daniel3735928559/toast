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

//--------------//
//  INITIALIZE  //
//--------------//
void rtl8029_init(rtl8029_dev_t *dev)
{
  rtl8029_write(dev, 0, 0x21);                 // Page 0 | STOP
  rtl8029_write(dev, wDCR, 0x49);              // Always 1 | FIFO1 | FIFO0 | Send packet not done | Normal operation | Always 0 | 80x86 byte-order | Word-width DMA (should be 0x58??)
  rtl8029_write(dev, wRBCR0, 0);               // No bytes to send
  rtl8029_write(dev, wRBCR1, 0);               // No bytes to send
  rtl8029_write(dev, wRCR, 0);                 //
  rtl8029_write(dev, wTPSR, 0x20);             // Transmit page start at 0x20.  (Why?)
  rtl8029_write(dev, wTCR, 0x02);              //
  rtl8029_write(dev, wPSTART, 0x26);           //
  rtl8029_write(dev, wBNRY, 0x26);             //
  rtl8029_write(dev, wPSTOP, 0x40);            //
  rtl8029_write(dev, 0, 0x61);                 // Page 1 | STOP
  rtl8029_write(dev, wCURR, 0x26);             //
  rtl8029_write(dev, 0, 0x22);                 //
  rtl8029_write(dev, wISR, 0xFF);              //
  rtl8029_write(dev, wIMR, 0x0B);              //
  rtl8029_write(dev, wTCR, 0);                 //
}
//--------//
//  SEND  //
//--------//

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

//-----------//
//  RECEIVE  //
//-----------//
