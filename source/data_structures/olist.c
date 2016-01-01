typedef struct
{
	u32 value;
	u32 level;
	node_t *right;
	node_t *left;
} node_t

typedef struct
{
	node_t *root;
	u32 node_count;
	u32 max;
} olist_t;

void olist_insert(olist_t *list, node_t *to_insert)
{
	olist_insert_helper(list->root, to_insert, 1);
}

olist_insert_helper(node_t *root, u32 v, u32 l)
{
	if(root->level == 0)
	{
		root->value = to_insert->value;
		root->level = l;
	}
	else
	{
		olist_insert_helper((root->value < v ? root->left : root->right), v, l+1);
	}
}

olist_remove(olist_t *list, node_t *to_remove)
{
	to_remove
}

olist_create(u32 max_size)
{
	
}

int main()
{
	olist_t olist_create();
}