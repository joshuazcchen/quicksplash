CC ?= gcc
CFLAGS ?= -Wall -g -Iinclude
LDFLAGS ?=
LDLIBS ?=

SERVER_SRCS := \
	src/server/init.c \
	src/server/cards.c \
	src/server/gamestates.c \
	src/server/gamehandler.c \
	src/server/server_comms.c \
	src/server/lobby.c \
	src/common/socket.c \
	src/common/comms.c \
	src/common/protocol.c

CLIENT_SRCS := \
	src/client/init.c \
	src/client/client_comms.c \
	src/client/input.c \
	src/client/output.c \
	src/common/protocol.c \
	src/common/comms.c \
	src/common/socket.c

SERVER_TEST_SRCS := \
	test/server_test.c \
	test/server_test_support.c \
	src/server/cards.c \
	src/server/gamehandler.c \
	src/server/gamestates.c \
	src/common/protocol.c

CLIENT_TEST_SRCS := \
	test/client_test.c \
	src/common/socket.c \
	src/common/comms.c \
	src/client/output.c \
	src/client/input.c

.PHONY: all clean test

all: server client

test: clean test_client server_test

server: $(SERVER_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

client: $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

server_test: $(SERVER_TEST_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test_client: $(CLIENT_TEST_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f server client server_test test_client
