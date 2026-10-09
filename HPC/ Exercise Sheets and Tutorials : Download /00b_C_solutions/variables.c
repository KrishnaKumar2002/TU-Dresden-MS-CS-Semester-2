#include <stdio.h>

int main(){
    int n, i, t1 = 2, nextTerm, t2 = 1; // t1 and t2 are declared and assigned a value, all other variables do not have a defined value
    printf("Enter the number of terms: ");
    scanf("%d", &n); //scanf reads input from the command line. The "%d" defines the type of the input. The input (here: &n) must be a memory address. Here, the "&" operator gives scanf() access to the address.
    printf("Lucas Numbers: ");

    for (i=1; i<=n; ++i){
        printf("%d, ", t1);
        nextTerm = t1 + t2;
        t1 = t2;
        t2 = nextTerm;
    }

    return 0;
}
