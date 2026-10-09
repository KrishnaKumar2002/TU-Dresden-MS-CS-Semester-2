#include <stdio.h>

int main() {
  char s1[100] = "Programming ", s2[] = "is awesome";
  int length, j;

  // store length of s1 in the length variable
  length = 0;

  // do this as long as the condition in brackets is fulfilled
  while(s1[length] != '\0') {
    // increase length by 1 (short hand notation without using a "=" sign)
    // Here, prefix notation is used. Alternatively, you can use the postfix notation (see below). Note the significant difference between the two:
    // Prefix increments/decrements before the evaluation of the expression
    // Postfix evaluates the expression first, then increments/decrements
    // Search for "postfix increment" or read https://en.cppreference.com/w/c/language/operator_incdec
    ++length; 
    // length++; works as well, this is the postfix increment
  }

  // concatenate s2 to s1 using a loop
  for(j = 0; s2[j] != '\0'; ++j, ++length) { // increase j and length by 1 each at every iteration
    // access arrays by using arrayname[position], with position being an int.
    // all arrays start at 0. Elements of an array with 10 elements will be accessible if position is between 0 and 9.
    s1[length] = s2[j];
  }

  // terminate the s1 string
  s1[length] = '\0';

  printf("After concatenation: ");
  puts(s1); // for a description of puts() see man 3 puts

  return 0;
}
