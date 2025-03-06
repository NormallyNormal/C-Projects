#include <stdio.h>
#include <stdlib.h>

#include "createspectrum.h"

int createSpectrum(char * file, struct Color ** color) {
   FILE *fp = fopen( file, "rb" );
   if(fp == NULL) {
      return -1;
   }
   int size = 50;
   int count = 0;
   int readIn = 0;
   *color = malloc(sizeof(struct Color) * size);
   readIn = fscanf(fp, "%hhd", &((*color)[0].red));
   readIn = fscanf(fp, "%hhd", &((*color)[0].green));
   readIn = fscanf(fp, "%hhd", &((*color)[0].blue));
   count++;
   struct Color nextColor;
   int gradient;
   fscanf(fp, "%hhd", &((*color)[count].red));
   fscanf(fp, "%hhd", &((*color)[count].green));
   fscanf(fp, "%hhd", &((*color)[count].blue));
   while(readIn != -1) {
      readIn = fscanf(fp, "%d", &gradient);
      readIn = fscanf(fp, "%hhd", &(nextColor.red));
      readIn = fscanf(fp, "%hhd", &(nextColor.green));
      readIn = fscanf(fp, "%hhd", &(nextColor.blue));
      if(readIn == -1) {
         break;
      }
      if(size < count + gradient + 1) {
         size *= 2;
         (*color) = realloc( *color, size * sizeof(struct Color));
      }
      for(int i = 1; i < gradient; i++) {
         (*color)[count + i].red = ((float)(gradient - i) / gradient) * (*color)[count].red + ((float)i / gradient) * nextColor.red;
         (*color)[count + i].green = ((float)(gradient - i) / gradient) * (*color)[count].green + ((float)i / gradient) * nextColor.green;
         (*color)[count + i].blue = ((float)(gradient - i) / gradient) * (*color)[count].blue + ((float)i / gradient) * nextColor.blue;
      }
      count += gradient;
      (*color)[count].red = nextColor.red;
      (*color)[count].green = nextColor.green;
      (*color)[count].blue = nextColor.blue;
   }
   return count;
}
