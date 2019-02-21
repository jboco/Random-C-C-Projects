#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "header.h"
/*comments*/
/* space all entries using the space key*/
/*space key serves to push value on the stack*/
/*hit s + enter to see result*/
/*I give credit to the following website http://www.learntosolveit.com/cprogramming/sec_4.3.html 
from which my work was inspired */

int main(int argc, char *argv[])
{
  char ch; 
printf(" Enter an expression and separate each entry using the space key and hit enter:\n ");

  do{
    ch=getchar();
    decode(ch);

  }while (ch!='q');

     return 0;
}
