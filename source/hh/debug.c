void bin_tree_debug_print(bin_tree_t *b)
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
}


void block_print(block_header_t *b)
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
