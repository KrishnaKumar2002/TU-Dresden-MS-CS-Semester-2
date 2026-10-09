#include <stdio.h>

int main() {
   // declare a pointer to an integer and an integer
   // *pc points to a memory location that can hold one or more integeres, c is an individual integer
   int *pc, c;
   
   c = 22;
   // %p prints the addres of the memory location
   // the "&" operator gets the memory address
   printf("Address of c: %p\n", &c); 
   printf("Value of c: %d\n\n", c);  // 22
   
   // the pointer pc points to the memory address c uses for its values
   // the pc address and value are therefore identical to those of c
   pc = &c;
   printf("Address of pointer pc: %p\n", pc);
   printf("Content of pointer pc: %d\n\n", *pc); // 22
   
   // the value of c is changed.
   // since pc points to the same memory location, it reads value of c
   c = 11;
   printf("Address of pointer pc: %p\n", pc);
   printf("Content of pointer pc: %d\n\n", *pc); // 11
   
   // by using the "*" operator, the pointer is dereferenced, i.e., the value in its memory address is accessed
   // here, the value 2 is stored in the pointers's memory address 
   // as the pointer points to the memory address of c, the value of c changes, but not its address
   *pc = 2;
   printf("Address of c: %p\n", &c);
   printf("Value of c: %d\n\n", c); // 2
   return 0;
}
