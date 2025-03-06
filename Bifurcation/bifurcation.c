#include <stdbool.h>
#include <stdio.h>

bool image[1600][1000];
unsigned char buffer[1600 * 1000 * 3];
int main() {
  for(int i = 0; i < 1600; i++) {
    for(int j = 0; j < 1000; j++) {
      image[i][j] = false;
    }
  }
  for(int x = 2400; x < 4000; x++) {
    double r = (x - 2400) / 1000.0 + 2.4;
    double xn = 0.5;
    for(int i = 0; i < 300; i++) {
      xn = xn * r * (1.0 - xn);
    }
    int y;
    for(int i = 0; i < 400; i++) {
      xn = xn * r * (1.0 - xn);
      y = (int)(xn * 1000.0);
      if(y < 1000 && y > 0) {
        image[x - 2400][y] = true;
      }
    }
  }
  int buffloc = 0;
  for(int j = 999; j >= 0; j--) {
    for(int i = 0; i < 1600; i++) {
      if(image[i][j]) {
        buffer[buffloc] = 0;
        buffer[buffloc+1] = 0;
        buffer[buffloc+2] = 0;
      }
      else {
        buffer[buffloc] = 255;
        buffer[buffloc+1] = 255;
        buffer[buffloc+2] = 255;
      }
      buffloc += 3;
    }
  }
  FILE *fp = fopen( "bifur.ppm", "wb" );
  (void) fprintf( fp, "P6\n%d %d\n255\n", 1600, 1000 );
  fwrite(buffer, 1, 1600 * 1000 * 3, fp);
  (void) fclose(fp);
}
