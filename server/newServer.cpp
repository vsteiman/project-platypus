#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "newServer.h"

GameServer::GameServer( int port ):
  _port( port),
  _connected( false ) {

};

GameServer::~GameServer() {
  if( _connected )
    disconnect();
};

bool GameServer::connect() {
  struct sockaddr_in server = { AF_INET, _port, INADDR_ANY };

  if ( ( _server_sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
    perror( "socket call failed" );
    return false;
  }
  if ( bind( _server_sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in) ) == -1 ) {
    perror( "bind call failed" );
    return false;
  }
  if( listen( _server_sockfd, 16 ) == -1 ) {
    perror( "listen call failed" );
    return false;
  }

  FD_ZERO( &_readfds );
  FD_SET( _server_sockfd, &_readfds );
  FD_SET( STDIN_FILENO, &_readfds );

  _connected = true;
  run_select();

  return true;
};

void GameServer::run_select() {
  int fd;  
  while( _connected ) {
    _testfds = _readfds;
    select( 4 + _num_connected, &_testfds, NULL, NULL, NULL );
    for ( fd = 0; fd < FD_SETSIZE; fd++ ) {
      if ( FD_ISSET( fd, &_testfds ) ) {
        if ( fd == _server_sockfd )
          handle_new_client();
        else if ( fd == STDIN_FILENO )
          handle_server_stdin();
        else
          handle_client( fd );
      }
    }
  }
};

void GameServer::handle_client( int sock_num ) {
  int nread,
      fd;  
  ioctl( sock_num, FIONREAD, &nread );
  if( nread == 0 ) {
    close( sock_num );
    FD_CLR( sock_num, &_readfds);
    sprintf( _msg, "Client %d disconnected\n", sock_num );
    log( string( _msg ) );
  } else {
    nread = read( sock_num, _msg, nread );
    _msg[ nread ] = '\0';
    broadcast( sock_num, string( _msg ) );
  }
};

void GameServer::handle_new_client() {
  int new_fd;  
  log( string( "accepting client...\n" ) );
  new_fd = accept( _server_sockfd, NULL, NULL );
  FD_SET( new_fd, &_readfds );
  sprintf( _msg,  "client has connected on fd#: %d\n", new_fd );
  log( string( _msg ) );
};

void GameServer::handle_server_stdin() {
  int nread;  
  ioctl( STDIN_FILENO, FIONREAD, &nread );
  if( nread > 0 ) {
    nread = read( STDIN_FILENO, _msg, nread );
    if ( _msg[ 0 ] != '\n' ) {
      _msg[ nread ] = '\0';

      broadcast( string( _msg ) );
    }
  }
};

void GameServer::die() {
  signal( SIGINT, SIG_IGN );
  disconnect();
};

void GameServer::disconnect_client( int client_sock ) {
  // network-core would be useful here.
  close( client_sock );  
  FD_CLR( client_sock, &_readfds );
  _connected = false;
};

void GameServer::close_sockets() {
  int fd;
  for ( fd = 4; fd < 4 + _num_connected; fd++ ) {
    if ( FD_ISSET( fd, &_readfds ) ) {
      disconnect_client( fd );
    }
  }
};

bool GameServer::broadcast( int targ_fd, string message ) {
  // will not broadcast to targ_fd (if message originated from targ_fd
  // use broadcast( string ) to send to all
  if( targ_fd < 0 || message.empty() ) {
    return false;
  }
  int fd;

  for ( fd = 4; fd < 4 + _num_connected; fd++ ) {
    if ( FD_ISSET( fd, &_readfds ) && targ_fd != fd ) {
      //use network-core here :D
    }
  }
};

bool GameServer::broadcast( string message ) {
  // broadcast message to all client fd's
  broadcast( -1, message );
};

void GameServer::disconnect() {
  close_sockets();
};

void GameServer::log( string message ) {
  if( message.empty() )
    return;

  printf( "%s\n", message.c_str() );
};

bool GameServer::parse_message( int client_sock, string message ) {
  
  //best handled in network-core?

};


