//MODIFY DECREASE KEY TO INCLUDE SEARCH.
typedef struct NODE
{
	int key;
	int marked;
	int degree;
	int vertex;
	struct NODE *left;
	struct NODE *right;
	struct NODE *children;
	struct NODE *parent;
}node;


int get_min();
node* extract_min(node **rootlist_end_add, node **min_add, int max_degree);
void insert(int value, node **rootlist_end_add, node **min_add, int vertex);
void decrease_key(node *ptrtonode,int decreased_key, node **min_add, node **rootlist_end_add);


//Functions used in insert()
node *getnode(node **rootlist_end_add, int vertex);

//Utility functions to print fib heap, and search for a node (obsolete).
void print_tree(node *root);
void display(node **rootlist_end_add);
node *search(int value, node **rootlist_end_add);

//Functions used in extract_min()
void merge_trees(node **rootlist_end_add, int max_degree);
node *find_new_min(node **rootlist_end_add);
void merge(node **smaller_add, node **larger_add);
void rebuild_heap(node **rootlist_end_add, node** max_trees, node **root_array, int max_degree);
node *insert_into_max_trees(node **max_trees, node *detached_tree);
void compare(node *tree1, node *tree2, node **smaller, node **larger);
node **insert_into_array(node **root_array, node *detached_tree, node** max_trees, int i, int max_degree);

//Functions used in decrease_key()
void cutout(node *ptrtonode, node **rootlist_end_add);


void print_tree(node *root)
{
	node *temp = root;
	while (temp != NULL)
	{
		printf("%d: ",temp->vertex);
		node *child = temp->children;
		while (child != NULL)
		{
			printf("%d ",child->vertex);
			if (child == child->right)
				child->right = NULL;
			child = child->right;
		}
		printf("\n");
		if (temp != temp->children)
			print_tree(temp->children);
		if (temp == temp->right)
			temp->right = NULL;
		temp = temp->right;
	}

}


void display(node **rootlist_end_add)
{
	node *rootlist_end = *rootlist_end_add;
	node *temp = rootlist_end;
	if (temp == NULL)
		return;
	node *ptr = temp;
	while (temp->left != NULL)
	{
		temp = temp->left;
		if (temp == ptr)
			break;
	}
	print_tree(temp);
}


node *getnode(node **rootlist_end_add, int vertex)
{
	node *rootlist_end = *rootlist_end_add;
	node *newnode = (node*)malloc(sizeof(node));
	newnode->left = *rootlist_end_add; 
	newnode->right = NULL;
	newnode->parent = NULL;
	newnode->vertex = vertex;
	newnode->degree = 0;
	newnode->marked = 0;
	newnode->children = NULL;
	return newnode;
}


void insert(int value, node **rootlist_end_add, node **min_add, int vertex){
//	node *rootlist_end = *rootlist_end_add;
//	node *min = *min_add;
       	if ((*rootlist_end_add) == NULL){
       		(*rootlist_end_add) = getnode(rootlist_end_add, vertex);
       		(*rootlist_end_add)->key = value;
       		(*min_add) = (*rootlist_end_add);
       		return;
       	}
       	(*rootlist_end_add)->right=getnode(rootlist_end_add, vertex);
	(*rootlist_end_add) = (*rootlist_end_add)->right;
	(*rootlist_end_add)->key=value;
	if(value < (*min_add)->key){
		(*min_add) = (*rootlist_end_add);
	}
	return;
}

int get_min(node **min_add){
	return (*min_add)->key;
}



node* find_new_min(node **rootlist_end_add)
{
//	node *rootlist_end = *rootlist_end_add;
	node *new_min = *rootlist_end_add;
	node *temp = new_min;
	while (temp != NULL)
	{
		if (temp->key < new_min->key)
			new_min = temp;
		if (temp == temp->left)
			temp->left = NULL;
		temp = temp->left;
	}
	return new_min;
}


node *search(int value, node **rootlist_end_add)
{
	node *rootlist_end = *rootlist_end_add;
	node *temp = *rootlist_end_add;
	if (temp == NULL)
		return NULL;
	if (temp->key == value)
		return temp;
	else if ((temp->key < value) && (temp->children != NULL))
	{
		temp = temp->children;
		while (temp->right != NULL)
			temp = temp->right;
		return search(value, &temp);
	}
	else
		return search(value, &(temp->left));
}


void merge(node **smaller_add, node **larger_add)
{
	node *smaller = *smaller_add;
	node *larger = *larger_add;
	if (smaller->children == NULL) 
	{
		smaller->children = larger;
		larger->left = NULL;
	}
	
	{
		node *temp = smaller->children;
		while (temp->right != NULL)
			temp = temp->right;
		temp->right = larger;
		larger->left = temp;
	}
	larger->right = NULL;
	larger->parent = smaller;
	smaller->degree += 1;
}


node *insert_into_max_trees(node **max_trees, node *detached_tree)
{
	if (*max_trees != NULL)
	{
		while ((*max_trees)->right != NULL)
			(*max_trees) = (*max_trees)->right;
		(*max_trees)->right = detached_tree;
		detached_tree->left = (*max_trees);
	}
	return detached_tree;
}


void compare(node *tree1, node *tree2, node **smaller, node **larger)
{
	if (tree1->key <= tree2->key)
	{
		*smaller = tree1;
		*larger = tree2;
	}
	else
	{
		*smaller = tree2;
		*larger = tree1; 
	}
}


node **insert_into_array(node **root_array, node *detached_tree, node** max_trees, int i, int max_degree)
{
	if ((root_array != NULL) && (root_array[i] == NULL))
	{
		root_array[i] = detached_tree;
		return root_array;
	}
	
	node *smaller = NULL,*larger = NULL;
	compare(root_array[i], detached_tree, &smaller, &larger);
	
	merge(&smaller,&larger); 
	root_array[i] = NULL; 

	if (smaller->degree < max_degree) insert_into_array(root_array, smaller, max_trees, smaller->degree, max_degree);
	else *max_trees = insert_into_max_trees(max_trees, smaller);
	return root_array;
}


void rebuild_heap(node **rootlist_end_add, node** max_trees, node **root_array, int max_degree)
{
	node *rootlist_end = *rootlist_end_add;
	*rootlist_end_add = (*max_trees);
	for (int i = max_degree; i >= 0; i--)
	{
		if (root_array[i] == NULL) 
			continue;
		else if (*rootlist_end_add == NULL) 
			*rootlist_end_add = root_array[i];
		{
			(*rootlist_end_add)->right = root_array[i];
			root_array[i]->left = *rootlist_end_add;
			*rootlist_end_add = (*rootlist_end_add)->right;
		}
	}
}


void merge_trees(node **rootlist_end_add, int max_degree)
{
	node *rootlist_end = *rootlist_end_add;
	node **root_array = (node**)malloc(sizeof(node*)*(max_degree + 1)); 
	node *max_trees = NULL;
	for (int i = 0; i < max_degree + 1; i++)
		root_array[i] = NULL;
	while(*rootlist_end_add != NULL) 
	{
		node *detached_tree = *rootlist_end_add;
		*rootlist_end_add = (*rootlist_end_add)->left; 
		if (*rootlist_end_add != NULL)
		{
			detached_tree->left = NULL;
			(*rootlist_end_add)->right = NULL;
		}
		root_array = insert_into_array(root_array, detached_tree, &max_trees, detached_tree->degree, max_degree);	
	}
	rebuild_heap(rootlist_end_add, &max_trees, root_array, max_degree);
}


node *extract_min(node **rootlist_end_add, node **min_add, int max_degree){
	node *rootlist_end = *rootlist_end_add;
	node *min = *min_add;
	node *old_min = NULL;
	if (min == NULL)
		return NULL;
	
	if ((*min_add)->degree){
		node *child = (*min_add)->children;
		while (child != NULL)
		{
			child->parent = NULL;
			child = child->right;
		}
		(*min_add)->degree=0;
		if (*rootlist_end_add != NULL)
			(*rootlist_end_add)->right = (*min_add)->children;
		(*min_add)->children->left = (*rootlist_end_add);
		(*min_add)->children = NULL;
	}


	if ((*min_add)->left != NULL) ((*min_add)->left)->right=(*min_add)->right;
	if ((*min_add)->right != NULL) ((*min_add)->right)->left=(*min_add)->left;

	
	
	if ((*rootlist_end_add) != NULL)
		while((*rootlist_end_add)->right!=NULL)
		{
			(*rootlist_end_add) = (*rootlist_end_add)->right;
			if ((*rootlist_end_add)->right != NULL && (*rootlist_end_add)->right->right != NULL)
				if ((*rootlist_end_add)->right == (*rootlist_end_add)->right->right)
					(*rootlist_end_add)->right = NULL;
		}	
		
	if ((*rootlist_end_add) == (*min_add))
	{
		(*rootlist_end_add) = (*rootlist_end_add)->left;	
		if ((*rootlist_end_add) != NULL)
			(*rootlist_end_add)->right = NULL;
	}
	old_min = (*min_add);
	merge_trees(rootlist_end_add, max_degree);
	if ((*rootlist_end_add) == NULL)
		(*min_add) = (*rootlist_end_add);
	(*min_add) = find_new_min(rootlist_end_add);
	return old_min;
}


void cutout(node *ptrtonode, node **rootlist_end_add){
	node *rootlist_end = *rootlist_end_add;
	if(ptrtonode->parent==NULL){
		return;
	}
	else{
		if (ptrtonode->right != NULL) ptrtonode->right->left=ptrtonode->left;
		if (ptrtonode->left != NULL) ptrtonode->left->right=ptrtonode->right;
		else ptrtonode->parent->children = ptrtonode->right;
		
		while ((*rootlist_end_add)->right !=  NULL)
		{
			(*rootlist_end_add) = (*rootlist_end_add)->right;
			if ((*rootlist_end_add)->right == (*rootlist_end_add))
				(*rootlist_end_add)->right  = NULL;
		}
		(*rootlist_end_add)->right=ptrtonode;
		(*rootlist_end_add)->right->left = (*rootlist_end_add);
		(*rootlist_end_add)=(*rootlist_end_add)->right;
		
		ptrtonode->right=NULL;
		node *temp=ptrtonode->parent;
		ptrtonode->parent=NULL;
		
		
		temp->degree=temp->degree-1;
		if(temp->marked==0){
			temp->marked=1;
		}
		
		else if(temp->marked==1){
			temp->marked=0;
			cutout(temp, rootlist_end_add);
		}
	}
}


void decrease_key(node *ptrtonode,int decreased_key, node **min_add, node **rootlist_end_add){
	node *min = *min_add;
	node *rootlist_end = *rootlist_end_add;
	if (ptrtonode == NULL){
		printf("key to be decreased not found in heap.\n");
		return;
	}
	
	if(ptrtonode->parent==NULL){
		if(ptrtonode==(*min_add)){
			ptrtonode->key=decreased_key;
		}
		else {
			ptrtonode->key=decreased_key;
			if(ptrtonode->key<(*min_add)->key){
				(*min_add)=ptrtonode;
			}
		}
	}
	else if(ptrtonode->parent!=NULL){
		ptrtonode->key=decreased_key;
		if(ptrtonode->key<(ptrtonode->parent)->key){
			cutout(ptrtonode, &rootlist_end);
			if(ptrtonode->key<(*min_add)->key){
				(*min_add)=ptrtonode;
			}
		}
	}
}
