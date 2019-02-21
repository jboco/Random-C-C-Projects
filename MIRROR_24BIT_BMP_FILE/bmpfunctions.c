#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/*****open binary file for write operation***/
FILE* openBpmFileOut(char* fname){
    FILE* f=fopen(fname,"wb");
    return f;
}

/******** open binary file for read operation*******/
FILE* openBpmFile(char* fname){
    FILE* f=fopen(fname,"rb");
    return f;
}

/********close opened file******/
int closeBmpFile(FILE* f){
    fclose(f);
    return 0;
}

/***** get big Endian integer from little endian integer ***********/
int toBigEndianInt(int num){
 return (((num&0xff000000)>>24) | // move byte 3 to byte 0
                    (((num&0x00ff0000)>>8)) | // move byte 1 to byte 2
                    ((num&0x0000ff00)<<8) | // move byte 2 to byte 1
                    ((num&0x000000ff)<<24)); // byte 0 to byte 3
}

/***** return integer value of little endian encoded string ***********/
int fromLittleEndianToInt(char *tab, int size){
    int num=0;
    if(size<=0){
        num=0;
    } else if(size==2){
    num=0|(0x00ff&&tab[1]);
    num=(num<<8)|(0x00ff&tab[0]);
    }
    else if(size==3){
    num=0|(0x00ff&&tab[2]);
    num=(num<<8)|(0x00ff&tab[1]);
    num=(num<<8)|(0x00ff&tab[0]);
    } else {
    num=0|(0x00ff&&tab[3]);
    num=(num<<8)|(0x00ff&tab[2]);
    num=(num<<8)|(0x00ff&tab[1]);
    num=(num<<8)|(0x00ff&tab[0]);
    }
return num;
}

/***** return little endian encoded string of integer value ***********/
char* toLittleEndian(int num){
    char out[4];
    out[3]=((num&0xff000000)>>24);
    out[2]=(((num&0x00ff0000)>>16));
    out[1]= ((num&0x0000ff00)>>8);
    out[0]= (num&0x000000ff);
    return out;
}

/***** return big endian encoded string of integer value ***********/
char* toBigEndian(int num){
    char out[4];
    out[0]=((num&0xff000000)>>24);
    out[1]=(((num&0x00ff0000)>>16));
    out[2]= ((num&0x0000ff00)>>8);
    out[3]= (num&0x000000ff);
    return out;
}

/*********** printf hexadecimal representation of string*****************/
int toHexPrintf(unsigned char *str, int size){
    //char* out=(char*)malloc(2*size*sizeof(char));
    int i=0;
    for(i=0; i<size; i++){
        //num=str[i];;
                fprintf(stdout, "%02x", str[i]);

    }
    return 0;
}

/*********** printf hexadecimal representation of string in inverse order*****************/
int toHexBigEndianPrintf(unsigned char *str, int size){
    if(size<=0) return 0;
    else if(size==1)
          fprintf(stdout, "%02x", str[0]);
    else if(size==2){
          fprintf(stdout, "%02x", str[1]);
          fprintf(stdout, "%02x", str[0]);
    }  else if(size==3){
          fprintf(stdout, "%02x", str[2]);
          fprintf(stdout, "%02x", str[1]);
          fprintf(stdout, "%02x", str[0]);
    }else if(size>3){
          fprintf(stdout, "%02x", str[3]);
          fprintf(stdout, "%02x", str[2]);
          fprintf(stdout, "%02x", str[1]);
          fprintf(stdout, "%02x", str[0]);
    }
    return 0;
}

/*********** printf all needed data*****************/
int printfData(unsigned char* charval, int val, int size){
toHexPrintf(charval,size);
printf(",\t");
toHexBigEndianPrintf(charval,size);
printf(",\t");
fprintf(stdout,"%d,\t%s\n ",val,/*toBigEndian(val)*/charval);
}

/************read bmp file f and create it's mirror in out file ***************/
int readWriteMirrorBmp(FILE* f,FILE* out){

char* headerfield=(char*)malloc(3*sizeof(char));
int cmp=0, cmp2=0, cmp3=0;
int size=0;
unsigned char* reader=(unsigned char*)malloc(1*sizeof(unsigned char));
char* reserv1=(char*)malloc(3*sizeof(char));
char* reserv2=(char*)malloc(3*sizeof(char));
unsigned char cmstr[5];

unsigned char* image;
//int rest0;
int offset=0;
int rowsize;

int bidhead=0;
int width=0;
int height=0;

int nbrcolorplane=0;

int nbrbitsperpixel=0;

int compression=0;
int imgsize=0;
int hres=0;
int vres=0;

int nbrcolorpalette=0;
int nbrcolorused=0;
cmstr[4]=0;
/***********read bmp source file*********/
fread(headerfield, sizeof(char), 2, f);
headerfield[2]=0;

fread(cmstr, sizeof(unsigned char), 4, f);
size=fromLittleEndianToInt(cmstr,4);

fread(reserv1, sizeof(char), 2, f);
reserv1[2]=0;

fread(reserv2, sizeof(char), 2, f);
reserv2[2]=0;

fread(cmstr, sizeof(unsigned char), 4, f);
offset=fromLittleEndianToInt(cmstr,4);

fread(cmstr, sizeof(unsigned char), 4, f);
bidhead=fromLittleEndianToInt(cmstr,4);

fread(cmstr, sizeof(unsigned char), 4, f);
width=fromLittleEndianToInt(cmstr,4);


fread(cmstr, sizeof(unsigned char), 4, f);
height=fromLittleEndianToInt(cmstr,4);


fread(cmstr, sizeof(unsigned char), 2, f);

nbrcolorplane=fromLittleEndianToInt(cmstr,2);


fread(cmstr, sizeof(unsigned char), 2, f);

nbrbitsperpixel=fromLittleEndianToInt(cmstr,2);


fread(cmstr, sizeof(unsigned char), 4, f);
compression=fromLittleEndianToInt(cmstr,4);


fread(cmstr, sizeof(unsigned char), 4, f);
imgsize=fromLittleEndianToInt(cmstr,4);


fread(cmstr, sizeof(unsigned char), 4, f);
hres=fromLittleEndianToInt(cmstr,4);


fread(cmstr, sizeof(unsigned char), 4, f);
vres=fromLittleEndianToInt(cmstr,4);


fread(cmstr, sizeof(unsigned char), 4, f);
nbrcolorpalette=fromLittleEndianToInt(cmstr,4);


fread(cmstr, sizeof(unsigned char), 4, f);
nbrcolorused=fromLittleEndianToInt(cmstr,4);

//copy header
/*********** write destination bmp file header **********/
fseek(f, 0, SEEK_SET);
for(cmp=0;cmp<offset; cmp++){
    fread(reader, sizeof(unsigned char), 1, f);
    fwrite(reader, sizeof(unsigned char), 1, out);
}
/*************** read source bmp image data **********/
//the size of one row is width*number_of_bits_per_pixel plus  Line Padding
rowsize=4*((width*nbrbitsperpixel/8+nbrbitsperpixel/8)/4);
//image data's pointer
image= (unsigned char*) malloc(rowsize*(height)*sizeof(unsigned char));
if(image==NULL){
    printf("Not enough memory");
    free(image);
    return 0;
}
for(cmp=0; cmp<height; cmp++){
  for(cmp2=0; cmp2<width; cmp2++){
        //reading data
       
   for(cmp3=0; cmp3<nbrbitsperpixel/8; cmp3++){
    fread( (image+(cmp*rowsize+cmp2*nbrbitsperpixel/8+cmp3)),//reader
          sizeof(unsigned char), 1 , f);

   }
  }
  //reading padding
  for(cmp2=0; cmp2<rowsize-width*nbrbitsperpixel/8; cmp2++){
    fread(reader,sizeof(unsigned char),1,f);
  }
}

/*********write bmp image destination data ********/
for(cmp=0; cmp<height; cmp++){
  for(cmp2=width-1; cmp2>=0; cmp2--){

   for(cmp3=0; cmp3<nbrbitsperpixel/8; cmp3++){
        //writing data
    fwrite((image+cmp*rowsize+cmp2*nbrbitsperpixel/8+cmp3),
          sizeof(unsigned char), 1 , out);
   }
  }
  //writing padding
  for(cmp2=0; cmp2<rowsize-width*nbrbitsperpixel/8; cmp2++){
    fwrite('0',sizeof(unsigned char),1,out);
  }
}

}
