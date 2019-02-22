#include<stdio.h>
#include <math.h>
#include<stdlib.h>
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
    char* out=(char*)malloc(4*sizeof(char));
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
    
    int i=0;
    for(i=0; i<size; i++){
       
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

/************* compute bilinear interpolation elementary operation **************/
int bilinearInt(char c00, char c10, char c01, char c11, float dx, float dy){
     float xt = (int)(c00) * ((1) - dx) + (int)(c10) * dx;
     float yt= (int)( c01) * ((1) - dx) + (int)(c11) * dx;
     return (int)(xt * ((1) - dy) + yt * dy);
}

/************compute bilinear interpolation ****************/
unsigned char* bilinearCompression(unsigned char* image, /*unsigned char* newimage0,*/ int width,
                                   int height, int nbrpixel, int newwidth, int newheight, int newdatasize){

 
 unsigned char* newimage=(unsigned char*) malloc(newdatasize*sizeof(unsigned char));
 int gx, gy;
 float gxf, gyf, dx, dy;
 unsigned char r, g, b;
 char *px00, *px10, *px01, *px11;
 int cmpx=0, cmpy=0;
  
      for(cmpx= 0, cmpy=0; cmpy < newheight; cmpx++){
       {
        if(cmpx > newwidth){
            cmpx = 0; cmpy++;
        }
            gxf=(cmpx/(float)(newwidth)*(width-1));
            gyf=(cmpy/(float)(newheight)*(height-1));
            gx=(int)gxf;
            gy=(int)gyf;
            dx=gxf-gx;
            dy=gyf-gy;
            if(dx<0|| dy<0)
                fprintf(stdout,"compx:%d dx:%d  dy:%d\n", cmpx, dx, dy);
            if(cmpx>gxf|| cmpy>gyf)
                fprintf(stdout,"compx:%d dx:%d  dy:%d\n", cmpx, dx, dy);

            px00=image+(gy*width*nbrpixel/8+gx*nbrpixel/8);
            px10=image+(gy*width*nbrpixel/8+(gx+1)*nbrpixel/8);
            px01=image+((gy+1)*width*nbrpixel/8+gx*nbrpixel/8);
            px11=image+((gy+1)*width*nbrpixel/8+(gx+1)*nbrpixel/8);

           newimage[((cmpy*newwidth*nbrpixel/8)+(cmpx*nbrpixel/8))]= /*(b)=*/
                        bilinearInt(*(px00+0), *(px10+0), *(px01+0), *(px11+0), dx, dy );
           newimage[((cmpy*newwidth*nbrpixel/8)+(cmpx*nbrpixel/8)+1)]=/*(g)=*/
                        bilinearInt(*(px00+1), *(px10+1), *(px01+1), *(px11+1), dx, dy  );
           newimage[((cmpy*newwidth*nbrpixel/8)+(cmpx*nbrpixel/8)+2)]= /*(r)=*/
                        bilinearInt(*(px00+2), *(px10+2),  *(px01+2), *(px11+2), dx, dy );
           if(((cmpy*newwidth*nbrpixel/8)+(cmpx*nbrpixel/8)+2)>newdatasize)
                    fprintf(stdout, "supérieur (%d;%d;%d) / %d",
                           ((cmpy*newwidth*nbrpixel/8)+(cmpx*nbrpixel/8)), ((cmpy*newwidth*nbrpixel/8)+(cmpx*nbrpixel/8)+1),
                           ((cmpy*newwidth*nbrpixel/8)+(cmpx*nbrpixel/8)+2), newdatasize  );
           
          }
          
     }
 return newimage;
}

/****************** read a bmp image file f and compute it's bilinear interpolation with   *********************/
/*************bilinear interpolation ***************/
int readWriteInterpolatedBmp(FILE* f,FILE* out, float weight){

unsigned char* headerfield=(char*)malloc(3*sizeof(char));
int cmp=0, cmp2=0, cmp3=0;
int size=0, newsize=0;
unsigned char* reader=(unsigned char*)malloc(1*sizeof(unsigned char));
unsigned char* reserv1=(char*)malloc(3*sizeof(char));
unsigned char* reserv2=(char*)malloc(3*sizeof(char));
unsigned char cmstr[5];

unsigned char *image, *newimage;
//int rest0;
int offset=0;
int rowsize, newrowsize;

int bidhead=0;
int width=0;
int newwidth=0;
int newheight=0;
int height=0;

int nbrcolorplane=0;

int nbrbitsperpixel=0;

int compression=0;
int imgsize=0;
int hres=0;
int vres=0;

int nbrcolorpalette=0;
int nbrcolorused=0;
int glue=0, glue2=0;
int newdatasize=0;


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
//printfData(cmstr, nbrcolorused, 4);

/**********************/
newwidth=(int)(width*weight);
newheight=(int)(height*weight);
rowsize=4*((width*nbrbitsperpixel/8+nbrbitsperpixel/8)/4);
newrowsize=4*(((newwidth)*nbrbitsperpixel/8+nbrbitsperpixel/8)/4);
newsize=offset+newrowsize*newheight;
newdatasize=newrowsize*(newheight);//((newwidth+1)*nbrpixel/8)*(newheight)
image= (unsigned char*) malloc(rowsize*(height)*sizeof(unsigned char));

/******write destination image's header ******/
int  pos=0;
pos+=fwrite(headerfield, sizeof(unsigned char), 2, out);
pos+=fwrite(toLittleEndian(newsize), sizeof(char), 4, out);
pos+=fwrite(reserv1, sizeof(unsigned char), 2, out);
pos+=fwrite(reserv2, sizeof(unsigned char), 2, out);
pos+=fwrite(toLittleEndian(offset), sizeof(unsigned char), 4, out);
pos+=fwrite(toLittleEndian(bidhead), sizeof(unsigned char), 4, out);
pos+=fwrite(toLittleEndian(newwidth), sizeof(unsigned char), 4, out);
pos+=fwrite(toLittleEndian(newheight), sizeof(unsigned char), 4, out);
pos+=fwrite(toLittleEndian(nbrcolorplane), sizeof(unsigned char), 2, out);
pos+=fwrite(toLittleEndian(nbrbitsperpixel), sizeof(unsigned char),2 , out);
pos+=fwrite(toLittleEndian(compression), sizeof(unsigned char), 4, out);
pos+=fwrite(toLittleEndian(imgsize), sizeof(unsigned char), 4, out);//newwidth*newheight newrowsize*newheight
pos+=fwrite(toLittleEndian(hres), sizeof( char), 4, out);
pos+=fwrite(toLittleEndian(vres), sizeof( char), 4, out);
pos+=fwrite(toLittleEndian(nbrcolorpalette), sizeof(unsigned char), 4, out);
pos+=fwrite(toLittleEndian(nbrcolorused), sizeof(unsigned char), 4, out);/**/

/***/
while(pos<offset){
    pos+=fwrite(reader,1,1,out);
}

/**/


if(image==NULL){
    printf("Not enough memory");
    free(image);
    return 0;
}


for(cmp=0; cmp<height; cmp++){
  for(cmp2=0; cmp2<width; cmp2++){
/
   for(cmp3=0; cmp3<nbrbitsperpixel/8; cmp3++){
    glue+=fread( (image+(cmp*rowsize+cmp2*nbrbitsperpixel/8+cmp3)),//reader
          sizeof(unsigned char), 1 , f);
    
   }
  }
  //reading padding
  for(cmp2=0; cmp2<rowsize-width*nbrbitsperpixel/8; cmp2++){
    glue+=fread(reader,sizeof(unsigned char),1,f);
  }
  // fprintf(stdout,"lue0: %d\n", glue);
}

newimage=bilinearCompression(image,  width,height,nbrbitsperpixel,newwidth,newheight, newdatasize);
//glue=0;

for(cmp=0; cmp<newheight; cmp++){

  for(cmp2=0; cmp2<newwidth; cmp2++){
    
    for(cmp3=0; cmp3<nbrbitsperpixel/8; cmp3++){
       
        glue2+=fwrite((newimage+((cmp*newrowsize)+(cmp2*nbrbitsperpixel/8)+cmp3)),
              sizeof(unsigned char), 1 , out);
   }
  }

  for(cmp2=0; cmp2<newrowsize-(newwidth*nbrbitsperpixel/8); cmp2++){
    glue2+=fwrite(reader,sizeof(unsigned char),1,out);
  }

}


}

/*********** read bmp file and print out it's header *****************/
int readBmpHeader(FILE* f){

char* headerfield=(char*)malloc(3*sizeof(char));
int cmp=0, cmp2=0, cmp3=0, sizecomp=0;
int size=0;
char* reader=(char*)malloc(1*sizeof(char));
char* reserv1=(char*)malloc(3*sizeof(char));
char* reserv2=(char*)malloc(3*sizeof(char));
unsigned char cmstr[5];

int offset=0;

int bidhead=0;
int width=0;
int height=0;
//char * nbrcolorplanec=(char*)malloc(3*sizeof(char));
int nbrcolorplane=0;
//char * nbrbitsperpixelc=(char*)malloc(3*sizeof(char));
int nbrbitsperpixel=0;

int compression=0;
int imgsize=0;
int hres=0;
int vres=0;

int nbrcolorpalette=0;
int nbrcolorused=0;
long sizet=0, position=0;
cmstr[4]=0;
fprintf(stdout, "    BYTES\t xVAL(LE)\tiVAL(LE)\t   CHARS\n");

/*********read and printf operation***********/
fread(headerfield, sizeof(char), 2, f);
headerfield[2]=0;
printfData(headerfield,fromLittleEndianToInt(headerfield,2), 2);

fread(cmstr, sizeof(unsigned char), 4, f);
size=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,size, 4);

fread(reserv1, sizeof(char), 2, f);
reserv1[2]=0;
printfData(reserv1,fromLittleEndianToInt(reserv1,2), 2);

fread(reserv2, sizeof(char), 2, f);
reserv2[2]=0;
printfData(reserv2,fromLittleEndianToInt(reserv2,2), 2);

fread(cmstr, sizeof(unsigned char), 4, f);
offset=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,offset, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
bidhead=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,bidhead, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
width=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,width, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
height=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,height, 4);

fread(cmstr, sizeof(unsigned char), 2, f);
//nbrcolorplanec[3]=0;
//nbrcolorplane=0|(0x00ff&&nbrcolorplanec[1]);
//nbrcolorplane=(nbrcolorplane<<8)|(0x00ff&nbrcolorplanec[0]);
nbrcolorplane=fromLittleEndianToInt(cmstr,2);
printfData(cmstr,nbrcolorplane, 2);

fread(cmstr, sizeof(unsigned char), 2, f);
//nbrbitsperpixelc[3]=0;
//nbrbitsperpixel=0|(0x00ff&&nbrbitsperpixelc[1]);
//nbrbitsperpixel=(nbrbitsperpixel<<8)|(0x00ff&nbrbitsperpixelc[0]);
nbrbitsperpixel=fromLittleEndianToInt(cmstr,2);
printfData(cmstr,nbrbitsperpixel, 2);

fread(cmstr, sizeof(unsigned char), 4, f);
compression=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,compression, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
imgsize=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,imgsize, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
hres=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,hres, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
vres=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,vres, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
nbrcolorpalette=fromLittleEndianToInt(cmstr,4);
printfData(cmstr,nbrcolorpalette, 4);

fread(cmstr, sizeof(unsigned char), 4, f);
nbrcolorused=fromLittleEndianToInt(cmstr,4);
printfData(cmstr, nbrcolorused, 4);



}
