#include <stdio.h>

// this function does not return values, therefore its return type is void
// this function does not need any arguments, therefore none are given. The empty braces "()" are still needed.
void functionName(){
    printf("I am a function");
}

// main() returns an int value on completion, therefore its type is int
int main() {
    functionName();// print "I am a function." Do not use printf or equivalent here.
    return 0;
}
