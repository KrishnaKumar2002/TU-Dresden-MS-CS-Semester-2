#include <stdio.h>

// find more info about union in https://en.cppreference.com/w/c/language/union
union test {
    int x, y;
};
 
int main() {
    union test t; // a union variable t
 
    t.x = 2; // t.y also gets value 2
    printf("After making x = 2:\n x = %d, y = %d\n\n", t.x, t.y ); // call for x and y from t
 
    t.y = 10; // t.x is also updated to 10
    printf("After making y = 10:\n x = %d, y = %d\n\n", t.x , t.y ); // call for x and y from t
    return 0;
}
