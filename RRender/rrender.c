#include <stdio.h>
#include <stdlib.h>

#include "vector3.h"
#include "color.h"
#include "rrender.h"
#include "io.h"

#define X_RES 600
#define Y_RES 400

int main() {
   Color (*picture)[X_RES] = calloc( Y_RES, sizeof *picture );
   for ( int y = 0; y < Y_RES; y++ ) {
      for ( int x = 0; x < X_RES; x++ ) {
         uint8_t num = (y % 100 >= 50) ? ( (x % 100 >= 50) ? 200 : 100 ) : ( (x % 100 < 50) ? 200 : 100 ) ;
         picture[y][x].red = num;
         picture[y][x].green = num;
         picture[y][x].blue = num;
      }
   }
   export( X_RES, Y_RES, picture );
   free( picture );
}
