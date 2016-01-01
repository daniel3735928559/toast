#define RD_SIZE 64
#define FN_SIZE 16
#define DIR_SIZE 32

/*
[format:]
rd_header_t;
rd_entry_t rd_entries[]
...data...
*/

typedef struct rd_entry
{
  char name[FN_SIZE];
  //u32 is_di;r
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
  //print_hex_32(rd_data);prints(" ");print_hex_32(node->start);prints(" ");print_hex_32(data_start);prints("!\n");
  //prints((char *)(data_start));
  //prints(" hi ");print_hex_32(node->start);
  if(offset + size > node->size)
    return 1;
  u32 i = 0;
  //prints("COPYING...");
  while(i < size){
    *((char *)(buffer+i)) = *((char *)data_start + offset + i);
    i++;
  }
  //prints((char *)(data_start));
  //memcpy(rd_data + offset, buffer, size);
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
  //if((node->is_dir & 1) && index >= 0 && index < node->size){
  // u32 *dirent_indices = (u32 *)(node->start);
  //return &(rd_entries[dirent_indices[index]]);
  //}
  
  return NULL;
}
rd_entry_t *rd_finddir(rd_entry_t *node, char *name)
{
  /*u32 i;
  for(i = 0; i < nentries; i++){
    rd_entry_t *next = rd_index + i*sizeof(rd_entry_t);
    u32 j = 0;
    while(next->name[j] == name[j])
      if(next->name[++j] == 0)
	return next;
  }*/
  return NULL;
}
rd_entry_t *rd_find(char *name)
{
  u32 i;
  for(i = 0; i <= rd_num_files; i++){
    rd_entry_t *next = &(rd_entries[i]);
    u32 j = 0;
    //print_hex_32(i);prints(": ");prints(next->name);prints(" ... ");
    while(next->name[j] == name[j])
      if(name[j++] == 0) return next;
  }
  prints("NOT FOUND!");
  return NULL;
}

void rd_init(void *ramdisk)
{
  rd_data = (u32)ramdisk;
  rd_entries = (rd_entry_t *)((u32 *)ramdisk + 1);
  rd_num_files = *((u32 *)(ramdisk));
  prints(" ");print_hex_32(rd_num_files);prints(" ");
  
  //print_hex_32((u32)(rd_entries));prints(" ");
  prints(rd_entries[1].name);prints(" ");
  rd_data = (u32)(rd_entries + rd_num_files);
  //prints((char *)(0x20004));prints(" ");
  print_hex_32(sizeof(struct rd_entry));prints("! ");
  print_hex_32(rd_data);prints("! ");
  //rd_header_t *header = (rd_header_t *)(rd_data);
}

