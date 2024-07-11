/********************************************************
***IMPORTANT NOTE***
If you have problems with the provided sample code,
part of the reason might be due to the function "fopen".
Please try changing "r/w" to "rb/wb" or the other way
when you use this function.
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>  
#include <memory.h>

#define max(x, y) ((x>y) ? (x):(y))
#define min(x, y) ((x<y) ? (x):(y))


int xdim;
int ydim;
int maxraw;
unsigned char *image;
unsigned char *image2;
void ReadPGM(FILE*);
void scale(FILE*, float scaling_factor, char **argv);
void WritePGM(FILE*,int xdim, int ydim);
int i, j;

int main(int argc, char **argv)
{
  char str[20];
  
  FILE *fp;
  printf(" value of argc: %d", argc);
  if (argc != 3){
    printf("Usage: MyProgram <input_ppm> <output_ppm> \n");
    printf("       <input_ppm>: PGM file \n");
    printf("       <output_ppm>: PGM file \n");
    exit(0);              
  }

  /* begin reading PGM.... */
  printf("begin reading PGM.... \n");
  if ((fp=fopen(argv[1], "rb"))==NULL){
    printf("read error...\n");
    exit(0);
  }
  
  ReadPGM(fp);
  
  float scaling_factor;  // keep the scaling factor number here.
  
  printf("Enter scaling factor: ");
  scanf("%f",&scaling_factor);
  
  printf("scaling_factor -> %f\n",scaling_factor);
  
  scale(fp, scaling_factor, argv); // calling scale function to resample the image.
  
  free(image);
  free(image2);
  return (1);
}

void scale(FILE* fp, float scaling_factor, char **argv){ // function to resample the image.
  printf("%d<-width\n",xdim);
  printf("%d<-height\n",ydim);
  int xdim1 = (int)xdim*scaling_factor;
  int ydim1 = (int)ydim*scaling_factor;
  printf("%d<-new_width\n",xdim1);
  printf("%d<-new_height\n",ydim1);
  image2 = (unsigned char*)malloc(sizeof(unsigned char)*xdim1*ydim1);
  for (j=0; j<ydim1; j++){  //new height->rows
    for (i=0; i<xdim1; i++) { //new width->coloums
    	float _y = (i/scaling_factor);
    	float _x = (j/scaling_factor);
    	int x1 = (int)_x;
	int y1 = (int)_y;
	int x2 = min(x1 + 1, ydim - 1);
	int y2 = min(y1 + 1, xdim - 1);
	int dx = _x - x1;
	int dy = _y - y1;
	image2[j*xdim1+i] = (1 - dx) * (1 - dy) * image[x1*xdim+y1] + \
                    dx * (1 - dy) * image[x2*xdim+y1] + \
                    (1 - dx) * dy * image[x1*xdim+y2] + \
                    dx * dy * image[x2*xdim+y2];
    }
  }
  printf("Begin writing PGM.... \n");
  if ((fp=fopen(argv[2], "wb")) == NULL){
     printf("write pgm error....\n");
     exit(0);
   }
  WritePGM(fp,xdim1,ydim1);
}

void ReadPGM(FILE* fp)
{
    int c;
    int i,j;
    int val;
    unsigned char *line;
    char buf[1024];


    while ((c=fgetc(fp)) == '#')
        fgets(buf, 1024, fp);
     ungetc(c, fp);
     if (fscanf(fp, "P%d\n", &c) != 1) {
       printf ("read error ....");
       exit(0);
     }
     if (c != 5 && c != 2) {
       printf ("read error ....");
       exit(0);
     }

     if (c==5) {
       while ((c=fgetc(fp)) == '#')
         fgets(buf, 1024, fp);
       ungetc(c, fp);
       if (fscanf(fp, "%d%d%d",&xdim, &ydim, &maxraw) != 3) {
         printf("failed to read width/height/max\n");
         exit(0);
       }
       printf("Width=%d, Height=%d \nMaximum=%d\n",xdim,ydim,maxraw);

       image = (unsigned char*)malloc(sizeof(unsigned char)*xdim*ydim);
       getc(fp);

       line = (unsigned char *)malloc(sizeof(unsigned char)*xdim);
       for (j=0; j<ydim; j++) {
          fread(line, 1, xdim, fp);
          for (i=0; i<xdim; i++) {
            image[j*xdim+i] = line[i];
         }
       }
       free(line);

     }

     else if (c==2) {
       while ((c=fgetc(fp)) == '#')
         fgets(buf, 1024, fp);
       ungetc(c, fp);
       if (fscanf(fp, "%d%d%d", &xdim, &ydim, &maxraw) != 3) {
         printf("failed to read width/height/max\n");
         exit(0);
       }
       printf("Width=%d, Height=%d \nMaximum=%d,\n",xdim,ydim,maxraw);

       image = (unsigned char*)malloc(sizeof(unsigned char)*xdim*ydim);
       getc(fp);

       for (j=0; j<ydim; j++)
         for (i=0; i<xdim; i++) {
            fscanf(fp, "%d", &val);
            image[j*xdim+i] = val;
         }

     }

     fclose(fp);
}


void WritePGM(FILE* fp,int xdim,int ydim)
{
  int i,j;
  fprintf(fp, "P5\n%d %d\n%d\n", xdim, ydim, 255);
  for (j=0; j<ydim; j++)
    for (i=0; i<xdim; i++) {
      fputc(image2[j*xdim+i], fp);
    }

  fclose(fp);
  
}
