PREFIX = .
DIST_DIR = ${PREFIX}/dist
SERVER_DIR = ${PREFIX}/server
NCURSES_CLIENT_DIR = ${PREFIX}/ncurses-client

# SERVERIP - will replace the string @IPADDR in the server. default to 127.0.0.1
IPADDR = 127\.0\.0\.1

PORT = 9000


# Set the server IP in server code
set_ip = cat $(1) | sed -e 's/@IPADDR/${IPADDR}/' > $(1).__tmp__ ; \
                    mv $(1).__tmp__ $(1)

# Set the Port number
set_port = cat $(1) | sed -e 's/@PORT/${PORT}/' > $(1).__tmp__ ; \
                    mv $(1).__tmp__ $(1)

# source files
SERVER_SRC = ${SERVER_DIR}/server.cpp
NCURSES_CLIENT_SRC = ${NCURSES_CLIENT_DIR}/nclient.cpp
GLOBAL_H = ${PREFIX}/global.h
SERVER_H = ${SERVER_DIR}/server.h
NCURSES_CLIENT_H = ${NCURSES_CLIENT_DIR}/nclient.h

# Destinations
SERVER_SRC_DEST = ${DIST_DIR}/server.c
NCURSES_CLIENT_SRC_DEST = ${DIST_DIR}/nclient.cpp
GLOBAL_H_DEST = ${DIST_DIR}/global.h
SERVER_H_DEST = ${DIST_DIR}/server.h
NCURSES_CLIENT_H_DEST = ${DIST_DIR}/nclient.h

# Binaries
SERVER_BIN = ${DIST_DIR}/s
NCLIENT_BIN = ${DIST_DIR}/nc

all: clean build

${DIST_DIR}:
	@@mkdir -p ${DIST_DIR}

build: ${DIST_DIR}
	@@echo "Building Server"
	@@cp -r ${SERVER_SRC} ${SERVER_SRC_DEST}
	@@cp -r ${GLOBAL_H} ${GLOBAL_H_DEST}
	@@$(call set_port, ${GLOBAL_H_DEST})
	@@$(call set_ip, ${GLOBAL_H_DEST})
	@@cp -r ${SERVER_H} ${SERVER_H_DEST}
	@@c++ ${SERVER_SRC_DEST} -o ${SERVER_BIN}
	@@echo "Building Client"
	@@cp -r ${NCURSES_CLIENT_SRC} ${NCURSES_CLIENT_SRC_DEST}
	@@cp -r ${NCURSES_CLIENT_H} ${NCURSES_CLIENT_H_DEST}
	@@c++ ${NCURSES_CLIENT_SRC_DEST} -lncurses -o ${NCLIENT_BIN}

clean:
	@@echo "Removing dist directory:" ${DIST_DIR}
	@@rm -rf ${DIST_DIR}

