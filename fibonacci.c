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


