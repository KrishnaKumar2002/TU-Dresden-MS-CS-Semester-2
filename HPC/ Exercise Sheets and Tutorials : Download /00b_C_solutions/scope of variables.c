#include <stdio.h>

//function "fun1"
void fun1(){
    /*local variable of function fun1, find information about it below*/
   
    // this instance of the variable x is only valid within the function.
    // No other occurrence of a variable x can interfere with it or change its value
    int x = 4;
    printf("%d\n",x); //should print 4
}


int main() {
    /*local variable of function main*/
    int x = 10; 

    /*local variable of this block*/
    {
        // as this instance of x is within a dedicated code block (see opening and closing curly braces around the declaration and printf()), it is only valid in the scope of this block
        int x = 5;
        printf("%d\n",x); //should print 5
    }
   
    
    // x from above is valid here, because no instruction changed its value within this scope
    printf("%d\n",x); //should print 10

    // fun1() uses its own version of x, which is independent from any other occurrence
    fun1(); //should print 4
}
