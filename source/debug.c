/*void bin_tree_debug_print(bin_tree_t *b)
{
	node_t *n = b->start;
	while(n <= (node_t *)b->end)
	{
		prints("n @ "); print_hex_32((u32)n); prints(" value="); print_hex_32(n->value); prints(" level="); print_hex_32(n->level); prints(" bal="); print_hex_32(n->balance);
		prints("\n* parent @ "); print_hex_32((u32)(n->parent)); prints(" left @ "); print_hex_32((u32)(n->left)); prints(" right @ "); print_hex_32((u32)(n->right));
		n += 1;
	}
}

void bin_tree_print_helper(node_t *n)
{
	prints("L[");
	if(n->left != NULL) bin_tree_print_helper(n->left);
	prints("] ");

	print_hex_32(n->value);

	prints(" R[");
	if(n->right != NULL) bin_tree_print_helper(n->right);
	prints("]");
}

void bin_tree_print(bin_tree_t *b)
{
	bin_tree_debug_print(b);
	prints("\n");
	bin_tree_print_helper(b->root);
	prints("\n");
}*/


/*void block_print(block_header_t *b)
{
	block_footer_t *f = get_block_footer(b);
	prints("\nHeader @ "); print_hex_32((u32)b); 
	prints("\n* Head Magic =  "); print_hex_32(b->magic); prints(" Length =  "); print_hex_32(b->length); prints(" Entry @  "); print_hex_32((u32)(b->entry)); 
	prints("\n* Footer @  "); print_hex_32((u32)f); prints(" Foot Magic =  "); print_hex_32(f->magic); prints(" Footer Header @  "); print_hex_32((u32)(f->header));
}

void heap_debug_print(heap_t *heap)
{
	prints("\n==============================BEGIN HEAP=================================\n");
	prints("======\n|HEAP|\n======\n");
	prints("Start: "); print_hex_32((u32)(heap->start)); prints(" Size: "); print_hex_32(heap->size); prints(" Index @ "); print_hex_32((u32)(heap->index)); prints("\n");
	u8 *h = (u8 *)(heap->start);
	while(h < (u8 *)((u8 *)heap->start + heap->size))
	{
		block_header_t *b = (block_header_t *)h;
		block_print(b);
		h += HEADER_SIZE + b->length;
	}

	prints("\nINDEX: \n");
	bin_tree_print(heap->index);
	prints("\n==============================END HEAP=================================\n\n");
}

void heap_test()
{
	heap_alloc(kheap, 100);
	heap_debug_print(kheap);
}
*/



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
  //bitmap_debug_print(imap->bitmap);
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
    //print_hex_32(kernel_dir->tables[0]->pages[i].frame);prints("! ");
  }
  /*for(i = 0; i < 1024; i++)
    {
    print_hex_32(kernel_dir->tables[0]->pages[i].frame);prints("! ");
    }*/
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
