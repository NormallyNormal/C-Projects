#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ppmrender.h"
#include "createspectrum.h"

struct Color hueToRgb( int hue );

void createImageFromSet( int rows, int cols, int image[ rows ][ cols ],  int colorOffset, char * spectrum ) {
   int i, j;
   FILE *fp = fopen( "fractal.ppm", "wb" );
   struct Color * color = NULL;
   int spectrumRange = createSpectrum( spectrum, &color );
   (void) fprintf( fp, "P6\n%d %d\n255\n", cols, rows );
   if(spectrumRange != -1) {
      for (j = 0; j < cols; ++j)
      {
         for (i = 0; i < rows; ++i)
         {
            if ( image[ i ][ j ] == 0 ) {
               static unsigned char colorArray[3];
               colorArray[0] = color[0].red;
               colorArray[1] = color[0].green;
               colorArray[2] = color[0].blue;
               (void) fwrite(colorArray, 1, 3, fp);
            }
            else {
               int inters = image[ i ][ j ];
               static unsigned char colorArray[3];
               colorArray[0] = color[inters % (spectrumRange - 1) + 1].red;
               colorArray[1] = color[inters % (spectrumRange - 1) + 1].green;
               colorArray[2] = color[inters % (spectrumRange - 1) + 1].blue;
               (void) fwrite(colorArray, 1, 3, fp);
            }
         }
      }
   }
   else {
      for (j = 0; j < cols; ++j)
      {
         for (i = 0; i < rows; ++i)
         {
            if ( image[ i ][ j ] == 0 ) {
               static unsigned char colorArray[3];
               colorArray[0] = 0;
               colorArray[1] = 0;
               colorArray[2] = 0;
               (void) fwrite(colorArray, 1, 3, fp);
            }
            else {
               struct Color color = hueToRgb( image[ i ][ j ] * 5 + colorOffset );
               static unsigned char colorArray[3];
               colorArray[0] = color.red;
               colorArray[1] = color.green;
               colorArray[2] = color.blue;
               (void) fwrite(colorArray, 1, 3, fp);
            }
         }
      }
   }
   if(color != NULL) {
      free(color);
   }
   (void) fclose(fp);
}

struct Color hueToRgb( int hue )
{
   struct Color color;
   switch( (hue / 60) % 6 ) {
      case 0:
         color.red = 255;
         color.green = (hue % 60) * 255 / 60;
         color.blue = 0;
         break;
      case 1:
         color.red = 255 - (hue % 60) * 255 / 60;
         color.green = 255;
         color.blue = 0;
         break;
      case 2:
         color.red = 0;
         color.green = 255;
         color.blue = (hue % 60) * 255 / 60;
         break;
      case 3:
         color.red = 0;
         color.green = 255 - (hue % 60) * 255 / 60;
         color.blue = 255;
         break;
      case 4:
         color.red = (hue % 60) * 255 / 60;
         color.green = 0;
         color.blue = 255;
         break;
      case 5:
         color.red = 255;
         color.green = 0;
         color.blue = 255 - (hue % 60) * 255 / 60;
         break;
   }
   return color;
}
