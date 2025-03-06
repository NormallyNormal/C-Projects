#define _DEFAULT_SOURCE
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int mod(int a, int b) {
  return (a%b + b)%b;
}

uint8_t wrapAccess( int mx, int my, uint8_t (*array)[mx][my], int i, int j ) {
  return (*array)[mod(i, mx)][mod(j, my)];
}

char *readLine( FILE *fp ) {
   int size = 5;
   char *line = malloc( size );
   char read;
   int i = 0;
   //Read characters until newline or EOF
   while ( fscanf( fp, "%c", &read) != EOF && read != '\n' ) {
      line[i++] = read;
      //+ 1 to account for null terminator that will need to be added
      if( i + 1 > size ) {
         size *= 2;
         line = realloc( line, size );
      }
   }
   line[i] = '\0';
   if ( i == 0 ) {
      free( line );
      line = NULL;
   }
   return line;
}

int main() {
  initscr();
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  
  curs_set(0);
  int mx=0, my=0;
  getmaxyx(stdscr, mx, my);
  
  uint8_t array1[mx][my];
  uint8_t array2[mx][my];
  
  srand ( time(NULL) );
  for(int i = 0; i < mx; i++) {
    for(int j = 0; j < my; j++) {
      array1[i][j] = rand() % 2;
      array2[i][j] = 0;
    }
  }
  
  /**
  array1[5][5] = 1;
  array1[6][6] = 1;
  array1[7][4] = 1;
  array1[7][5] = 1;
  array1[7][6] = 1;
  */
  
  /**
  array1[10][20] = 1;
  array1[10][21] = 1;
  array1[10][22] = 1;
  array1[10][24] = 1;
  
  array1[11][20] = 1;
  
  array1[12][23] = 1;
  array1[12][24] = 1;
  
  array1[13][21] = 1;
  array1[13][22] = 1;
  array1[13][24] = 1;
  
  array1[14][20] = 1;
  array1[14][22] = 1;
  array1[14][24] = 1;
  */
  
  uint8_t (*arrayA)[mx][my];
  uint8_t (*arrayB)[mx][my];
  
  arrayA = &array1;
  arrayB = &array2;
  
  for(int i = 0; i < mx; i++) {
    for(int j = 0; j < my; j++) {
      move(i, j);
      if( wrapAccess(mx, my, arrayA, i, j) > 0 ) {
        addch( '%' );
      }
      else
        addch( ' ' );
    }
  }
  
  attron(COLOR_PAIR(1));
  
  refresh();
  usleep(100000);
    
  while(true) {
    for(int i = 0; i < mx; i++) {
      for(int j = 0; j < my; j++) {
        int count = 0;
        if( wrapAccess(mx, my, arrayA, i - 1, j - 1) > 0 ) count++;
        if( wrapAccess(mx, my, arrayA, i + 0, j - 1) > 0 ) count++;
        if( wrapAccess(mx, my, arrayA, i + 1, j - 1) > 0 ) count++;
        if( wrapAccess(mx, my, arrayA, i - 1, j + 1) > 0 ) count++;
        if( wrapAccess(mx, my, arrayA, i + 0, j + 1) > 0 ) count++;
        if( wrapAccess(mx, my, arrayA, i + 1, j + 1) > 0 ) count++;
        if( wrapAccess(mx, my, arrayA, i - 1, j + 0) > 0 ) count++;
        if( wrapAccess(mx, my, arrayA, i + 1, j + 0) > 0 ) count++;
        if( count < 2 || count > 3 ) {
          if( wrapAccess(mx, my, arrayA, i, j) > 0 ) {
            move(i, j);
            addch( ' ' );
          }
          (*arrayB)[mod(i, mx)][mod(j, my)] = 0;
        }
        else if( count == 3 ) {
          if( wrapAccess(mx, my, arrayA, i, j) == 0 ) {
            move(i, j);
            addch( '%' );
          }
          (*arrayB)[mod(i, mx)][mod(j, my)] = 1;
        }
        else {
          (*arrayB)[mod(i, mx)][mod(j, my)] = wrapAccess(mx, my, arrayA, i, j);
        }
      }
    }

    uint8_t (*arrayX)[mx][my] = arrayA;
    arrayA = arrayB;
    arrayB = arrayX;
    refresh();

    usleep(100000);
  }
}
