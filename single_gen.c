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
    argv[1] -- size
    argv[2] -- min density
    argv[3] -- max density
    argv[4] -- step
    argv[5] -- graphs per density
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
                        edges -= 1;
                        free -= 1;
                    }
                }
            }
        }
    }

    return graph;
}

int main(int argc, char *argv[])
{
    int size;
    float min_density;
    float max_density;
    float step;
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
        size = atoi(argv[1]);
        min_density = atof(argv[2]);
        max_density = atof(argv[3]);
        step = atof(argv[4]);
        graphs = atoi(argv[5]);
        seed = time(NULL);
    }
    else if (argc == 7)
    {
        size = atoi(argv[1]);
        min_density = atof(argv[2]);
        max_density = atof(argv[3]);
        step = atof(argv[4]);
        graphs = atoi(argv[5]);
        seed = atoi(argv[6]);
    }
    else
    {
        printf("Too much arguments\n");
        return 0;
    }
    srand(seed);

    if (size < 1 || min_density > max_density || min_density < 0 || step <= 0 || graphs < 1)
    {
        printf("Wrong arguments\n");
        return 0;
    }
    
    char path[15];
    sprintf(path, "data/%d/", seed);
    mkdir(path, 0777);
    
    int iname = 1;
    
    for (float density = min_density; density < max_density+step; density += step)
    {
        if (density > max_density)
        {
            density = max_density;
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
