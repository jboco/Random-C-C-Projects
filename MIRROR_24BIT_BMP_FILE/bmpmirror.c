//#include <stdio.h>
//#include <math.h>
//#include <stdlib.h>
//#include <string.h>
#include "bmpfunctions.c"

int main(int argc, char* argv[]){
    if(argc<=0) return 0;
   /**/ char *filename=argv[1];
   if(filename==NULL) return 0;
    char filenameop[30];
    char filenameout[50];

    strcpy(filenameop, filename);
    strcat(filenameop, ".bmp");

    strcpy(filenameout, filename);
    strcat(filenameout, "mirrored.bmp");



    FILE* f=openBpmFile(filenameop);
    FILE* fout=openBpmFileOut(filenameout);
    readWriteMirrorBmp(f,fout);
    closeBmpFile(f);
    closeBmpFile(fout);


   // free(filename); free(filenameop); free(filenameout);

   // free(f);   free(fout);

}
