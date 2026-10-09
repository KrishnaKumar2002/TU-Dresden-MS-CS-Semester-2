#include <stdio.h>

int main() {
     int i, n, sum = 0, average;

     printf("Enter number of elements: ");
     scanf("%d", &n);

     // the array "marks" has n elements of type int
     int marks[n];

     for(i=0; i<n; ++i)
     {
          printf("Enter number%d: ",i+1);
          // &marks[i] gets the memory location of the ith element of the array
          scanf("%d", &marks[i]); // save the value into the marks array using a reference
          
          // adding integers entered by the user to the sum variable
          // with the += notation, this statement is equivalent to sum = sum + marks[i];
          sum += marks[i];
     }

     average = sum/n;
     printf("Average = %d", average);

     return 0;
}
