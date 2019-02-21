#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#define size 100
int stack[size];
int sp=0;
char num[8];
int ind=0;


int pop(void)
{
    int ret;
    sp--;
    if (sp<0)
    {
        sp++;
        ret=0;
        printf("Error, Empty Stack");
    }
    else
    {
        ret=stack[sp];
        stack[sp]=0;
    }
    return ret;
}


void push(int num)
{
    if(sp<size)
    {
        stack[sp]=num;
        sp++;
    }
    else
    {
        printf("Error, Stack Overflow");
    }
}

void decode(char ch)
{

    int i;
    int x,y;
    if(ch>='0' && ch<='9')
    {
        num[ind]=ch;
        ind++;
    }
    else if (ch=='A' || ch=='B' || ch=='C' || ch=='D' || ch=='E' || ch=='D')
    {
        switch(ch)
        {
            case 'A': num[ind]='A'; ind++; break;
            case 'B': num[ind]='B'; ind++; break;
            case 'C': num[ind]='C'; ind++; break;
            case 'D': num[ind]='D'; ind++; break;
            case 'E': num[ind]='E'; ind++; break;
            case 'F': num[ind]='F'; ind++; break;
        }
    }
    else if(ch=='&')
    {
     x=pop();
     y=pop();
     push(x&y);
    }
    else if (ch=='|')
    {
        x=pop();
        y=pop();
        push(x|y);
    }
    else if(ch=='^')
    {
         x=pop();
        y=pop();
        push(x^y);
    }
    else if(ch=='~')
    {
         x=pop();

        push(~x);
    }
    else if((isspace(ch)) && ind!=0)
    {
        push(strtol(num, NULL, 16));
        for (i=0; i<ind; i++)
        {
            num[ind]='\0';
        }
        ind=0;
    }
    else if(ch=='s')
    {
        for(i=0; i<sp; i++)
        {

            printf("%x  ", stack[i]);
        }
        printf("\n");
    }
 else if(ch!='\n' && ch!=EOF && ch!=' ' && ch!='q')
    {
        printf("Error, unsupported input!\n");
    }


}
