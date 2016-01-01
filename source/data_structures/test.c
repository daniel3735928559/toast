#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef char s8;

#include "util.c"
/*#include "stack.c"
#include "bitmap.c"
#include "imap.c"*/
#include "bin_tree.c"
#include "heap.c"

int main(char argc, char **argv)
{
	/*
	u32 x = 87;
	bit_print(x);
	bit_print(x & -x);
	printf("%d\n",get_first_set(x));
	printf("%d\n",get_first_set_alt(x));
	//*/
	
	/*bitmap_t *b = bitmap_create(640);
	bitmap_print(b);
	u32 i;
	//*/
	/*
	for(i = 0; i < 640; i++)
	{
		bitmap_set(b, i);
	}
	bitmap_clear(b, 3);
	bitmap_clear(b, 7);
	bitmap_clear(b, 10);
	bitmap_clear(b, 11);
	bitmap_clear(b, 12);
	bitmap_clear(b, 13);
	bitmap_clear(b, 14);
	bitmap_clear(b, 33);
	bitmap_clear(b, 73);
	bitmap_print(b);
	bit_print(bitmap_get_context(b, 3));
	bit_print(bitmap_get_context(b, 7));
	bit_print(bitmap_get_context(b, 33));
	bit_print(bitmap_get_context(b, 73));
	printf("%d-%d-%d-%d\n",	 	~bitmap_get_context(b, 3), 
										~bitmap_get_context(b, 7), 
										~bitmap_get_context(b, 33), 
										~bitmap_get_context(b, 73));
	//*/

	/*stack_t *s = stack_create(100);\
	printf("%d,%d\n", s->top, s->base);
	printf("push: %d\n", stack_push(s, 10));
	printf("push: %d\n", stack_push(s, 20));
	printf("pop: %d\n", stack_pop(s));
	printf("pop: %d\n", stack_pop(s));*/
	/*
	imap_t *imap = imap_create(64);
	for(i = 0; i < 80; i++)
	{
		printf("%d:%d:%d\n\n", i, imap_set_next(imap), stack_peek(imap->stack));
		bitmap_print(imap->bitmap);
	}
	printf("%d\n", imap_set_next(imap));
	for(i = 50; i < 100; i++)
	{
		printf("CLEAR %d\n", i);
		imap_clear(imap, i);
		bitmap_print(imap->bitmap);
	}
	imap_clear(imap, 6);
	imap_clear(imap, 9);
	for(i = 0; i < 80; i++)
	{
		printf("%d:%d:%d\n\n", i, imap_set_next(imap), stack_peek(imap->stack));
		bitmap_print(imap->bitmap);
	}
	//*/

	/*
	bit_print(134929);
	stack_t *s = stack_create(64);
	for(i = 0; i < 64; i++)
	{
		stack_push(s, i);
		printf("%d\n", stack_peek(s));
		stack_print(s);
	}
	stack_print(s);
	//*/
	//*
	bin_tree_t *tree = bin_tree_create(100, 23, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 25, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 2, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 11, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 13, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 31, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 3, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 10, 0);
	bin_tree_print(tree);
	bin_tree_insert(tree, 19, 0);
	bin_tree_print(tree);
	bin_tree_rotate(tree, tree->root, 1);
	bin_tree_print(tree);
	node_t *n = bin_tree_lookup(tree, 2, 1);
	if(n != NULL)
	{
		node_print(n);
		printf("REMOVING\n");
		bin_tree_remove(tree, n);
	}
	bin_tree_print(tree);
	n = bin_tree_lookup(tree, 10, 1);
	if(n != NULL)
	{
		node_print(n);
		printf("REMOVING\n");
		bin_tree_remove(tree, n);
	}
	bin_tree_print(tree);
	n = bin_tree_lookup(tree, 13, 0);
	if(n != NULL)
	{
		node_print(n);
		printf("REMOVING\n");
		bin_tree_remove(tree, n);
	}
	bin_tree_print(tree);
	//*/
	//*
	heap_t *h = heap_create(INIT_HEAP_SIZE);
	heap_debug_print(h);
	heap_alloc(h, 0x100);
	heap_debug_print(h);
	void *s1 = heap_alloc(h, 0x303);
	heap_debug_print(h);
	void *s2 = heap_alloc(h, 0x302);
	heap_debug_print(h);
	void *s = heap_alloc(h, 0x999);
	heap_debug_print(h);
	void *s3 = heap_alloc(h, 0x8);
	heap_debug_print(h);
	//printf("%d\n", s);
	heap_free(h, s);
	heap_debug_print(h);
	//printf("%d\n", s1);
	heap_free(h, s1);
	heap_debug_print(h);
	//printf("%d\n", s2);
	heap_free(h, s2);
	heap_debug_print(h);
	//printf("%d\n", s3);
	heap_free(h, s3);
	heap_debug_print(h);
	printf("Done\n");
	//*/
}
