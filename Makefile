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

# server
SERVER_DIST = ${DIST_DIR}/server

# client
NCURSES_CLIENT_DIST = ${DIST_DIR}/ncurses-client

# source files
SERVER_SRC = ${SERVER_DIR}/server.c
NCURSES_CLIENT_SRC = ${NCURSES_CLIENT_DIR}/client.c

${DIST_DIR}:
	@@mkdir -p ${DIST_DIR}

server: ${DIST_DIR}
	@@echo "Building " ${SERVER_DIST}
	@@cp -r ${SERVER_DIR} ${SERVER_DIST}
	@@$(call set_port, ${SERVER_DIST}/server.c)
	@@cc ${SERVER_DIST}/server.c -o ${DIST_DIR}/s

${SERVER_DIST}: ${SERVER_SRC} ${DIST_DIR}
	@@echo "Building" ${SERVER_DIST}
	@@cat ${SERVER_SRC} ${SERVER_DIST}

client: ${DIST_DIR}
	@@echo "Building " ${NCURSES_CLIENT_DIST}
	@@cp -r ${NCURSES_CLIENT_DIR} ${NCURSES_CLIENT_DIST}
	@@$(call set_ip, ${NCURSES_CLIENT_DIST}/client.c)
	@@$(call set_port, ${NCURSES_CLIENT_DIST}/client.c)
	@@cc ${NCURSES_CLIENT_DIST}/client.c -lncurses -o ${DIST_DIR}/nc

all: clean server client

clean:
	@@echo "Removing dist directory:" ${DIST_DIR}
	@@rm -rf ${DIST_DIR}

