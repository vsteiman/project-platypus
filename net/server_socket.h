// Project Platypus
// server_socket.h - holds ServerSocketArray and ServerSocketHandler class definitions

#ifndef PLA_NET_SERVER_SOCKET_H
#define PLA_NET_SERVER_SOCKET_H

#include "socket.h"

class GameServer;
class NetPacket;

enum ServerStatus
{
   STATUS_SERVER_INACTIVE,        // client is just connected and not authorized
   STATUS_SERVER_AUTHORIZED,      // client is authorized
   STATUS_SERVER_JOINING,         // client tries to join
   STATUS_SERVER_ACTIVE,          // client is active in the game
   STATUS_SERVER_END              // always at the end
};

class ServerSocketHandler : public SocketHandler
{
   // network address of a client (for admin reasons)
   // sockaddr_in _net_addr;

   // status of a client for protection reasons
   ServerStatus _status;
   
   // client is associated with this server
   GameServer* _serv;

   // private constructor, so only ServerSocketArray
   // can create an instance of ServerSocketHandler

   ServerSocketHandler(int socket, GameServer* serv);
   
   public:
   
   virtual ~ServerSocketHandler();   
   bool HandlePacket(NetPacket *p);
   bool RecvAck(NetPacket* p);
   bool SendChatMsg(const char* msg);
   bool RecvChatMsg(NetPacket* p);
   bool RecvClientLogin(NetPacket* p);
   bool RecvClientMovement(NetPacket* p);
   
   friend class ServerSocketArray;
};

class ServerSocketArray
{
   // array of ptrs to DYNAMICALLY allocated SocketHandlers
   ServerSocketHandler* _client_sock[MAX_CLIENTS];

   // length of an array
   uint _length;
   
   // array is associated with this server
   GameServer* _serv;

   public:
   // default constructor of an empty array
   ServerSocketArray(GameServer* serv);

   // deletes SocketHandlers
   ~ServerSocketArray();

   // add a client to an array
   bool AddClient(int socket);

   // remove a client from an array, automatically moves others
   bool RemoveClient(int socket);
   
   // wipe everything
   void ClearList();

   // returns current number of clients in the array
   int Length();
   
   // get a client pointer from an array
   ServerSocketHandler* GetClient (const int sockfd);

   // we can cast a class to an int
   // returns current number of clients in the array
   operator int();

   // operator [] to work with this class as an simple array
   ServerSocketHandler* operator [] (const int sockfd);
};


#endif /* PLA_NET_SERVER_SOCKET_H */

