#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <curses.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

#define IPADDRESS "@IPADDR"
#define PORT @PORT
#define SIZE sizeof(struct sockaddr_in)

struct character {
	int x, y;
}

main() {
  int sockfd, result, fd, ch;
  char message[BUFSIZ];
  fd_set readfds, testfds;
  
  struct sockaddr_in server = { AF_INET, PORT };

   server.sin_addr.s_addr = inet_addr( IPADDRESS );
   if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
      perror( "socket call failed" );
      exit( 1 );
   }
   if( connect( sockfd, ( struct sockaddr * ) &server, SIZE ) == -1 ) {
      perror( "connect call failed" );
      exit( 1 );
   }
   
  initCurses();

	struct character *c = ( struct character* )malloc(sizeof( struct character ) );

	c->x = c->y = 2;
  
  drawScreen(c->x, c->y);

  FD_ZERO(&readfds);
  FD_SET(sockfd, &readfds);
  FD_SET(0, &readfds);
  while (1) {
    testfds = readfds;
    select(FD_SETSIZE, &testfds, NULL, NULL, NULL);

    for (fd = 0; fd < 4; fd++) {
      if (FD_ISSET(fd, &testfds)) {
        if (fd == 0)  {
          ch = getch();
          switch ( ch ) {
            case KEY_DOWN:
							write(sockfd, "KEY_DOWN PRESSED", 17);
              if ( c->y < LINES-2 ) {
                c->y++;
              }
              break;
            case KEY_UP:
              write(sockfd, "KEY_UP PRESSED", 15);
              if ( c->y > 1 ) {
                c->y--;
              } 
              break;
            case KEY_LEFT:
              write(sockfd, "KEY_LEFT PRESSED", 17);
              if ( c->x > 2 ) {
                c->x--;
              }
              break;
            case KEY_RIGHT:
              write(sockfd, "KEY_RIGHT PRESSED", 18);
              if ( c->x < COLS-2) {
                c->x++;
              }
              break;
          }
          drawScreen(c->x, c->y);
        }
      }
    }
  }
}

initCurses()
{
  int die();
  initscr();
  clear();
  keypad(stdscr,1);
  signal(SIGINT,(__sighandler_t)die);
  crmode();
  noecho();
  nonl();
  curs_set(0);
}

die() {
  signal(SIGINT,SIG_IGN);
  mvcur(0,COLS-1,LINES-1,0);
  endwin();
  exit(0);
}

getInput() 
{
  int c,x,y,ch;

  move(1,1);
  do {

    
  } while(1);
  for (y = 1; y < LINES-1; y++)
    for (x = 1; x < COLS-1; x++) {
         move(y,x);
   ch=inch() & A_CHARTEXT;
         if (ch == 'x') addlist(y,x);
    } 
}

drawScreen( int x, int y ) {
	if( !x || !y )
		return;
	clear();
	move( y, x);
	addch('X');	
  box(stdscr,'|','-');
  refresh();
}

