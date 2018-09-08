//Madison Williams HW4
#include <stdio.h>
#include <stdlib.h>
#include<ctype.h>
#include <limits.h>
#include "input_error.h"

struct node{
    int vertex;
    int distance;
    int weight;
    struct node* next;
    };
struct q{
    int key;
    struct q* nextq;
};

struct Graph{
    int numVerticies;
    struct node** adjLists;
};

struct HeapNode{
    int  v;
    int distance;
};
 
struct MinHeap{
    int size;      
    int capacity; 
    int *position;     
    struct HeapNode **array;
};

struct node* createNode(int, int);
struct Graph* createGraph(int verticies);
void addEdge(struct Graph* graph, int v1, int v2, int w);
void printGraph(struct Graph* graph, FILE* fp2);
struct HeapNode* addMinheapNode(int v, int distance);
struct MinHeap* makeMinHeap(int total);
void swapNodes(struct HeapNode** x, struct HeapNode** y);
void minHeapify(struct MinHeap* minHeap, int index);
int isEmpty(struct MinHeap* minHeap);
struct HeapNode* extractMin(struct MinHeap* minHeap);
void decreaseKey(struct MinHeap* minHeap, int v, int distance);
int inHeap(struct MinHeap *minHeap, int v);
void printArr(int distance[], int n, FILE* fp);
void dijkstra(struct Graph* graph, int source, FILE* fptr);
void parse_file(FILE* fp, FILE* fp2);

int main(int argc, char** argv) {
    if (argc != 3)
         exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS); 
    FILE* fpin = fopen(argv[1], "r");
    if (!fpin)
        exit(INPUT_FILE_FAILED_TO_OPEN);
    FILE* fpout = fopen(argv[2], "w");
    if (!fpout)
        exit(OUTPUT_FILE_FAILED_TO_OPEN);  
    parse_file(fpin, fpout);
    if (fclose(fpin) == EOF)
        exit(INPUT_FILE_FAILED_TO_CLOSE);
    return 0;
    
}

void parse_file(FILE* fp, FILE* fp2)
{    
    if (feof(fp))
    {exit(PARSING_ERROR_EMPTY_INPUT_FILE);}
    //get number of total verticies
    int total;
    fscanf(fp,"%d\n", &total);
    //printf("Number of verticies: %d \n", total);
    int v1, v2, w;
    struct Graph* graph = createGraph(total);
	while (fscanf(fp, "(%d,%d,%d)\n", &v1, &v2, &w) == 3)//scan in verticies and add their edge
    {    
         if(v1<0 || v1>total || v2<0 ||v1>total)
         {
             exit(INTEGER_IS_NOT_A_VERTEX);
         }
        addEdge(graph, v1, v2, w);
    } 
     if (!feof(fp))
    {exit(PARSING_ERROR_INVALID_FORMAT);}
    //fclose(fp);
    printGraph(graph, fp2);
 }   
    struct node* createNode(int v, int w)
    {
        struct node* newNode= malloc(sizeof(struct node));
        newNode->vertex=v;
        newNode->distance=-1;
        newNode->weight=w;
        newNode->next=NULL;
        return newNode;
    }

    struct Graph* createGraph(int verticies){
        struct Graph* graph = malloc(sizeof(struct Graph));
        graph->numVerticies= verticies;
        graph->adjLists = malloc(verticies* (sizeof(struct node)));
        int i;
        for(i=0; i<verticies;i++)
        {
         graph->adjLists[i]=malloc(sizeof(struct node));
         graph->adjLists[i]->vertex=i+1;
         graph->adjLists[i]->distance=-1;
         graph->adjLists[i]->weight=0;
         graph->adjLists[i]->next=NULL;
        }
        return graph;
    }

    void addEdge(struct Graph* graph, int v1, int v2, int w){        
        struct node* temp= graph->adjLists[v1-1];
        while(temp->next!=NULL)
        {
            temp=temp->next;
        }
        temp->next=createNode(v2, w);
    }

    void printGraph(struct Graph* graph, FILE* fpout){
       dijkstra(graph, 1, fpout);
       //free adj list
       int i;
       for(i=0; i<graph->numVerticies; i++)
       {
           struct node* temp=graph->adjLists[i];
           while(temp!=NULL)
           {
                struct node* hold=temp;
                temp = temp->next;
                free(hold);
           }
       }
       
       free(graph->adjLists);
       free(graph);
       fclose(fpout);
    }

struct HeapNode* addMinheapNode(int v,int distance)
{
    struct HeapNode* minHeapNode = (struct HeapNode*) malloc(sizeof(struct HeapNode));
    minHeapNode->distance = distance;
    minHeapNode->v = v;
    return minHeapNode;
}
 
struct MinHeap* makeMinHeap(int total)
{
    struct MinHeap* minHeap =malloc(sizeof(struct MinHeap));
    minHeap->position = malloc(total * sizeof(int));
    minHeap->capacity = total;
    minHeap->size = 0;
    minHeap->array =malloc(total * sizeof(struct HeapNode*));
    return minHeap;
}
 
void swapNodes(struct HeapNode** x, struct HeapNode** y)
{   struct HeapNode* z = *x;
    *x=*y;
    *y=z;
}
struct HeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;
    struct HeapNode* root=minHeap->array[0];
    struct HeapNode* lastNode=minHeap->array[minHeap->size-1];
    minHeap->array[0]=lastNode;
    minHeap->position[root->v]=minHeap->size-1;
    minHeap->position[lastNode->v]=0;
    minHeap->size-=1;
    minHeapify(minHeap,0);
 
    return root;
}
 
void minHeapify(struct MinHeap* minHeap, int index)
{
    int smallest,left,right;
    smallest=index;
    left=2*index+1;
    right=2*index+2;
    if (left<minHeap->size && minHeap->array[left]->distance <minHeap->array[smallest]->distance)
      smallest=left;
    if (right<minHeap->size &&minHeap->array[right]->distance<minHeap->array[smallest]->distance)
      smallest=right;
 
    if (smallest!=index)
    {
        struct HeapNode *smallestNode=minHeap->array[smallest];
        struct HeapNode *idxNode=minHeap->array[index];
        minHeap->position[smallestNode->v]=index;
        minHeap->position[idxNode->v]=smallest;
        swapNodes(&minHeap->array[smallest], &minHeap->array[index]);
        minHeapify(minHeap,smallest);
    }
}

void decreaseKey(struct MinHeap* minHeap, int v, int distance)
{
    int i=minHeap->position[v];
    minHeap->array[i]->distance=distance;
 
    while (i!=0 && minHeap->array[i]->distance < minHeap->array[(i-1)/2]->distance)
    {
        // Swap nodes
        minHeap->position[minHeap->array[i]->v]=(i-1)/2;
        minHeap->position[minHeap->array[(i-1)/2]->v]=i;
        swapNodes(&minHeap->array[i], &minHeap->array[(i-1)/2]);
        i = (i-1)/2;
    }
}

int inHeap(struct MinHeap *minHeap,int x)
{
   if (minHeap->position[x]<minHeap->size)
     return 1;
   return 0;
}

int isEmpty(struct MinHeap* minHeap)
{   if(minHeap->size == 0)
    {return 1;}
    return 0;
}

void printArr(int distance[],int n, FILE* fp)
{
    int i;
    for (i=0; i<n; i++)
        if(distance[i]==INT_MAX){
        fprintf(fp, "%d\n",-1);}
        else{
        fprintf(fp, "%d\n",distance[i]);}

}

void dijkstra(struct Graph* graph,int source, FILE* fptr)
{
    int V =graph->numVerticies;
    int distance[V];
    struct MinHeap* minHeap=makeMinHeap(V);

    int v;
    for (v=0;v<V;v++)
    {
        distance[v]=INT_MAX;
        minHeap->array[v] =addMinheapNode(v, distance[v]);
        minHeap->position[v]=v;
    }
    //1's distance from 1 is 0
    minHeap->array[source-1]=addMinheapNode(source, distance[source-1]);
    minHeap->position[source-1]= source;
    distance[source-1]=0;
    decreaseKey(minHeap,source,distance[source-1]);
    minHeap->size = V;

    while (isEmpty(minHeap)!=1)
    {
        struct HeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; 
        struct node* temp =graph->adjLists[u-1];
        while (temp!= NULL)
        {
            int v = temp->vertex;
            if (inHeap(minHeap, v)&&distance[u-1]!=INT_MAX && temp->weight+distance[u-1] <distance[v-1])
            {
                distance[v-1]= distance[u-1]+temp->weight;
                decreaseKey(minHeap,v,distance[v-1]);
            }
            temp=temp->next;
        }
    }

    printArr(distance,V, fptr);

    //free memory
    int i;
    for(i=0; i<V; i++){
        free(minHeap->array[i]);
    }
    free(minHeap->position);
    free(minHeap->array);
    free(minHeap);
}
    
    

