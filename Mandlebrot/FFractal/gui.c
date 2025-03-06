#include <ncurses.h>
#include <stdlib.h>

#include "gui.h"

void startGUI() {
   initscr();
   
   curs_set(0);
   addstr("'.' = Awaiting Thread\n'/' = In Progress\n'%' = Complete\n");
   refresh();
}

void endGUI() {
   addstr("\nDone!\nPress any key to exit...");
   curs_set(1);
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
   move( 0, 0 );
   refresh();
}
