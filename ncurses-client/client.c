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

#define IP "127.0.0.1"
#define PORT 9000
#define SIZE sizeof(struct sockaddr_in)

main() {
  int sockfd, result, fd, c;
  char message[BUFSIZ];
  fd_set readfds, testfds;
  
  struct sockaddr_in server = { AF_INET, PORT };

   server.sin_addr.s_addr = inet_addr( IP );
   if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
      perror( "socket call failed" );
      exit( 1 );
   }
   if( connect( sockfd, ( struct sockaddr * ) &server, SIZE ) == -1 ) {
      perror( "connect call failed" );
      exit( 1 );
   }
   
  initCurses();
  
  box(stdscr,'|','-');
  move( 10,10);
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
          c = getch();
          switch ( c ) {
            case KEY_DOWN:
              if ( stdscr->_cury < LINES-3 ) {
                stdscr->_cury++;
                addch('X');
              }
              break;
            case KEY_UP:
              //write(sockfd, "KEY_UP PRESSED", 15);
              if ( !( stdscr->_cury < 2) ) {
                stdscr->_cury--;
                addch('X');
              } 
              break;
            case KEY_LEFT:
              //write(sockfd, "KEY_LEFT PRESSED", 17);
              if ( !( stdscr->_curx < 2) ) {
                stdscr->_curx--;
              }
              break;
            case KEY_RIGHT:
              //write(sockfd, "KEY_RIGHT PRESSED", 18);
              if ( !( stdscr->_curx > COLS-3) ) {
                stdscr->_curx++;
                addch('X');
              }
              break;
          }
          refresh();
          
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
  /*for (;;) {
  drawScreen();  
  update();
  usleep(1000);
  }*/
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

drawScreen() {
  box(stdscr,'|','-');
  mvaddstr(0,2," Project Platypus ");
}

