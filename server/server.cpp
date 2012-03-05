#include <ctype.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "server.h"
#include "global.h"

GameServer::GameServer(): _connected( false ) { };

GameServer::~GameServer() {
  if( _connected )
    close_sockets();
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

  log( string( "Server Ready" ) );
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
    sprintf( _msg, "Client %d disconnected", sock_num );
    log( string( _msg ) );
  } else {
    nread = read( sock_num, _msg, nread );
    _msg[ nread ] = '\0';
    string message = string( _msg );
    log( message );
    broadcast( sock_num, message );
  }
};

void GameServer::handle_new_client() {
  int new_fd;  
  new_fd = accept( _server_sockfd, NULL, NULL );
  if ( new_fd > 3 ) {
    FD_SET( new_fd, &_readfds );
    sprintf( _msg,  "client has connected on fd#: %d", new_fd );
    log( string( _msg ) );
    _num_connected++;
  }
};

void GameServer::handle_server_stdin() {
  int nread;  
  ioctl( STDIN_FILENO, FIONREAD, &nread );
  if( nread > 0 ) {
    nread = read( STDIN_FILENO, _msg, nread );
    if ( _msg[ 0 ] != '\n' ) {
      _msg[ nread ] = '\0';
      string message = string( _msg );
      message.insert( 0, ": " );
      message.insert( 0, "Server" );
      broadcast( message );
    }
  }
};

void GameServer::die() {
  _connected = false;
  close_sockets();
  close( _server_sockfd );
};

void GameServer::disconnect_client( int client_sock ) {
  // network-core would be useful here.
  close( client_sock );  
  FD_CLR( client_sock, &_readfds );
  _num_connected--;
};

void GameServer::close_sockets() {
  int fd;
  for ( fd = 4; fd < FD_SETSIZE; fd++ )
    if ( FD_ISSET( fd, &_readfds ) )
      disconnect_client( fd );
};

bool GameServer::broadcast( int targ_fd, string message ) {
  // will not broadcast to targ_fd (if message originated from targ_fd
  // use broadcast( string ) to send to all
  if( message.empty() )
    return false;

  int fd;

  for ( fd = 4; fd < 4 + _num_connected; fd++ )
    if ( FD_ISSET( fd, &_readfds ) && targ_fd != fd )
      //use network-core here :D
      write(fd, message.c_str(), message.size() );
};

bool GameServer::broadcast( string message ) {
  // broadcast message to all client fd's
  broadcast( -1, message );
};

void GameServer::log( string message ) {
  if( message.empty() )
    return;

  printf( "%s\n", message.c_str() );
};

bool GameServer::parse_message( int client_sock, string message ) {
  
  //best handled in network-core?

};

bool GameServer::isConnected() {
  return _connected;
};

void GameServer::setPort( int p ) {
  if ( p )
    _port = p;
}

GameServer gs = GameServer();

void sigHandler() {
  signal( SIGINT, SIG_IGN );
  gs.die();
}

int main() {
  gs.setPort( PORT );

  signal(SIGINT,(__sighandler_t)sigHandler);

  gs.connect();

};
