#include <stdio.h>
#include <stdlib.h>
 

int main() {
    int num; //allocate integer named "num" on stack
    float *data; //allocate pointer to float named "data" on stack
    printf("Enter the total number of elements: ");

    // scanf() writes memory to an address
    // pass the memory location either as a pointer or get the memory location with the "&" operator
    scanf("%d", &num); //put in "num" in the right form (num, *num, &num, ...)

    // Allocating memory for num elements
    // data is the pointer to the memory address, at which the memory is to be allocated.
    // the calloc command allocates memory for an array. 
    // instead of calculating its size, as it would be done for malloc, pass the number of elements (here: num) and the size of the data type to calloc. see man 3 calloc
    data = (float *)calloc(num, sizeof(float)); //allocate space for user defined floating point numbers on heap //put in "data" in the right form (data, *data, &data, ...)
    if (data == NULL) {
        printf("Error!!! memory not allocated.");
        exit(0);
    }

    // Storing numbers entered by the user.
    for (int i = 0; i < num; ++i) {
        printf("Enter Number %d: ", i + 1);

        // data points to the beginning of the allocated array.
        // by using data + i, the pointer points to the ith element within the array. 
        scanf("%f", data + i); //put in "data" in the right form (data, *data, &data, ...)
    }

    // Finding the largest number
    for (int i = 1; i < num; ++i) {
        if (*data < *(data + i))
            *data = *(data + i);
    }
    // by using the "%.2f" notation limits the floating point output to two decimal places
    printf("Largest number = %.2f", * data); //put in "data" in the right form (data, *data, &data, ...)

    return 0;
}
