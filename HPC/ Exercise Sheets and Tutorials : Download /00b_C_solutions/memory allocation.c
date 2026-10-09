#include <stdio.h>
#include <stdlib.h>
 

int main()
{
    int *ptr, i , n1, n2; // *ptr is a pointer which points to some memory with one or more elements of type int
    printf("Enter size (number of desired integers to hold): ");
    scanf("%d", &n1);

    // the (int*) casts (i.e., converts), the type of the elements in that memory to int
    // technically, this is not necessary in this case, as ptr is already of type int
    // use type casting to a) make sure the memory is of the desired type, or b) if you want to reuse a pointer for another data type
    // malloc requires the size of the memory buffer in bytes.
    // instead of manually calculating that value, use the notation <number of elements> * <size of single element> for arrays
    // here, n1 elements of the size of an int each are allocated
    ptr = (int*) malloc(n1 * sizeof(int)); // allocate memory space that can hold n1 integers

    printf("Addresses of previously allocated memory: ");
    for(i = 0; i < n1; ++i)
         printf("%p\n",ptr + i);

    printf("\nEnter the new size: ");
    scanf("%d", &n2);

    ptr = realloc(ptr, n2*sizeof(int));//reallocate the memory using n2 instead of n1
    // ptr = (int*) realloc(ptr, n2*sizeof(int)); // alternate solution, see explanation above

    printf("Addresses of newly allocated memory: ");
    for(i = 0; i < n2; ++i)
         printf("%p\n", ptr + i);
 
    // freeing up used memory is vital, not just for resource efficiency
    // not freeing no longer required memory might lead to undesired results and security issues
    free(ptr);// let go of ptrs allocated memory

    return 0;
}
