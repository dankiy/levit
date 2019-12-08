#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <dirent.h> 
#include <stdbool.h>

#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define INF 0x3f3f3f3f

/*
    argv[1] -- data path
    argv[2] -- runs per graph
    argv[3] -- result path
*/

struct pointer_and_size 
{ 
    int **ptr;
    int size; 
};  
typedef struct pointer_and_size Graph; 

Graph read_graph(char* path)
{
    FILE *f = fopen(path, "rb");

    int size;
    fread(&size, sizeof(int), 1, f);

    int **g;
    g = malloc(sizeof(int*) * size);
    
    for (int i = 0; i < size; i++)
    {
        int weights[size];
        fread(weights, sizeof(int), size, f); 
        g[i] = malloc(sizeof(int*) * size);
        for (int j = 0; j < size; j++)
        {
            g[i][j] = weights[j];
        }
    }
    
    fclose(f); 
 
    Graph graph;
    graph.ptr = g;
    graph.size = size;
    return graph; 
}

void levit(Graph graph, int runs, char *result)
{
    int size = graph.size;
    int **g = graph.ptr;
    int s = 0;
    
    int tsum = 0;
    for (int t = 0; t < runs; t++)
    {
        clock_t start, end;
        int d[size];
        for (int i = 0; i < size; i++)
        {
            d[i] = INF;
        }
        d[s] = 0;
    
        _Bool M0[size];
        _Bool M1[size];
        _Bool M2[size];
        for (int i = 0; i < size; i++)
        {
            M0[i] = 0;
            M1[i] = 0;
            M2[i] = 1;
        }
        M2[s] = 0;
    
        struct queue 
        { 
            int *array; 
            int front;
            int rear;
            int item_count;
        }; 
        typedef struct queue Queue;
    
        void push(Queue *q, int data)
        {
            if(q->rear == size-1) 
            {
                q->rear = -1;            
            }       
            
            q->array[++q->rear] = data;
            q->item_count++;
            M1[data] = 1;
        }
    
        int pop(Queue *q)
        {
            int data = q->array[q->front++];
    	
            if (q->front == size) 
            {
                q->front = 0;
            }
            
            q->item_count--;
            M1[data] = 0;
            return data;  
        }
        
        int *M11_array;
        M11_array = malloc(sizeof(int) * size);
        int *M12_array;
        M12_array = malloc(sizeof(int) * size);
        Queue M11 = {M11_array, 0, -1, 0};
        Queue M12 = {M12_array, 0, -1, 0};
    
        push(&M11, s);
        
        
        start = clock();

        while (M11.item_count != 0 || M12.item_count != 0) 
        {
            int u = (M12.item_count == 0 ? pop(&M11) : pop(&M12)); 
  
            for (int v = 0; v < size; v++) 
            {
                if (v != u) 
                {
                    if (g[u][v] != INF) 
                    {
                        int len = d[u] + g[u][v]; 

                        if (M2[v] == 1) 
                        {
                            push(&M11, v); 
                            M2[v] = 0; 
                            d[v] = min(d[v], len); 
                        }
                        else if (M1[v] == 1) 
                        {
                            d[v] = len; 
                        }
                        else if (M0[v] == 1) 
                        {
                            if (d[v] > len) 
                            {
                                push(&M12, v); 
                                M0[v] = 0; 
                                d[v] = len; 
                            }
                        }
                    }
                    
                }
            }
            M0[u] = 1; 
        } 
        end = clock();
        tsum += end - start;
    }

    int edges = 0;
    for (int i = 0; i < size; i++)
    {
        for (int k = 0; k < size; k++)
        {
            if (i != k && g[i][k] != INF)  
                edges++;
        }
    }
    int node_len = snprintf(NULL, 0, "size: %d, edges: %d, time: %d \n", 
                                                    size, edges, tsum / runs);
    char *node = malloc(node_len + 1);
    sprintf(node, "size: %d, edges: %d, time: %d \n", size, edges, tsum / runs);
    FILE *f = fopen(result, "a");
    fwrite(node, node_len, 1, f);
    fclose(f);
}

int main(int argc, char *argv[])
{
    char dir[100];
    char *result;
    int runs;
    if (argc == 4)
    {
        sprintf(dir, argv[1]);
        runs = atoi(argv[2]);
        result = argv[3];
    }    
    else if (argc < 4)
    {
        printf("Not enough arguments\n");
        return 0;
    }
    else
    {
        printf("Too much arguments\n");
    }

    struct dirent *de;  
    DIR *dr = opendir(dir); 
  
    if (dr == NULL)  
    { 
        printf("Could not open current directory\n"); 
        return 0; 
    } 
  
    while ((de = readdir(dr)) != NULL) 
    {
        char *name = de->d_name;
        if (strstr(name, ".data") != NULL)
        {
            char path[sizeof(dir)+sizeof(name)];
            sprintf(path, "%s/", dir);
            strcat(path, name);
            Graph graph = read_graph(path);
            levit(graph, runs, result);
        }
    }
 
    closedir(dr);     
    return 0; 
}