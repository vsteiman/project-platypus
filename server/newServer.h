#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/select.h>
#include <string>

using namespace std;

class GameServer {
  private:
    fd_set _readfds;
    fd_set _testfds;
    int _port;
    int _num_connected;
    char _msg[1024];
    int _server_sockfd;
    bool _connected;

    // die needs to be called through a wrapper function if
    // it is to be used as a signal handler, becuase of security limitations.
    void die();
    void close_sockets();
    void run_select();
    void disconnect_client( int );
    void log( string );
    void handle_client( int );
    void handle_new_client();
    void handle_server_stdin();
  public:
    GameServer( int );
    ~GameServer();
    bool broadcast( string );
    bool broadcast( int, string );
    bool parse_message( int, string );
    bool connect();
    void disconnect();
};


#endif
