typedef struct node
{
	void *header;
	u32 value;
	u32 level;
	s32 balance;
	struct node *parent;
	struct node *right;
	struct node *left;
} node_t;

typedef struct
{
	u32 node_count;
	node_t *root;
	void *start;
	void *end;
	u32 max;
} bin_tree_t;

typedef struct
{
	u32 magic;
	u32 length;
	node_t *entry;
} block_header_t;

//-1 = left rotation, 1 = right
void bin_tree_rotate(bin_tree_t *tree, node_t *base, u8 dir)
{
	node_t *pivot;
	if(dir > 0)
	{
		pivot = base->left;
		if(pivot == NULL){ return; }
		base->left = pivot->right;
		pivot->right = base;
	}
	else
	{
		pivot = base->right;
		if(pivot == NULL){ return; }
		base->right = pivot->left;
		pivot->left = base;
	}
	pivot->parent = base->parent;
	base->parent = pivot;
	if(base == tree->root);
		tree->root = pivot;
}

void rebalance(bin_tree_t *tree, node_t *start, u8 is_insert)
{
	if(start->parent != NULL)
	{
		node_t *par = start->parent;
		s8 bal = (par->left == start ? -1 : 1);
		par->balance += bal;
		
		if(par->balance > 1 || par->balance < -1)
		{
			node_t *heavy = (par->balance > 1 ? par->right : par->left);
			s8 sub_bal = (heavy->balance == bal ? bal : -bal);
			if(sub_bal != bal) bin_tree_rotate(tree, heavy, -sub_bal);
			bin_tree_rotate(tree, par, -bal);
		}
		else if(!is_insert && (par->balance == 1 || par->balance == -1))
			return;
		rebalance(tree, par, is_insert);
	}
}

node_t *bin_tree_put_leaf(bin_tree_t *tree, node_t *at, u32 val, void *header)
{
	tree->node_count++;
	tree->end += sizeof(node_t);
	node_t *new_node = (node_t *)(tree->end);
	new_node->value = val;
	new_node->parent = at;
	new_node->right = NULL;
	new_node->left = NULL;
	new_node->balance = 0;
	new_node->level = at->level + 1;
	new_node->header = header;
	if(val < at->value)
	{
		at->left = new_node;
		at->balance--;
	}
	else
	{
		at->right = new_node;
		at->balance++;
	}
	return new_node;
}

node_t *bin_tree_insert_helper(bin_tree_t *tree, node_t *at, u32 val, void *header)
{
	node_t *to = at->value < val ? at->right : at->left;
	if(to == NULL)
		return bin_tree_put_leaf(tree, at, val, header);
		//rebalance(bin_tree_put_leaf(tree, at, val), 1);
	else
		return bin_tree_insert_helper(tree, to, val, header);
}

node_t *bin_tree_insert(bin_tree_t *tree, u32 val, void *header)
{
	if(!(tree->root))
	{
		tree->node_count = 1;
		tree->end += sizeof(node_t);
		tree->root = (node_t *)tree->start;
		tree->root->value = val;
		tree->root->parent = NULL;
		tree->root->right = NULL;
		tree->root->left = NULL;
		tree->root->balance = 0;
		tree->root->level = 0;
		tree->root->header = header;
		return tree->root;
	}
	else
		return bin_tree_insert_helper(tree, tree->root, val, header);
}

node_t *bin_tree_best_fit_lookup(node_t *node, u32 val)
{
	if(node == NULL)
		return NULL;
	if(node->value >= val)
	{
		node_t *ret = bin_tree_best_fit_lookup(node->left, val);
		return (ret != NULL ? ret : node);
	}
	else
		return bin_tree_best_fit_lookup(node->right, val);
}

node_t *bin_tree_exact_lookup(node_t *node, u32 val)
{
	if(node->value == val)
		return node;
	node_t *to = node->value < val ? node->right : node->left;
	return (to != NULL ? bin_tree_exact_lookup(to, val) : NULL);
}

node_t *bin_tree_lookup(bin_tree_t *tree, u32 val, u8 best_fit)
{
	return (best_fit ? bin_tree_best_fit_lookup(tree->root, val) : bin_tree_exact_lookup(tree->root, val));
}

void node_copy(bin_tree_t *tree, node_t *source, node_t *target)
{
	target->header = source->header;
	target->value = source->value;
	target->level = source->level;
	target->balance = source->balance;
	target->parent = source->parent;
	target->left = source->left;
	target->right = source->right;
	if(target->header)
		((block_header_t *)(target->header))->entry = target;
	if(tree->root == source)
		tree->root = target;
}

void bin_tree_erase(bin_tree_t *tree, node_t *to_remove)
{
	tree->node_count -= 1;
	if(to_remove != tree->end)
	{
		node_copy(tree, tree->end, to_remove);
		node_t *end = ((node_t *)(tree->end));
		if(end->parent)
		{
			node_t *n = end->parent;
			if(n->left == end) n->left = to_remove;
			if(n->right == end) n->right = to_remove;
		}
		if(end->left) end->left->parent = to_remove;
		if(end->right) end->right->parent = to_remove;
	}
	tree->end -= sizeof(node_t);
}

void bin_tree_remove(bin_tree_t *tree, node_t *to_remove)
{
	if(to_remove->left == NULL || to_remove->right == NULL)
	{
		node_t *replacement = (to_remove->left == NULL ? to_remove->right : to_remove->left);
		if(to_remove->parent == NULL)
			tree->root = replacement;
		else
		{
			if(to_remove == to_remove->parent->left)
				to_remove->parent->left = replacement;
			else
				to_remove->parent->right = replacement;
		}
		if(replacement)
			replacement->parent = to_remove->parent;
		bin_tree_erase(tree, to_remove);
	}
	else
	{
		if(to_remove->balance > 0)
		{
			node_t *temp = to_remove->right;
			while(temp->left != NULL)
				temp = temp->left;
			to_remove->value = temp->value;
			bin_tree_remove(tree, temp);
		}
		else
		{
			node_t *temp = to_remove->left;
			while(temp->right != NULL)
				temp = temp->right;
			to_remove->value = temp->value;
			bin_tree_remove(tree, temp);
		}
	}
}

bin_tree_t *bin_tree_create(u32 max_size, u32 root_val, void *header)
{
	bin_tree_t *b = (bin_tree_t *)kmalloc(sizeof(bin_tree_t));

	node_t *root = (node_t *)kmalloc(sizeof(node_t) * max_size);
	b->root = root;
	b->start = b->root;
	b->end = b->start;
	b->node_count = 1;
	b->max = max_size;

	root->value = root_val;
	root->level = 0;
	root->balance = 0;
	root->parent = NULL;
	root->left = NULL;
	root->right = NULL;
	root->header = header;
	return b;
}

