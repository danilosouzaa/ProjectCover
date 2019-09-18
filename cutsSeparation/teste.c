#include <stdlib.h>
#include <stdio.h>

void bubble_sort(int *vetor,int *pos, int n)
{
    int k, j, aux;
    for (k = 1; k < n; k++)
    {
        for (j = 0; j < n - 1; j++)
        {
            if (vetor[j] > vetor[j + 1])
            {
                aux          = vetor[j];
                vetor[j]     = vetor[j + 1];
                vetor[j + 1] = aux;
                aux = pos[j];
                pos[j] = pos[j+1];
                pos[j+1] = aux;
            }
        }
    }
}


int* sortPerViolation(int *vViolation, int nConstraints)
{
    int *pos= (int*)malloc(sizeof(int)*nConstraints);
    int i;
    for(i=0; i<nConstraints; i++)
    {
        pos[i] = i;
    }
    bubble_sort(vViolation,pos,nConstraints);
//    for(i = 0;i<nConstraints;i++){
//        printf("%d - %d\n",pos[i],vViolation[i]);
//    }
    return pos;
}
