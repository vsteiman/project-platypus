#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <curses.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#include "global.h"
#include "nclient.h"

#define IPADDRESS "@IPADDR"
#define PORT @PORT
#define SIZE sizeof(struct sockaddr_in)
#define CHATSIZE 15 

void initCurses();
void drawScreen( int, int );

main() {
  int sockfd, result, fd, ch, nread;
  char message[1024];
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

  ChatWindow *cw = new ChatWindow( LINES - (CHATSIZE+2), CHATSIZE - 2);

  drawScreen(c->x, c->y);
  cw->draw();
  refresh();

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
              //write(sockfd, "KEY_DOWN PRESSED", 17);
              if ( c->y < ( LINES - CHATSIZE ) - 3 ) {
                c->y++;
              }
              break;
            case KEY_UP:
              //write(sockfd, "KEY_UP PRESSED", 15);
              if ( c->y > 1 ) {
                c->y--;
              }
              break;
            case KEY_LEFT:
              //write(sockfd, "KEY_LEFT PRESSED", 17);
              if ( c->x > 2 ) {
                c->x--;
              }
              break;
            case KEY_RIGHT:
              //write(sockfd, "KEY_RIGHT PRESSED", 18);
              if ( c->x < COLS-2) {
                c->x++;
              }
              break;
          }
          clear();
          cw->draw();
          drawScreen(c->x, c->y);
          refresh();
        } else {
          ioctl( fd, FIONREAD, &nread );

          if( nread == 0 )
            exit(0);

          nread = read( fd, message, nread );
          message[nread] = '\0';

          cw->addMessage( message, (char *)"admin" );
          cw->draw();
          drawScreen(c->x, c->y);					
          refresh();
        }
      }
    }
  }
}

void initCurses()
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

int die() {
  signal(SIGINT,SIG_IGN);
  mvcur(0,COLS-1,LINES-1,0);
  endwin();
  exit(0);
}

void drawScreen( int x, int y ) {
  if( !x || !y )
    return;
  move( y, x );
  addch( 'X' );

  box( stdscr, '|', '-' );

  move( LINES - CHATSIZE - 2, 0 );
  addch( ACS_LTEE );

  move( LINES - CHATSIZE - 2, COLS -1 );
  addch( ACS_RTEE );
}

