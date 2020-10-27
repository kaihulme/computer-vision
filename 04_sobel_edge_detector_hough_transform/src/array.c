// Example code for allocating a 3D int array on the heap, many other more efficient solutions are possible

#include <stdio.h>
#include <stdlib.h>

int ***malloc3dArray(int dim1, int dim2, int dim3)
{
    int i, j, k;
    int ***array = (int ***) malloc(dim1 * sizeof(int **));

    for (i = 0; i < dim1; i++) {
        array[i] = (int **) malloc(dim2 * sizeof(int *));
	for (j = 0; j < dim2; j++) {
  	    array[i][j] = (int *) malloc(dim3 * sizeof(int));
	}

    }
    return array;
}

main()
{
    int ***ipppArr;
    int dim1 = 10, dim2 = 20, dim3 = 30;
    int i, j, k;

    ipppArr = malloc3dArray(dim1, dim2, dim3);

    for (i = 0; i < dim1; ++i)
	for (j = 0; j < dim2; ++j)
	    for (k = 0; k < dim3; ++k)
		ipppArr[i][j][k] = i * j * k;

    for (i = 0; i < dim1; ++i)
	for (j = 0; j < dim2; ++j)
	    for (k = 0; k < dim3; ++k)
		printf("[%d]",ipppArr[i][j][k]); 
}