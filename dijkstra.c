#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include "functions.c"
#define MAX_BUF_SIZE 50



//Structure of vertex nodes.
typedef struct VERTEX
{
	int value;		//Value/name of the vertex.
	int min_distance;	//Value of min distance to source.
	int weight;
	struct VERTEX *next;	//Next vertex (in adjacency list).
}vert;


int max_degree = 5;


typedef struct fheap
{
	node *rootlist_end;
	node *min;
}FIBHEAP;


vert **adjacency_list;
int n;


//Function to create and initialize a node.
vert *getvert(int v, int w)
{
	vert *newvert = (vert*)malloc(sizeof(vert));
	newvert->value = v;
	newvert->weight = w;
	newvert->min_distance = INT_MAX;
	newvert->next = NULL;
	return newvert;
}


//Function to insert an edge into the adjacency list.
void insert_edge(int u, int v, int w)
{
	vert *temp = adjacency_list[u];
	while (temp->next != NULL)
		temp = temp->next;
	temp->next = getvert(v, w);
}


//Function that takes the edges as user input.
void edge_input(char U_or_D)
{
	//Input string to take user input.
	char input_string[MAX_BUF_SIZE];
	while(1)
	{
		fgets(input_string, MAX_BUF_SIZE ,stdin);
		
		//Checking if user entered 'Q'.
		if (input_string[0] == 'Q')
			return;
		if (input_string[0] == '\0')
			continue;
		//Defining substrings to hold the numbers entered.
		char num1[MAX_BUF_SIZE],num2[MAX_BUF_SIZE],weight[MAX_BUF_SIZE];
		int i,j,k,u,v,w;
		for (i = 0; i < MAX_BUF_SIZE; i++)
		{
			if (input_string[i] == ' ')
				break;
			num1[i] = input_string[i];
		}
		num1[i] = '\0'; //Null terminating the first substring.
		
		i++;
		for (j = 0; j < MAX_BUF_SIZE && i < MAX_BUF_SIZE; j++, i++)
		{
			num2[j] = input_string[i];
			if (input_string[i] == ' ')
				break;
		}
		for (k = 0; k < MAX_BUF_SIZE && i < MAX_BUF_SIZE; k++, i++)
		{
			weight[k] = input_string[i];
			if (input_string[i] == '\0')
				break;			
		}
		//Converting the substrings to integers using inbuilt atoi().
		u = atoi(num1);
		v = atoi(num2);
		w = atoi(weight);
		
		//Ignoring loops, i.e. edges from a vertex to itself.
		if (u == v)
			continue;
		insert_edge(u,v,w);
		
		//Inserting the reverse edge too for undirected graphs.
		if (U_or_D == 'U')
			insert_edge(v,u,w);
	}
}


//Function to create and initialize the adjacency list.
void create_list()
{
	adjacency_list = (vert**)malloc(sizeof(vert*)*n);
	for (int i = 0; i < n; i++)
		adjacency_list[i] = getvert(i,0);
}


//Function to display the adjacency list.
void print_graph()
{
	for (int i = 0; i < n; i++)
	{
		printf("Vertex %d: ",i);
		vert *temp = adjacency_list[i];
		while (1)
		{
			temp = temp->next;
			if (temp == NULL || temp->next == NULL)
				break;
			printf("%d,%d->",temp->value,temp->weight);
		}
		if (temp != NULL && temp->value != i)
			printf("%d,%d\n",temp->value,temp->weight);
		else printf("\n");
	}
	printf("\n");
}


void dijkstra(int src)
{
//	FIBHEAP F;
//	F.rootlist_end = NULL;
//	F.min = NULL;
	node *rootlist_end = NULL, *min = NULL;
	node *node_array[n];
	insert(0, &rootlist_end, &min, src);
	adjacency_list[src]->min_distance = 0;
	node_array[src] = rootlist_end;

	for (int i = 0; i < n; i++)
	{
		if (i == src)
			continue;
		insert(INT_MAX, &rootlist_end, &min, i);
		adjacency_list[i]->min_distance = INT_MAX;
		node_array[i] = rootlist_end;
	}

	while (min != NULL)
	{
		node *min_vertex = extract_min(&rootlist_end, &min, max_degree);
		int u = min_vertex->vertex;
		vert *temp = adjacency_list[u]->next;
		while (temp != NULL)
		{
			int v = temp->value;
			int w = temp->weight;
			int uv = adjacency_list[u]->min_distance + w;
			if (uv < adjacency_list[v]->min_distance)
			{
				adjacency_list[v]->min_distance = uv;
				decrease_key(node_array[v], uv, &min, &rootlist_end);
			}
			temp = temp->next;
		}
	}
	
	printf("Vertex   Distance from Source\n");
	for (int i = 0; i < n; i++)
		printf("%d\t%d\n", i, adjacency_list[i]->min_distance);

}


int main()
{
	printf("Enter the number of vertices: ");
	scanf("%d",&n);

	//Initializing adjacency list and edge matrix.
	create_list();
	
	//Taking graph as input and printing the adjacency list.
	
	printf("Your vertices are numbered from 0 to %d\n",n-1);
	printf("Enter D for directed or U for undirected graph: ");
	char U_or_D;
	
	//Getting rid of newlines present in input buffer from previous input.
	U_or_D = getchar();
	while (U_or_D == '\n')
		U_or_D = getchar();
		
	//Taking edge input.
	printf("Enter your edges in the form of vertices separated by a space.\n");
	edge_input(U_or_D);
	
	//Printing the adjacency list.
	printf("\nProblem 1a:");
	if (U_or_D == 'U')
		printf("\nYour undirected graph is:\n");
	else
		printf("\nYour directed graph is:\n");
	print_graph();
	
	int src = 0;
	printf("\nEnter a source vertex to perform djikstra's algorithm on: ");
	scanf("%d",&src);
	dijkstra(src);
}
