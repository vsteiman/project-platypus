#ifndef __GLOBAL_H__
#define __GLOBAL_H__

// Global structs and definitions

#define PORT @PORT
#define IPADDRESS "@IPADDR"
#define SIZE sizeof(struct sockaddr_in)
#define CHATSIZE 15

struct character {
	int x, y;
	char *name;
};

#endif

