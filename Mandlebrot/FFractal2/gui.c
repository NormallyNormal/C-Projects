#include <ncurses.h>
#include <stdlib.h>

#include "gui.h"

void startGUI() {
   initscr();
   
   curs_set(0);
   addstr("'.' = Awaiting Thread\n'/' = In Progress\n'%' = Complete\n");
   refresh();
}

void computationDone() {
   move( 0, 0 );
   addstr("Writing image...\n\t\t\n\t\t");
   refresh();
}

void endGUI() {
   move( 0, 0 );
   addstr("Done!\nPress any key to exit...\n\t\t");
   curs_set(0);
   refresh();
   getch();
   endwin();
}

void threadVisual( int taskRows, int taskCols ) 
{
   curs_set(0);
   for( int i = 0; i < taskCols; i++ ) {
      for( int j = 0; j < taskRows; j++ ) {
         addch( '.' );
      }
      addch( '\n' );
   }
   refresh();
}

void threadVisualUpdate( int taskRow, int taskCol, short state ) 
{
   move ( 3 + taskRow, taskCol );
   switch( state ) {
      case 0:
         addch( '.' );
         break;
      case 1:
         addch( '/' );
         break;
      case 2:
         addch( '%' );
         break;
      default:
         addch( 'X' );
         break;
   }
   refresh();
}
