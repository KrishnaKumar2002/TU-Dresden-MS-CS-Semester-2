#include <stdio.h>

// function "print_values"
void print_values(int a, char b, float c, double d) { // functions must be given the type that their return value has. Here, nothing is returned, therefore this function is of type "void". The type of each parameter has to be specified.
   printf("a's value : %d, size: %lu\n",a, sizeof(a)); // sizeof() returns a value of type size_t, which is equivalent to long unsigned (%lu)
   printf("b's value : %c, size: %lu\n",b, sizeof(b));
   printf("c's value : %.8lg, size: %lu\n",c, sizeof(c));
   printf("d's value : %.13lg, size: %lu\n",d, sizeof(d));
}

// function "main"
int main() {
   // types can be derived from both the input value ("S" is no double) and the required output (see comments below) 
   int a = 10;
   char b = 'S';
   float c = 2.1234567;
   double d = 28.123456789;
   
   print_values(a, b, c, d); // call function and pass values to it. These values must have the same type, but not the same name as those in the function definition
   
   /*  you should see this:
   
    a's value : 10, size: 4
    b's value : S, size: 1
    c's value : 2.1234567, size: 4
    d's value : 28.123456789, size: 8
    
   */
   return 0;
}
