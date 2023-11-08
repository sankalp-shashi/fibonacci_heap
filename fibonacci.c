#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#define MAX_BUF_SIZE 50
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


node *min = NULL;
node *rootlist_end = NULL; 									


void print_tree(node *root)
{
	node *temp = root;
	while (temp != NULL)
	{
		node *level_finder= temp;
		while (level_finder->parent != NULL)
		{
			printf("\t");
			level_finder = level_finder->parent;
		}
		printf("%d: ",temp->key);
		node *child = temp->children;
		while (child != NULL)
		{
			printf("%d ",child->key);
			child = child->right;
		}
		printf("\n");
		print_tree(temp->children);
		temp = temp->right;
	}

}

void display(node *rootlist_end)
{
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

//To find node for decrease key.
node *search(int value, node *rootlist_end)
{
	//If element found, return its address.
	node *temp = rootlist_end;
	if (temp == NULL)
		return NULL;
	if (temp->key == value)
		return temp;
	//If element might be an internal node, check the children recursively. 
	else if ((temp->key < value) && (temp->children != NULL))
	{
		temp = temp->children;
		while (temp->right != NULL)
			temp = temp->right;
		return search(value, temp);
	}
	//Else just check the next tree.
	else
		return search(value, temp->left);
}

node* find_new_min()
{
	node *new_min = rootlist_end;
	node *temp = new_min;
	while (temp != NULL)
	{
		if (temp->key < new_min->key)
			new_min = temp;
		temp = temp->left;
	}
	return new_min;
}

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


void insert(int value){
       	if (rootlist_end == NULL){
       		rootlist_end = getnode(rootlist_end);
       		rootlist_end->key = value;
       		min=rootlist_end;
       		return;
       	}
       	rootlist_end->right=getnode(rootlist_end);
	rootlist_end=rootlist_end->right;
	rootlist_end->key=value;
	if(value<min->key){
		min=rootlist_end;
	}
	return;
}


int get_min(){
	if (min != NULL)
		return min->key;
	else
	{
		printf("Heap is empty, no element is present.\n");
		return INT_MIN;
	}
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


//Assigning smaller to the tree with smaller key, and larger to the other.
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


void cutout(node *ptrtonode){
	//Do nothing if node is already in rootlist. (In recursive call) (already checked in decrease_key().)
	if(ptrtonode->parent==NULL){
		return;
	}
	//If node is an internal node:
	else{
		//Interconnect the siblings, bypassing node to be cutout.
		if (ptrtonode->right != NULL) ptrtonode->right->left=ptrtonode->left;
		if (ptrtonode->left != NULL) ptrtonode->left->right=ptrtonode->right;
		else ptrtonode->parent->children = ptrtonode->right;

		while (rootlist_end->right !=  NULL)
			rootlist_end = rootlist_end->right;
		//Attaching node at end of rootlist, and updating end of rootlist.
		rootlist_end->right=ptrtonode;
		rootlist_end->right->left = rootlist_end;
		rootlist_end=rootlist_end->right;

		//Isolating the node being cutout.
		ptrtonode->right=NULL;
		node *temp=ptrtonode->parent; //Storing parent temporarily.
		ptrtonode->parent=NULL;


		//Marking parent if not marked already.
		temp->degree=temp->degree-1;
		if(temp->marked==0){
			temp->marked=1;
		}

		//Recursively cutting out parent if already marked.
		else if(temp->marked==1){
			temp->marked=0;
			cutout(temp);
		}
	}
}


void decrease_key(node *ptrtonode,int decreased_key){
	//If node is already in rootlist, no need to cutout.
	if (ptrtonode == NULL)
	{
		printf("Key to be decreased not found in heap.\n");
		return;
	}

	if(ptrtonode->parent==NULL){
		//No need to change min if key was already the minimum.
		if(ptrtonode==min){
			ptrtonode->key=decreased_key;
		}
		//Updating min.
		else {
			ptrtonode->key=decreased_key;
			if(ptrtonode->key<min->key){
				min=ptrtonode;
			}
		}
	}
	//If node is an internal node, cutout if min-heap property is violated.
	else if(ptrtonode->parent!=NULL){
		ptrtonode->key=decreased_key;
		if(ptrtonode->key<(ptrtonode->parent)->key){
			cutout(ptrtonode);
			if(ptrtonode->key<min->key){
				min=ptrtonode;
			}
		}
	}
}


void delete_key(int x)
{
	node *ptrtonode = search(x, rootlist_end);	//Getting the pointer to node to be deleted.
	if (ptrtonode == NULL)				//If heap is empty,
	{
		printf("Node with key %d is not present in the heap.\n", x);
		return;
	}
	
	//If node is in the rootlist.
	if (ptrtonode->parent == NULL)
	{
		//Insert the children (if present) of the node into the rootlist.
		if (ptrtonode->children != NULL){
			node *child = ptrtonode->children;
			while (child != NULL)
			{
				child->parent = NULL;
				child = child->right;
			}
			if (rootlist_end != NULL)
				rootlist_end->right = ptrtonode->children;
			ptrtonode->children->left = rootlist_end;
		}


		//Interconnecting siblings of node.
		if (ptrtonode->left != NULL) (ptrtonode->left)->right=ptrtonode->right;
		if (ptrtonode->right != NULL) (ptrtonode->right)->left=ptrtonode->left;

		
		//move rootlist_end pointer to the end of the rootlist.
		if (rootlist_end != NULL)
			while(rootlist_end->right!=NULL)
				rootlist_end = rootlist_end->right;
			
		if (rootlist_end == ptrtonode)
			rootlist_end = rootlist_end->left;	

		if (ptrtonode == min)
		{
			min = NULL;
			min = find_new_min();
		}
		free(ptrtonode);
		ptrtonode = NULL;
		printf("Successfully deleted node with key %d.\n",x);
		return;
	}
	
	if (ptrtonode->children != NULL)
	{
		node *last_child = ptrtonode->children;
		while (last_child->right != NULL)
		{
			last_child->parent = ptrtonode->parent;
			last_child = last_child->right;
		}
		last_child->parent = ptrtonode->parent;

		if (ptrtonode->left != NULL)	ptrtonode->left->right = ptrtonode->children;
		else	ptrtonode->parent->children = ptrtonode->children;

		if (ptrtonode->right != NULL)	ptrtonode->right->left = last_child;
		ptrtonode->parent->degree += ptrtonode->degree - 1;
		free(ptrtonode);
		ptrtonode = NULL;
		printf("Successfully deleted node with key %d.\n",x);
		return;
	}
	
	else
	{
		if (ptrtonode->left != NULL)	ptrtonode->left->right = ptrtonode->right;
		if (ptrtonode->right != NULL)	ptrtonode->right->left = ptrtonode->left;		
		
		if (ptrtonode->left == NULL)	ptrtonode->parent->children = ptrtonode->right;
		ptrtonode->parent->degree -= 1;
		free(ptrtonode);
		ptrtonode = NULL;
		printf("Successfully deleted node with key %d.\n",x);
		return;
	}
}


void clear_heap()
{
	while(rootlist_end->left != NULL)
	{
		rootlist_end = rootlist_end->left;
		free(rootlist_end->right);
	}
	free(rootlist_end);
	rootlist_end = NULL;
	
	printf("Heap is empty now.\n");
}


int main()
{

	printf("In display, the Heap is displayed as follows:\nLevel0\tLevel1\tLevel2...\nParent1: C1 C2...\n");
	printf("\tParent2: C1 C2...\n");
	printf("\t\tParent3: C1 C2...\n");
	printf("Where C1, C2, etc are child nodes.\n\n");
	printf("If a parent nodes are printed after tab spaces corresponding to their levels.\n");
	printf("This means, if a parent node is 2 levels below the rootlist, it will be printed after 2 tab spaces.\n");
	for (int i = 1; i <= 10; i++)
		insert(i);
		
	char input[MAX_BUF_SIZE];
	
	printf("Initial heap:\n");
	display(rootlist_end);
	printf("Would you like to clear the heap? (Enter y or n)\n");
	fgets(input, MAX_BUF_SIZE, stdin);
	if (input[0] == 'y' || input[0] == 'Y')
		clear_heap();
	while (1)
	{
		printf("\n\n\nFunctions available on the heap:\n");
		printf("1. Type 1 to insert an element into the heap.\n");
		printf("2. Type 2 to display the entire heap.\n");
		printf("3. Type 3 to print the minimum element (find_min).\n");
		printf("4. Type 4 to extract the minimum element.\n");
		printf("5. Type 5 to decrease the key of a particular element.\n");
		printf("6. Type 6 to delete a particular element.\n");
		printf("7. Type 7 to quit the program.\n");
		int control = 0;
		scanf("%d",&control);
		printf("\n\n\n");
		
		switch(control)
		{
			case 1:
			{
				int x;
				printf("Enter element to insert: ");
				scanf("%d",&x);
				insert(x);
				break;
			}
			case 2:
			{
				display(rootlist_end);
				break;
			}
			case 3:
			{
				int x = get_min();
				if (x != INT_MIN)
					printf("The minimum is %d.\n", x);	
				break;
			}
			case 4:
			{
				int x = extract_min();
				if (x != INT_MIN)
					printf("The minimum is %d, it has been removed from the heap.\n", x);
				break;
			}
			case 5:
			{
				int x,y;
				printf("Enter key to be decreased: ");
				scanf("%d", &x);
				printf("Enter new key value: ");
				scanf("%d",&y);
				decrease_key(search(x, rootlist_end), y);
				printf("key has been changed to %d.\n",y);
				break;
			}
			case 6:
			{
				int x;
				printf("Enter key to be deleted: ");
				scanf("%d", &x);
				delete_key(x);
				break;
			}
			case 7:
			{
				return 0;
			}
			default:
			{
				printf("Invalid input. Please try again\n");
			}
		}
	}
	return 0;
}
