#include <stdio.h>

#include "io.h"

void export( int x_res, int y_res, Color image[y_res][x_res] ) {
   FILE *fp = fopen( "image.ppm", "wb" );
   (void) fprintf( fp, "P6\n%d %d\n255\n", x_res, y_res );
   for ( int y = 0; y < y_res; y++ ) {
      for ( int x = 0; x < x_res; x++ ) {   
         uint8_t colorArray[] = { image[y][x].red, image[y][x].green, image[y][x].blue }; 
         (void) fwrite(colorArray, 1, 3, fp);
      }
   }
}
