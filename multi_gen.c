#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

#define INF 0x3f3f3f3f

/*
    argv[1] -- minsize
    argv[2] -- maxsize
    argv[3] -- step
    argv[4] -- density
    argv[5] -- graphs per size
    argv[6] -- seed
*/

int **graph(int size, int edges, int max_weight)
{
    int **graph;
    graph = malloc(sizeof(int*) * size);
    for(int i = 0; i < size; i++)
    {
        graph[i] = malloc(sizeof(int*) * size);
        for (int k = 0; k < size; k++)
        {
            graph[i][k] = (i == k) ? 0 : INF;
        }
    }

    int free = size * (size-1); 

    while (edges > 0)
    {
        int edge = rand() % free;

        int i = 0;
        for (; i < size; i++)
        {
            int k = 0;
            for (; k < size; k++)
            {
                if (graph[i][k] == INF)
                {
                    if (edge > 0)
                    {
                        edge -= 1;
                    }
                    else
                    {
                        graph[i][k] = rand() % max_weight + 1;
                        i = size;
                        k = size;
                    }
                }
            }
        }

        edges -= 1;
        free -= 1;
    }

    return graph;
}

int main(int argc, char *argv[])
{
    int min_size;
    int max_size;
    int step;
    float density;
    int graphs;
    int max_weight=20;
    int seed;
    if (argc < 6)
    {
        printf("Not enough arguments\n");
        return 0;
    }
    else if (argc == 6)
    {
        min_size = atoi(argv[1]);
        max_size = atoi(argv[2]);
        step = atoi(argv[3]);
        density = atof(argv[4]);
        graphs = atoi(argv[5]);
        seed = time(NULL);
    }
    else if (argc == 7)
    {
        min_size = atoi(argv[1]);
        max_size = atoi(argv[2]);
        step = atoi(argv[3]);
        density = atof(argv[4]);
        graphs = atoi(argv[5]);
        seed = atoi(argv[6]);
    }
    else
    {
        printf("Too much arguments\n");
        return 0;
    }
    srand(seed);

    if (max_size < min_size || min_size < 0 || graphs < 1 || density < 0 || density > 1)
    {
        printf("Wrong arguments\n");
        return 0;
    }
    
    char path[15];
    sprintf(path, "data/%d/", seed);
    mkdir(path, 0777);
    
    int size = min_size;
    int iname = 1;
    for (; size < max_size+step; size += step)
    {
        if (size > max_size)
        {
            size = max_size;
        }
        
        int edges = density * size * (size-1);
            
        for (int i = 1; i <= graphs; i++)
        {
            int **g = graph(size, edges, max_weight);

            char name[20];
            sprintf(name, "%d_%d_%d", size, edges, i);
            strcat(name, ".data");
            char dest[45];
            sprintf(dest, "data/%d/", seed);
            strcat(dest, name);
            
            FILE *f = fopen(dest, "wb");
            fwrite(&size, sizeof(int), 1, f);
            for (int i = 0; i < size; i++)
            {
                fwrite(g[i], sizeof(int), size, f);
            }
            fclose(f);
        }
        
    }
}
