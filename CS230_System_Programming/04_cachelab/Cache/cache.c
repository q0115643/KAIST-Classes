#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* cs20150599 Lee Ju Been*/

typedef struct block {
    char validbit;
    long tag;
    int count;
}Block;

void printCache(Block **root, int S, int E){
    int i, j;
    for(i = 0; i < S; i++){
        printf("Set %d: ", i);
        for(j = 0; j< E; j++){
            printf("%d %d\t", root[i][j].validbit, root[i][j].tag);
        }
        printf("\n");
    }
}

void access(Block **root, long smask, int E, long tagmask,
            int* miss_count, int* eviction_count, int *hit_count, int counter, int verbose_flag)
{
    int i, minlast, lru;
    int temp = E+1;
    for(i = 0; i < E; i++){
        if(root[smask][i].validbit == 0){
            temp = i; //empty space save
        }
        if(root[smask][i].validbit == 1 && root[smask][i].tag == tagmask) break;
        //search
    }
    if(i == E){
        //cache miss, load need
        if(verbose_flag) printf("miss ");
        (*miss_count)+=1;
        if(temp == E+1){ //full set
            //eviction
            minlast = root[smask][0].count;
            lru = 0;
            for(i = 0; i < E; i++){
                if(root[smask][i].count < minlast){
                    minlast = root[smask][i].count;
                    lru = i;
                }
            }
            temp = lru;
            (*eviction_count)+=1;
            if(verbose_flag) printf("eviction ");
            //evicted
        }
        root[smask][temp].tag = tagmask;
        root[smask][temp].validbit = 1;
        //loaded
    } else {
        (*hit_count)+=1; //cache hit
        if(verbose_flag) printf("hit ");
    }
    root[smask][i].count = counter;
}

int main(int argc, char* argv[])
{
    int i, j, s, b, E, S, hit_count=0, miss_count = 0, eviction_count = 0, counter = 0;
    char path[100], C[100], addr[17], operation;
    long address, bmask, smask;
    unsigned long tagmask;
    int verbose_flag = 0;

    if(argc == 9){
        s = atoi(argv[2]);
        E = atoi(argv[4]);
        b = atoi(argv[6]);
        strcpy(path, argv[8]);
    } else if (argc == 10){
        s = atoi(argv[3]);
        E = atoi(argv[5]);
        b = atoi(argv[7]);
        strcpy(path, argv[9]);
        verbose_flag = 1;
    } else {
        return 0;
    }

    S = 1<<s;
    //64bit address
    Block **root = (Block **)malloc(S*sizeof(Block));

    for(i = 0; i < S; i++){
        root[i] = (Block *)malloc(E*sizeof(Block));
        for(j = 0; j < E; j++){
            root[i][j].tag = 0;
            root[i][j].validbit = 0;
            root[i][j].count = 0;
        }
    }
    //init complete

    freopen(path, "r", stdin);
    freopen("output.txt", "w", stdout);

    while(gets(C) != 0){
        if(C[0] == ' ') i = 3;
        else continue;
        operation = C[1];
        while(C[i] != ',') i++;
        strncpy(addr, &C[3], i-3);
        addr[i-3] = 0;
        address = strtol(addr, NULL, 16);
        //addr extract

        bmask = (1<<(b))-1;
        smask = (1<<(s+b))-1-bmask;
        tagmask = -1-smask-bmask;

        smask &= address;
        smask >>=b; //now smask is set number
        bmask &= address; //now bmask is b
        tagmask &= address;
        tagmask >>= s+b; //now tagmask is tag
        if(verbose_flag) printf("%c %x ", operation, address);
        if (operation == 'M'){
            access(root, smask, E, tagmask, &miss_count, &eviction_count, &hit_count, counter, verbose_flag);
            access(root, smask, E, tagmask, &miss_count, &eviction_count, &hit_count, counter, verbose_flag);
        } else if (operation == 'L'){
            access(root, smask, E, tagmask, &miss_count, &eviction_count, &hit_count, counter, verbose_flag);
        } else if (operation == 'S'){
            access(root, smask, E, tagmask, &miss_count, &eviction_count, &hit_count, counter, verbose_flag);
        }
        counter++;
        if(verbose_flag) printf("\n");
        printCache(root, S, E);
    }
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
