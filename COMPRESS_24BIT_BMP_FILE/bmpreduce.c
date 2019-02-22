//#include <stdio.h>
//#include <math.h>
//#include <stdlib.h>
//#include <string.h>
#include "bmpfunctions.c"

int main( int argc, char* argv[]){
     //if(argc<=1) return 0;
   /**/ char *filename= argv[1];
     float weight= atof(argv[2]);
    char filenameop[30];
    char  filenamecout[50];

    strcpy(filenameop, filename);
    strcat(filenameop, ".bmp");

    strcpy(filenamecout, filename);
    strcat(filenamecout, "reduced.bmp");

    FILE* f2=openBpmFile(filenameop);
    FILE* fcout=openBpmFileOut(filenamecout);
    readWriteInterpolatedBmp(f2,fcout,weight);
    closeBmpFile(f2);
    closeBmpFile(fcout);


  //  free(filename); free(filenameop);

    //free(f2);  free(fcout);
     f2=openBpmFile(filenamecout);
    readBmpHeader(f2);
    //free(filenamecout);
    //free(f2);
}
