#include <stdio.h>

int main() {    

    int number1, number2, sum;
    
    printf("Enter two integers:\n");
    // read number1 first, number2 second
    scanf("%d %d", &number1, &number2); // scanf() can read to multiple inputs and store them to multiple memory locations. Remember: of non-pointer types, the "&" operator is required to get the address

    // calculate sum
    sum = number1 + number2;      
    
    // print the result to the console
    // desired output: {number1} + {number2} = {sum}
    printf("%d + %d = %d\n", number1, number2, sum);
    
    return 0;
}
