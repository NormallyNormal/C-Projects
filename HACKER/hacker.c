#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

char * skull[300];
int skullSize = 0;
int skullWidth = 0;

int mod(int a, int b) {
  return (a%b + b)%b;
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
      if(skullWidth < i) {
        skullWidth = i;
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
  FILE * fp = fopen("skull.txt", "r");
  while( (skull[skullSize] = readLine(fp)) ) {
    skullSize++;
  }
  initscr();
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(3, COLOR_WHITE, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  
  curs_set(0);
  int mx=0, my=0;
  getmaxyx(stdscr, mx, my);
  int falls[my / 2];
  for(int i = 0; i < my/2; i++) {
    falls[i] = rand() % mx;
  }
  
  attron(COLOR_PAIR(2));
  for( int i = 0; i < skullSize; i++ ) {
      move(i + mx/2 - skullSize/2,my/2 - skullWidth/2);
      addstr(skull[i]);
  }
  refresh();  
  
  attron(COLOR_PAIR(1));
  while(true) {
    int rng = rand() % (my/2);
    move( falls[rng], rng * 2 );
    falls[rng] = (falls[rng] + 1) % mx;
    //attron(COLOR_PAIR(rand() % 3 + 2));
    if((char)inch() == ' ') {
      if(rand() % 2) {
        addch( '1' );
      }
      else {
        addch( '0' );
      }
    }
    attron(COLOR_PAIR(1));
    move( mod((falls[rng] - (rand() % 15 ) - 5),mx), rng * 2 );
    if((char)inch() == '0' || (char)inch() == '1') addch( ' ' );
    move( mod((falls[rng] - 20),mx) , rng * 2 );
    if((char)inch() == '0' || (char)inch() == '1') addch( ' ' );
    refresh();
    usleep(1000);
  }
}
