#include <stdio.h>

int main() {
    char operator;
    double n1, n2;

    printf("Enter an operator (+, -, *, /): ");
    scanf("%c", &operator);
    printf("Enter two operands: ");
    scanf("%lf %lf",&n1, &n2);

    int result = 0;
    // depending on the value of "operator", do different things
    switch(operator)
    {
        // do this if "operator" is "+"
        case '+':
            printf("%.1lf + %.1lf = %.1lf",n1, n2, n1+n2);
            // leave the block
            break;
        
        // do this if "operator" is "-"
        case '-':
            printf("%.1lf - %.1lf = %.1lf",n1, n2, n1-n2);
            // leave the block
            break;

        // do this if "operator" is "*"
        case '*':
            printf("%.1lf * %.1lf = %.1lf",n1, n2, n1*n2);
            // leave the block
            break;

        // do this if "operator" is "/"
        case '/':
            printf("%.1lf / %.1lf = %.1lf",n1, n2, n1/n2);
            // leave the block
            break;

        // operator doesn't match any case constant +, -, *, /
        default:
            result = 1;
    }

    // check a condition
    if(result == 0) {
        printf("Success! Run again to try a different set of parameters");
    }
    // if the condition is not satisfied, do this
    else
    {
        printf("Error! operator is not correct");
    }

    return result;
}
