#include <stdio.h>
#include <string.h>
 
 
// when defining a struct, the definition has to end with a ";", as this is not a scope or function, but an expression
// for more information on struct and how they manage memory, see https://en.cppreference.com/w/c/language/struct
struct Books {
   // the following variables are members of the struct
   char  title[50];
   char  author[50]; //allow 49 characters. 50 is required for the \0 string delimiter
   char  subject[100];
   int book_id; //make it an integer
};

 
int main() {

   struct Books Book1; //declare with type Books
   struct Books Book2; //declare with type Books
 
   // access struct members with the <struct>.<member> notation
   strcpy( Book1.title , "C Programming"); // set the title of Book1
   strcpy( Book1.author, "Nuha Ali");
   strcpy( Book1.subject, "C Programming Tutorial");
   Book1.book_id = 6495407; //set the id of Book1

   strcpy( Book2.title, "Telecom Billing");  // set the title of Book2
   strcpy( Book2.author, "Zara Ali");
   strcpy( Book2.subject, "Telecom Billing Tutorial");
   Book2.book_id = 6495700; //set the id of Book2
 
   // print books values
   printf( "Book 1 title : %s\n", Book1.title);
   printf( "Book 1 author : %s\n", Book1.author);
   printf( "Book 1 subject : %s\n", Book1.subject);
   printf( "Book 1 book_id : %d\n", Book1.book_id);

   printf( "Book 2 title : %s\n", Book2.title);
   printf( "Book 2 author : %s\n", Book2.author);
   printf( "Book 2 subject : %s\n", Book2.subject);
   printf( "Book 2 book_id : %d\n", Book2.book_id);

   return 0;
}
