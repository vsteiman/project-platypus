#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE sizeof(struct sockaddr_in)
#define PORT @PORT

main()
{

  int server_sockfd, client_sockfd, socks[ 16 ], die();
	char msg[1024] = "";
  struct sockaddr_in server = { AF_INET, PORT, INADDR_ANY };
  fd_set readfds, testfds;
  
  signal(SIGINT,(__sighandler_t)die);

  if ( ( server_sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    perror( "socket call failed" );
    exit( 1 );
  }
  if ( bind( server_sockfd, (struct sockaddr *)&server, SIZE ) == -1 ) {
    perror( "bind call failed" );
    exit( 1 );
  }
  if( listen( server_sockfd, 16 ) == -1 ) {
    perror( "listen call failed" );
    exit( 1 );
  }
  
  FD_ZERO( &readfds );
  FD_SET( server_sockfd, &readfds );
  printf( "Server started. Waiting for connections...\n" );
  while( 1 ) {
    int nread, fd;
    testfds = readfds;
    select( FD_SETSIZE, &testfds, NULL, NULL, NULL);
		
    for ( fd = 0; fd < FD_SETSIZE; fd++ ) {
      if ( FD_ISSET( fd, &testfds ) ) {
        if ( fd == server_sockfd ) {
          printf( "accepting client...\n" );
          client_sockfd = accept(server_sockfd, NULL, NULL);
          FD_SET(client_sockfd, &readfds);
          printf( "client has connected\n" );
        } else if ( fd == STDIN_FILENO ) {
          //admin keyboard activity
        
        } else {
          // this checks how many bytes are immediately available to be read
          ioctl( fd, FIONREAD, &nread );
          
          if( nread == 0 ) {
            // assume the client is disconnecting
          } else {
            nread = read( fd, msg, nread );
						msg[ nread ] = '\0';
						printf( "%s\n", msg );
						fflush( stdout );
          }
        }
      }
    }
  }
}

die() {
  signal(SIGINT,SIG_IGN);
  close( 3 );
  exit(0);
}
