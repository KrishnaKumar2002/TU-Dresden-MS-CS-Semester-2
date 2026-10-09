#include <stdio.h>

int main() {
    FILE *fp;
    int c;
   
    // open the current input file
    // __FILE__ is a predefined macro that returns the name of the current input file (i.e. the source code)
    // the second argument for fopen(), (here: "r"), describes how the file will be handled. Here, "r" opens the file for reading it. Find other parameters in the man page (man 3 fopen)
    fp = fopen(__FILE__,"r");

    do {
         c = getc(fp);   // read character
         // c = fgetc(fp); works as well. getc() is an equivalent to fgetc(). See man 3 getc
         putchar(c);     // display character
    }
    while(c != EOF);  // loop until the end of file is reached
    
    fclose(fp); //close the file
    return 0;
}
