#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
int max_degree = 3;


typedef struct NODE
{
	int key;
	int marked;
	int degree;
	struct NODE *left;
	struct NODE *right;
	struct NODE *children;
	struct NODE *parent;
}node; //typedef, to avoid typing 'struct' everytime.


node *min; 									//Have to Initialize before insert

//Intended use: rootlist_end->right = getnode(rootlist_end);
//		rootlist_end = rootlist_end->right;
node *getnode(node *rootlist_end)
{
	node *newnode = (node*)malloc(sizeof(node)); //Creating a new node.
	newnode->left = rootlist_end; //Initializing the pointers, and degree.
	newnode->right = NULL;
	newnode->parent = NULL;
	newnode->degree = 0;
	newnode->marked = 0;
	newnode->children = NULL; //No children yet, will add space for them if needed.
	return newnode;
}


void insert(node *rootlist_end,int value){
       	rootlist_end->right=getnode(node *rootlist_end);
	rootlist_end=rootlist_end->right;
	rootlist_end->key=value;
	if(value<min->key){
		min=rootlist_end;
	}
}

int get_min(){
	return min->key;
}


int extract_min(){
	if (min == NULL)
		return -1;
	
	//Only need to make changes if minimum node has children.
	if (min->degree){
		node *child = min->children;
		while (child != NULL)
		{
			child->parent = NULL;
			child = child->right;
		}
		min->degree=0;
		if (rootlist_end != NULL)
			rootlist_end->right = min->children;
		min->children->left = rootlist_end;
		min->children = NULL;
	}


	//Interconnecting siblings of min.
	if (min->left != NULL) (min->left)->right=min->right;
	if (min->right != NULL) (min->right)->left=min->left;

	
	
	if (rootlist_end != NULL)
		while(rootlist_end->right!=NULL)
			rootlist_end = rootlist_end->right;
		
	if (rootlist_end == min)
		rootlist_end = rootlist_end->left;	
	//Storing minimum key value.
	int minimum = min->key;
	free(min);
	min = NULL;
	rootlist_end = merge_trees();
	if (rootlist_end == NULL)
		min = rootlist_end;
	min = find_new_min();
	return minimum;
}


void merge(node **smaller_add, node **larger_add)
{
	node *smaller = *smaller_add;
	node *larger = *larger_add;
	if (smaller->children == NULL) //If smaller node was childless.
	{
		smaller->children = larger;
		larger->left = NULL;
	}
	
	else	//If smaller node had children.
	{
		node *temp = smaller->children;
		//Navigating to the last child.
		while (temp->right != NULL)
			temp = temp->right;
		//Connecting larger as the next child of smaller.
		temp->right = larger;
		larger->left = temp;
	}
	larger->right = NULL;
	larger->parent = smaller;
	smaller->degree += 1;
}


//Attaches detached tree to the end of the doubly linked list: "max trees", and returns it.
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



node **insert_into_array(node **root_array, node *detached_tree, node** max_trees, int i)
{
	//If array slot is empty, simply store the detached tree there.
	if ((root_array != NULL) && (root_array[i] == NULL))
	{
		root_array[i] = detached_tree;
		return root_array;
	}
	
	//Assigning smaller and larger labels to the colliding trees, based on key values.
	node *smaller = NULL,*larger = NULL;
	compare(root_array[i], detached_tree, &smaller, &larger);
	
	merge(&smaller,&larger); //Merging trees such that smaller remains the root.
	root_array[i] = NULL; //Emptying the previously occupied slot in root_array.

	//Recursive call (degree of smaller has been updated already in merge().)
	if (smaller->degree < max_degree) insert_into_array(root_array, smaller, max_trees, smaller->degree);
	//If new tree is of maximum degree, insert into the doubly linked list "max_trees".
	else *max_trees = insert_into_max_trees(max_trees, smaller);
	return root_array;
}



//Rebuilding the rootlist.
node *rebuild_heap(node** max_trees, node **root_array)
{
	node *rootlist_end = (*max_trees); //Adding all the trees of maximum degree to the rootlist.
	for (int i = max_degree; i >= 0; i--)
	{
		if (root_array[i] == NULL) //If a slot in array is empty, move to the next slot.
			continue;
		else if (rootlist_end == NULL) //If rootlist is empty so far, add the tree from array to rootlist.
			rootlist_end = root_array[i];
		else //If rootlist was not empty, add tree to the right and move the rootlist_end pointer.
		{
			rootlist_end->right = root_array[i];
			root_array[i]->left = rootlist_end;
			rootlist_end = rootlist_end->right;
		}
	}
	return rootlist_end;
}



node *merge_trees()
{
	node *max_trees = NULL; //Pointer to store the chain of trees of maximum degree.
	node **root_array = (node**)malloc(sizeof(node*)*(max_degree + 1)); //Creating and initializing array that will store roots.
	for (int i = 0; i < max_degree + 1; i++)
		root_array[i] = NULL;
	while(rootlist_end != NULL) //Iterating through rootlist.
	{
		// Isolating/Detaching tree to be removed.
		node *detached_tree = rootlist_end;
		rootlist_end = rootlist_end->left; 
		//If there are trees left in rootlist, connections have to be cut.
		if (rootlist_end != NULL)
		{
			rootlist_end->right->left = NULL;
			rootlist_end->right = NULL;
		}
		//If rootlist_end->left is NULL, no trees are left, no connections need to be cut.
		//Inserting the detached tree into apt slot in root_array, or into max_trees.
		root_array = insert_into_array(root_array, detached_tree, &max_trees, detached_tree->degree);	
	}
	return rebuild_heap(&max_trees, root_array);
}
